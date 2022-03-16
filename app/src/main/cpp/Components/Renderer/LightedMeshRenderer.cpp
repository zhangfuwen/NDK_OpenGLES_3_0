//
// Created by zhangfuwen on 2022/3/2.
//

#include "LightedMeshRenderer.h"
#ifdef ANDROID
#include <android/log.h>

#define LOG_TAG "ByteFlow"
#define FUN_PRINT(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, ##__VA_ARGS__)
#endif

#include "handycpp/logging.h"
#include "LightedMeshRenderer.h"
#include "Components/Transform.h"
#include <handycpp/logging.h>
#include <handycpp/file.h>
#include <GLUtils.h>
#include <happly.h>
#include <glm/ext.hpp>
#include <handycpp/image.h>

#define UNIFORM(uniform_name) FUN_INFO(#uniform_name": %d", m_program->uniformLocation(#uniform_name))

void LightedMeshRenderer::printBunnyVars() {
    FUN_INFO("program:%u", m_program->ID);
    FUN_INFO("Position:%d", m_VertexAttribPosition);
    FUN_INFO("Normal:%d", m_VertexAttribNormal);
    FUN_INFO("TexCoord:%d", m_VertexAttribTexCoord);
    UNIFORM(color_sampler);
    FUN_INFO("VBO:%d, VAO:%d", m_VBOPosition, m_VAO);
    FUN_INFO("numElements:%u", m_NumElements);
    FUN_INFO("m_colorTexture:%u", m_colorTexuture);
    UNIFORM(u_model);
    UNIFORM(u_view);
    UNIFORM(u_projection);
    UNIFORM(u_light_pos);
    auto s = glm::to_string(m_MVPMatrix);
    FUN_INFO("mvp: %s", s.c_str());
    printf("adf");
}


int LightedMeshRenderer::LoadTexturedMesh(ObjLoader *loader) {

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
        glGenTextures(1, &m_colorTexuture);
        glBindTexture(GL_TEXTURE_2D, m_colorTexuture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data.width, data.height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, &data.rgba_image[0]);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glActiveTexture(GL_TEXTURE0);

        m_program->setInt("color_sampler", 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        m_material.setKa(loader->materials[0].m_Ka);
        m_material.setKd(loader->materials[0].m_Kd);
        m_material.setKs(loader->materials[0].m_Ks);
        m_material.setKe(loader->materials[0].m_Ke);
        m_material.setMShininess(0.3f);
    } else {
        FUN_INFO("no texture file found");
    }

    return 0;
}

int LightedMeshRenderer::Init() {
#ifdef ANDROID
    std::string baseDir = "/sdcard/Android/data/com.byteflow.app/files/Download/";
#else
    std::string baseDir = RENDERER_SOURCE "/../assets/";
#endif
    auto vertexShaderFilePath = baseDir + "/shader_source/material_lighting.vs";
    auto fragmentShaderFilePath = baseDir + "/shader_source/material_lighting.fs";

    m_program = std::make_unique<RenderProgram>();

//    auto vertexShaderSrc = handycpp::file::readFile(vertexShaderFilePath);
//    FUN_INFO("compile %d,%s", vertexShaderSrc.size, vertexShaderSrc.addr);
//    auto fragmentShaderSrc = handycpp::file::readFile(fragmentShaderFilePath);
//    if(vertexShaderSrc.size == 0 || fragmentShaderSrc.size == 0) {
//        FUN_ERROR("zhangfuwen: failed to read file %d, %d", vertexShaderSrc.size, fragmentShaderSrc.size);
//        return -1;
//    }
//    int ret = m_program->InitWithSource((const char *)vertexShaderSrc.addr, (const char *)fragmentShaderSrc.addr);
//    if(ret < 0) {
//        FUN_ERROR("zhangfuwen: failed to init program");
//        return -1;
//    }

    std::string source1, source2;
    handycpp::file::for_each_line(vertexShaderFilePath, [&](int n_lines, std::string line) {
        FUN_ERROR("compiles %d: %s", n_lines, line.c_str());
        source1 = source1 + line + "\n";

    });

    handycpp::file::for_each_line(fragmentShaderFilePath, [&](int n_lines, std::string line) {
        FUN_DEBUG("compiles %d: %s", n_lines, line.c_str());
        source2 = source2 + line + "\n";

    });
    GO_CHECK_GL_ERROR();

    int ret = m_program->InitWithSource(source1.c_str(), source2.c_str());
    if(ret < 0) {
        FUN_ERROR("compile: failed to init program");
        return -1;
    }
    GO_CHECK_GL_ERROR();


    m_VertexAttribPosition = glGetAttribLocation(m_program->ID, "a_Position");
    FUN_ERROR("zhangfuwen pos:%d", m_VertexAttribPosition);
    GO_CHECK_GL_ERROR();
    m_VertexAttribTexCoord = glGetAttribLocation(m_program->ID, "a_TexCoord");
    FUN_ERROR("zhangfuwen tex:%d", m_VertexAttribTexCoord);
    GO_CHECK_GL_ERROR();
    auto x = glGetAttribLocation(m_program->ID, "a_Normal");
    FUN_ERROR("zhangfuwen normal:%d", x);
    m_VertexAttribNormal = x;
    GO_CHECK_GL_ERROR();

//    m_VertexAttribPosition = 0;
//    m_VertexAttribTexCoord = 1;
//    m_VertexAttribNormal = 2;

    glGenBuffers(1, &m_VBOPosition);
    glGenVertexArrays(1, &m_VAO);

    return 0;

}

int LightedMeshRenderer::Finalize() {
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

int LightedMeshRenderer::Draw(const Transform &transform, const Camera &camera, const std::vector<Light> &lights) {
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
    glLineWidth(1.0f);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);


    if (m_colorTexuture != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_colorTexuture);
    }
    m_program->use();
    m_program->setMat4("u_model", transform.GetModel());
    m_program->setMat4("u_view", camera.GetView());
    m_program->setMat4("u_projection", camera.GetProjection());
    m_program->setVec3("u_light_pos",lights[0].getLightPos());
    m_program->setVec3("u_view_pos", camera.GetViewPos());
//    m_program->setInt("u_color_sampler", 0);
    m_program->setVec3("u_light.ambient_color", lights[0].getAmbientColor());
    m_program->setVec3("u_light.diffuse_color", lights[0].getDiffuseColor());
    m_program->setVec3("u_light.specular_color", lights[0].getSpecularColor());
    m_program->setVec3("u_material.ambient_ratio", m_material.getKa());
    m_program->setVec3("u_material.diffuse_ratio", m_material.getKd());
    m_program->setVec3("u_material.specular_ratio", m_material.getKs());
    m_program->setFloat("u_material.shininess", m_material.getShininess());
//    m_MVPMatrix = camera.GetProjection() * camera.GetView() * transform.GetModel();
//    glUniformMatrix4fv(glGetUniformLocation(m_program->ID,  "u_MVPMatrix"), 1, GL_FALSE, &m_MVPMatrix[0][0]);

//#define DUMP_VEC3(title, vec3) FUN_INFO(#title ": %f, %f, %f", vec3.r, vec3.g, vec3.b)
//    DUMP_VEC3("ka:", m_material.getKa());
//    DUMP_VEC3("kd:", m_material.getKa());
//    DUMP_VEC3("ks:", m_material.getKa());
//    FUN_INFO("shininess:%f", m_material.getShininess());


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

