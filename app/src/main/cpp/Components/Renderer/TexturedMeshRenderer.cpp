//
// Created by zhangfuwen on 2022/3/2.
//

#include "TexturedMeshRenderer.h"
#include <android/log.h>

#define LOG_TAG "ByteFlow"
#define FUN_PRINT(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, ##__VA_ARGS__)

#include "handycpp/logging.h"
#include "TexturedMeshRenderer.h"
#include "Components/Transform.h"
#include <handycpp/logging.h>
#include <handycpp/file.h>
#include <GLUtils.h>
#include <happly.h>
#include <glm/ext.hpp>
#include <handycpp/image.h>


void TexturedMeshRenderer::printBunnyVars() {
    FUN_INFO("program:%u", m_ProgramObj);
    FUN_INFO("vertexshader:%d", m_VertexShader);
    FUN_INFO("fragment:%d", m_FragmentShader);
    FUN_INFO("Position:%d", m_VertexAttribPosition);
    FUN_INFO("Normal:%d", m_VertexAttribNormal);
    FUN_INFO("TexCoord:%d", m_VertexAttribTexCoord);
    FUN_INFO("ColorSampler:%d", m_colorSampler);
    FUN_INFO("VBO:%d, VAO:%d", m_VBOPosition, m_VAO);
    FUN_INFO("numElements:%u", m_NumElements);
    FUN_INFO("mvpLoc:%u", m_MVPUniformLoc);
    auto s = glm::to_string(m_MVPMatrix);
    FUN_INFO("mvp: %s", s.c_str());
    printf("adf");
}


int TexturedMeshRenderer::LoadTexturedMesh(ObjLoader *loader) {

    loader->LoadObjFile();
    loader->Dump();

    triangles.reserve(loader->faces.size() * 2); // every face is translated into two triangles
    for (const auto &face : loader->faces) {
        const auto &p0 = loader->vertices[face[0].vertex_index];
        const auto &p1 = loader->vertices[face[1].vertex_index];
        const auto &p2 = loader->vertices[face[2].vertex_index];
        const auto &p3 = loader->vertices[face[3].vertex_index];

        const auto &n0 = loader->normals[face[0].normal_index];
        const auto &n1 = loader->normals[face[1].normal_index];
        const auto &n2 = loader->normals[face[2].normal_index];
        const auto &n3 = loader->normals[face[3].normal_index];

        const auto &t0 = loader->texCoords[face[0].tex_coord_index];
        const auto &t1 = loader->texCoords[face[1].tex_coord_index];
        const auto &t2 = loader->texCoords[face[2].tex_coord_index];
        const auto &t3 = loader->texCoords[face[3].tex_coord_index];

        triangles.emplace_back(Triangle{
                Vertex{.pos = p0, .normal = n0, .tex_coord = t0},
                Vertex{.pos = p1, .normal = n1, .tex_coord = t1},
                Vertex{.pos = p2, .normal = n2, .tex_coord = t2}
        });

        triangles.emplace_back(Triangle{
                Vertex{.pos = p0, .normal = n0, .tex_coord = t0},
                Vertex{.pos = p2, .normal = n2, .tex_coord = t2},
                Vertex{.pos = p3, .normal = n3, .tex_coord = t3}
        });
    }
    m_NumElements = triangles.size() * 3; // every triangle has 3 points


    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOPosition);
    glBufferData(GL_ARRAY_BUFFER, m_NumElements * 8 * sizeof(float), &triangles[0].v1.pos,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(m_VertexAttribPosition, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *) 0);
    glVertexAttribPointer(m_VertexAttribNormal, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *) (3 * sizeof(float)));
    glVertexAttribPointer(m_VertexAttribTexCoord, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *) (6 * sizeof(float)));

    glEnableVertexAttribArray(m_VertexAttribPosition);
    glEnableVertexAttribArray(m_VertexAttribNormal);
    glEnableVertexAttribArray(m_VertexAttribTexCoord);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    if (!loader->materials.empty() &&
        loader->materials[0].m_textureFiles.count("map_Ka")) {
        std::string textureFileName = loader->materials[0].m_textureFiles["map_Ka"];
        auto data = handycpp::image::readPngAsRgba(textureFileName);
        FUN_INFO("map_Ka: %d, %d", data.width, data.height);
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &m_colorTexutre);
        glBindTexture(GL_TEXTURE_2D, m_colorTexutre);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data.width, data.height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, &data.rgba_image[0]);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glActiveTexture(GL_TEXTURE0);

        glUniform1ui(m_colorSampler, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    } else {
        FUN_INFO("no texture file found");
    }
    return 0;
}

