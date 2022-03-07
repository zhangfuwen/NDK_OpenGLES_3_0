//
// Created by zhangfuwen on 2022/3/2.
//

#include <android/log.h>

#define LOG_TAG "ByteFlow"
#define FUN_PRINT(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, ##__VA_ARGS__)

#include "handycpp/logging.h"
#include "Components//Transform.h"
#include <handycpp/logging.h>
#include <handycpp/file.h>
#include <GLUtils.h>
#include <happly.h>
#include <glm/ext.hpp>
#include <handycpp/image.h>
#include "PointRenderer.h"


void PointRenderer::printBunnyVars() {
    FUN_INFO("program:%u", m_ProgramObj);
    FUN_INFO("vertexshader:%d", m_VertexShader);
    FUN_INFO("fragment:%d", m_FragmentShader);
    FUN_INFO("Position:%d", m_VertexAttribPosition);
    FUN_INFO("VBO:%d, VAO:%d", m_VBOPosition, m_VAO);
    FUN_INFO("numElements:%u", m_NumElements);
    FUN_INFO("mvpLoc:%u", m_MVPUniformLoc);
    auto s = glm::to_string(m_MVPMatrix);
    FUN_INFO("mvp: %s", s.c_str());
    printf("adf");
}


int PointRenderer::LoadPoints(PointLoader loader) {
    auto ret = loader(points, m_NumElements);
    if(ret <= 0) {
        return ret;
    }
    glGenBuffers(1, &m_VBOPosition);
    glGenVertexArrays(1, &m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBOPosition);
    glBufferData(GL_ARRAY_BUFFER, points.size() * 3 * sizeof(float), &points[0][0], GL_STATIC_DRAW);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBOPosition);
    glVertexAttribPointer(m_VertexAttribPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *) 0);
    glEnableVertexAttribArray(m_VertexAttribPosition);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return points.size();
}

int PointRenderer::Init() {
    const char VertexShaderSrc[] =
            "#version 300 es                            \n"
            "layout(location = 0) in vec3 a_Position;\n"
            "layout(location = 1) in float a_PointSize; \n"
            "uniform mat4 u_MVPMatrix;                  \n"
            "void main() {\n"
            "   gl_PointSize = a_PointSize;\n"
            "	gl_Position=u_MVPMatrix * vec4(a_Position, 1.0f);\n"
            "}\n";
    /*
     * fragment shader built-in variables
     *   in vec4 gl_FragCoord;
     *   in bool gl_FrontFacing;
     *   in vec2 gl_PointCoord;
    */
    const char FragmentShaderSrc[] =
            "#version 300 es                            \n"
            "precision mediump float;\n"
            "layout(location = 0) out vec4 outColor;\n"
            "in float depth;\n"
            "void main(){\n"
            "   outColor = vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1.0f); \n"
            "}\n";

    // 编译链接用于渲染兔子的着色器程序
    PointRenderer::m_ProgramObj = GLUtils::CreateProgram(VertexShaderSrc, FragmentShaderSrc,
                                                             PointRenderer::m_VertexShader, PointRenderer::m_FragmentShader);
    PointRenderer::m_VertexAttribPosition = glGetAttribLocation(PointRenderer::m_ProgramObj, "a_Position");
    m_Po
//    if(m_VertexAttribPosition <0) {
//        m_VertexAttribPosition = 0;
//    }
    PointRenderer::m_MVPUniformLoc = glGetUniformLocation(PointRenderer::m_ProgramObj, "u_MVPMatrix");

    return 0;

}

int PointRenderer::Finalize() {
    if (PointRenderer::m_ProgramObj) {
        glDeleteProgram(PointRenderer::m_ProgramObj);
        PointRenderer::m_ProgramObj = 0;
    }
    if (PointRenderer::m_VertexShader) {
        glDeleteShader(PointRenderer::m_VertexShader);
        PointRenderer::m_VertexShader = 0;
    }
    if (PointRenderer::m_FragmentShader) {
        glDeleteShader(PointRenderer::m_FragmentShader);
        PointRenderer::m_FragmentShader = 0;
    }
    if (PointRenderer::m_VAO) {
        glDeleteVertexArrays(1, (const GLuint *)&m_VAO);
        PointRenderer::m_VAO = 0;
    }
    if (PointRenderer::m_VBOPosition) {
        glDeleteBuffers(1, &m_VBOPosition);
        PointRenderer::m_VBOPosition = 0;
    }
    return 0;
}

int PointRenderer::Draw(const Transform &transform, const Camera & camera, const std::vector<Light> & lights) {
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
    PointRenderer::printBunnyVars();
    static float x = 0;
//	x += 1;
    m_MVPMatrix = camera.GetProjection() * camera.GetView() * transform.GetModel();
    glDisable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);


    glUseProgram(PointRenderer::m_ProgramObj);
    glUniformMatrix4fv(PointRenderer::m_MVPUniformLoc, 1, GL_FALSE, &PointRenderer::m_MVPMatrix[0][0]);
    glBindVertexArray(PointRenderer::m_VAO);
    glDrawArrays(GL_LINES, 0, PointRenderer::m_NumElements);
    glBindVertexArray(0);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
#endif
    return 0;
}

