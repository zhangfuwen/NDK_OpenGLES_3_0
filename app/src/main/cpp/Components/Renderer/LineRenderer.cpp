//
// Created by zhangfuwen on 2022/3/2.
//

#ifdef ANDROID
#include <android/log.h>
#define LOG_TAG "ByteFlow"
#define FUN_PRINT(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, ##__VA_ARGS__)
#endif


#include "handycpp/logging.h"
#include "Components//Transform.h"
#include <handycpp/logging.h>
#include <handycpp/file.h>
#include <GLUtils.h>
#include <happly.h>
#include <glm/ext.hpp>
#include <handycpp/image.h>
#include "LineRenderer.h"
#include "ShaderString.h"


void LineRenderer::printBunnyVars() {
    FUN_INFO("program:%u", m_program->ID);
    FUN_INFO("Position:%d", m_VertexAttribPosition);
    FUN_INFO("VBO:%d, VAO:%d", m_VBOPosition, m_VAO);
    FUN_INFO("numElements:%u", m_NumElements);
    FUN_INFO("mvpLoc:%u", m_MVPUniformLoc);
    auto s = glm::to_string(m_MVPMatrix);
    FUN_INFO("mvp: %s", s.c_str());
    printf("adf");
}

int LineRenderer::LoadLines(ObjLoader * loader) {
    return LoadLines([loader](decltype(lines) &lines) -> int {
        for (const auto &face : loader->faces) {
            auto p0 = loader->vertices[face[0].vertex_index];
            auto p1 = loader->vertices[face[1].vertex_index];
            auto p2 = loader->vertices[face[2].vertex_index];
            auto p3 = loader->vertices[face[3].vertex_index];
            lines.emplace_back(std::array{p0, p1});
            lines.emplace_back(std::array{p1, p2});
            lines.emplace_back(std::array{p2, p3});
            lines.emplace_back(std::array{p3, p0});
        }

        return lines.size();
    });
}

int LineRenderer::LoadLines(LineLoader loader) {
    auto ret = loader(lines);
    if(ret <= 0) {
        return ret;
    }
    m_NumElements = lines.size() * 2;
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(1, &m_VBOPosition);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOPosition);
    glBufferData(GL_ARRAY_BUFFER, lines.size() * 2 * 3 * sizeof(float), &lines[0][0][0], GL_STATIC_DRAW);
    glVertexAttribPointer(m_VertexAttribPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *) 0);
    glEnableVertexAttribArray(m_VertexAttribPosition);

    if(!colors.empty()) {
        glGenBuffers(1, &m_VBOColor);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBOColor);
        glBufferData(GL_ARRAY_BUFFER, colors.size()*2 *4 * sizeof(float), &colors[0][0][0], GL_STATIC_DRAW);
        glVertexAttribPointer(m_VertexAttribColor, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                              (void *) 0);
        glEnableVertexAttribArray(m_VertexAttribColor);
    } else {
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return lines.size();
}

int LineRenderer::Init() {
    const char VertexShaderSrc[] =
            "precision mediump float;\n"
            "layout(location = 0) in vec3 a_Position;\n"
            "#if SHARE_COLOR \n"
            "uniform vec4 u_Color; \n"
            "#else \n"
            "layout(location = 1) in vec4 a_Color; \n"
            "out vec4 v_Color; \n"
            "#endif\n"
            "uniform mat4 u_MVPMatrix;                  \n"
            "void main() {\n"
            "#if SHARE_COLOR \n"
            "#else \n"
            "    v_Color = u_Color;\n"
            "#endif \n"
            "	gl_Position=u_MVPMatrix * vec4(a_Position, 1.0f);\n"
            "}\n";
    /*
     * fragment shader built-in variables
     *   in vec4 gl_FragCoord;
     *   in bool gl_FrontFacing;
     *   in vec2 gl_PointCoord;
    */
    const char FragmentShaderSrc[] =
            "precision mediump float;\n"
            "layout(location = 0) out vec4 outColor;\n"
            "#if SHARE_COLOR \n"
            "uniform vec4 u_Color; \n"
            "#else \n"
            "out vec4 v_Color; \n"
            "#endif\n"
            "in float depth;\n"
            "void main(){\n"
            "#if SHARE_COLOR \n"
            "   outColor = vec4(vec3(u_Color)*gl_FragCoord.z, 1.0f); \n"
            "#else \n"
            "   outColor = v_Color; \n"
            "#endif \n"
            "}\n";

    std::string vSource;
    std::string fSource;
    if(colors.empty()) {
        vSource = MakeShaderString(0x30, "#define SHARE_COLOR 1\n", VertexShaderSrc);
        fSource = MakeShaderString(0x30, "#define SHARE_COLOR 1\n", FragmentShaderSrc);
    } else {
        vSource = MakeShaderString(0x30, "#define SHARE_COLOR 0\n", VertexShaderSrc);
        fSource = MakeShaderString(0x30, "#define SHARE_COLOR 0\n", FragmentShaderSrc);
    }
    // 编译链接用于渲染兔子的着色器程序
    m_program = std::make_unique<RenderProgram>();
    auto ret = m_program->InitWithSource(vSource, fSource);
    if(ret < 0) {
        FUN_ERROR("failed to init program");
        return ret;
    }

    m_VertexAttribPosition = glGetAttribLocation(m_program->ID, "a_Position");
    if(!colors.empty()) {
        m_VertexAttribColor = glGetAttribLocation(m_program->ID, "a_Color");
    } else {
        m_UniformColor = glGetUniformLocation(m_program->ID, "u_Color");
    }
    m_MVPUniformLoc = glGetUniformLocation(m_program->ID, "u_MVPMatrix");

    return 0;

}

int LineRenderer::Finalize() {
    if (LineRenderer::m_VAO) {
        glDeleteVertexArrays(1, (const GLuint *)&m_VAO);
        LineRenderer::m_VAO = 0;
    }
    if (LineRenderer::m_VBOPosition) {
        glDeleteBuffers(1, &m_VBOPosition);
        LineRenderer::m_VBOPosition = 0;
    }
    return 0;
}

int LineRenderer::Draw(const Transform &transform, const Camera & camera, const std::vector<Light> & lights) {
#if 0
    glUseProgram(m_FboProgramObj);
    glBindVertexArray(m_VaoIds[1]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
    glUniform1i(m_FboSamplerLoc, 0);
    GO_CHECK_GL_ERROR();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)0);
    GO_CHECK_GL_ERROR();
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
#else
    static float x = 0;
//	x += 1;
    m_MVPMatrix = camera.GetProjection() * camera.GetView() * transform.GetModel();
    glLineWidth(1.0f);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);


    m_program->use();
    glUniformMatrix4fv(LineRenderer::m_MVPUniformLoc, 1, GL_FALSE, &LineRenderer::m_MVPMatrix[0][0]);
    if(colors.empty()) {
        glUniform4f(m_UniformColor, m_sharedColor[0], m_sharedColor[1], m_sharedColor[2], m_sharedColor[3]);
    }
    glBindVertexArray(LineRenderer::m_VAO);
    glDrawArrays(GL_LINES, 0, LineRenderer::m_NumElements);
    glBindVertexArray(0);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
#endif
    return 0;
}