int TexturedMeshRenderer::Init() {
    const char VertexShaderSrc[] =
            "#version 300 es                            \n"
            "layout(location = 0) in vec3 a_Position;\n"
            "layout(location = 1) in vec3 a_Normal;\n"
            "layout(location = 2) in vec2 a_TexCoord;\n"
            "uniform mat4 u_MVPMatrix;                  \n"
            "out highp float zDepth; \n"
            "out mediump vec2 texCoord; \n"
            "void main() {\n"
            "	gl_Position=u_MVPMatrix * vec4(a_Position, 1.0f);\n"
            "	zDepth = gl_Position.z/gl_Position.w;\n"
            "   texCoord = vec2(a_TexCoord.x, 1.0f - a_TexCoord.y); \n"
            "}\n";

    const char FragmentShaderSrc[] =
            "#version 300 es                            \n"
            "precision mediump float;\n"
            " layout(location = 0) uniform sampler2D color_sampler; \n "
            "in highp float zDepth; \n"
            "in mediump vec2 texCoord; \n"
            "layout(location = 0) out vec4 outColor;\n"
            "void main(){\n"
            "	vec4 color = texture(color_sampler, texCoord);\n"
            "   outColor = vec4(vec3(color), 1.0f); \n"
            "}\n";

    // 编译链接用于渲染兔子的着色器程序
    m_ProgramObj = GLUtils::CreateProgram(VertexShaderSrc, FragmentShaderSrc,
                                          m_VertexShader, m_FragmentShader);
    m_VertexAttribPosition = glGetAttribLocation(m_ProgramObj, "a_Position");
    m_VertexAttribNormal = glGetAttribLocation(m_ProgramObj, "a_Normal");
    m_VertexAttribTexCoord = glGetAttribLocation(m_ProgramObj, "a_TexCoord");
    m_VertexAttribNormal = 1;
    m_VertexAttribTexCoord = 2;
    m_MVPUniformLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
    m_colorSampler = glGetUniformLocation(m_ProgramObj, "color_sampler");

    if (m_colorSampler != 0) {
        FUN_ERROR("color_sampler:%d", m_colorSampler);
        m_colorSampler = 0;
    }
    glGenBuffers(1, &m_VBOPosition);
    glGenVertexArrays(1, &m_VAO);


    return 0;

}

int TexturedMeshRenderer::Finalize() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
        m_ProgramObj = 0;
    }
    if (m_VertexShader) {
        glDeleteShader(m_VertexShader);
        m_VertexShader = 0;
    }
    if (m_FragmentShader) {
        glDeleteShader(m_FragmentShader);
        m_FragmentShader = 0;
    }
    if (m_VAO) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
    if (m_VBOPosition) {
        glDeleteBuffers(1, &m_VBOPosition);
        m_VBOPosition = 0;
    }
    return 0;
}

int TexturedMeshRenderer::Draw(const Transform &transform) {
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
    printBunnyVars();
    static float x = 0;
//	x += 1;
    transform.GetMVPMatrix(m_MVPMatrix);
    glLineWidth(1.0f);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);


    if (m_colorTexutre != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_colorTexutre);
    }
    glUseProgram(m_ProgramObj);
    glUniformMatrix4fv(m_MVPUniformLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, m_NumElements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
#endif
    return 0;
}

