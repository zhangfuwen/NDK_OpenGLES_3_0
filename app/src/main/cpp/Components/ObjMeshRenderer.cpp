//
// Created by zhangfuwen on 2022/2/24.
//
#include <android/log.h>

#define LOG_TAG "ByteFlow"
#define FUN_PRINT(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, ##__VA_ARGS__)
#include "handycpp/logging.h"
#include "ObjMeshRenderer.h"
#include "Transform.h"
#include <handycpp/logging.h>
#include <handycpp/file.h>
#include <GLUtils.h>
#include <happly.h>
#include <ext.hpp>
#include <handycpp/image.h>


void ObjMeshRenderer::printBunnyVars() {
    FUN_INFO("program:%u", m_bunnyProgramObj);
    FUN_INFO("vertexshader:%d", m_bunnyVertexShader);
    FUN_INFO("fragment:%d", m_bunnyFragmentShader);
    FUN_INFO("Position:%d", m_bunnyVertexAttribPosition);
    FUN_INFO("Normal:%d", m_bunnyVertexAttribNormal);
    FUN_INFO("TexCoord:%d", m_bunnyVertexAttribTexCoord);
    FUN_INFO("ColorSampler:%d", m_colorSampler);
    FUN_INFO("VBO:%d,%d,%d , VAO:%d", m_bunnyVBOPosition, m_bunnyVBONormal, m_bunnyVBOTexCoord, m_bunnyVAO);
    FUN_INFO("numElements:%u", m_bunnyNumElements);
    FUN_INFO("mvpLoc:%u", m_bunnyMVPUniformLoc);
    auto s = glm::to_string(m_bunnyMVPMatrix);
    FUN_INFO("mvp: %s", s.c_str());
    printf("adf");
}

bool ObjMeshRenderer::Init() {
    const char bunnyVertexShaderSrc[] =
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
            "   texCoord = a_TexCoord; \n"
            "}\n";

    const char bunnyFragmentShaderSrc[] =
            "#version 300 es                            \n"
            "precision mediump float;\n"
            " layout(location = 0) uniform sampler2D color_sampler; \n "
            "in highp float zDepth; \n"
            "in mediump vec2 texCoord; \n"
            "layout(location = 0) out vec4 outColor;\n"
            "void main(){\n"
            "	outColor = texture(color_sampler, texCoord);\n"
            "}\n";

    // 编译链接用于渲染兔子的着色器程序
        m_bunnyProgramObj = GLUtils::CreateProgram(bunnyVertexShaderSrc, bunnyFragmentShaderSrc, m_bunnyVertexShader, m_bunnyFragmentShader);
        m_bunnyVertexAttribPosition = glGetAttribLocation(m_bunnyProgramObj, "a_Position");
        m_bunnyVertexAttribNormal = glGetAttribLocation(m_bunnyProgramObj, "a_Normal");
        m_bunnyVertexAttribTexCoord = glGetAttribLocation(m_bunnyProgramObj, "a_TexCoord");
        m_bunnyVertexAttribNormal =1;
        m_bunnyVertexAttribTexCoord = 2;
        m_bunnyMVPUniformLoc = glGetUniformLocation(m_bunnyProgramObj, "u_MVPMatrix");
        m_colorSampler = glGetUniformLocation(m_bunnyProgramObj, "color_sampler");

        if(m_colorSampler != 0) {
            FUN_ERROR("color_sampler:%d", m_colorSampler);
            m_colorSampler = 0;
        }
//		m_bunnyVertexAttribPosition = 0;
        glGenBuffers(1, &m_bunnyVBOPosition);
        glGenBuffers(1, &m_bunnyVBONormal);
        glGenBuffers(1, &m_bunnyVBOTexCoord);
        glGenVertexArrays(1, &m_bunnyVAO);

        m_objLoader = new ObjLoader();
    m_objLoader->LoadObjFile();
//    m_objLoader->LoadObjFile("/sdcard/Android/data/com.byteflow.app/files/Download/model/poly/1.obj");
        m_objLoader->Dump();


        if(m_bunnyWireframe) {
            for(const auto & face : m_objLoader->faces) {
                lines.push_back(m_objLoader->vertices[face[0].vertex_index]);
                lines.push_back(m_objLoader->vertices[face[1].vertex_index]);
                lines.push_back(m_objLoader->vertices[face[1].vertex_index]);
                lines.push_back(m_objLoader->vertices[face[2].vertex_index]);
                lines.push_back(m_objLoader->vertices[face[2].vertex_index]);
                lines.push_back(m_objLoader->vertices[face[3].vertex_index]);
                lines.push_back(m_objLoader->vertices[face[3].vertex_index]);
                lines.push_back(m_objLoader->vertices[face[0].vertex_index]);
            }
            m_bunnyNumElements = m_objLoader->faces.size()*8;
        } else {

        for(const auto & face : m_objLoader->faces) {
            vertices.push_back(m_objLoader->vertices[face[0].vertex_index]);
            vertices.push_back(m_objLoader->vertices[face[1].vertex_index]);
            vertices.push_back(m_objLoader->vertices[face[2].vertex_index]);
            vertices.push_back(m_objLoader->vertices[face[0].vertex_index]);
            vertices.push_back(m_objLoader->vertices[face[2].vertex_index]);
            vertices.push_back(m_objLoader->vertices[face[3].vertex_index]);

            if(!m_objLoader->normals.empty()) {
                normals.push_back(m_objLoader->normals[face[0].normal_index]);
                normals.push_back(m_objLoader->normals[face[1].normal_index]);
                normals.push_back(m_objLoader->normals[face[2].normal_index]);
                normals.push_back(m_objLoader->normals[face[0].normal_index]);
                normals.push_back(m_objLoader->normals[face[2].normal_index]);
                normals.push_back(m_objLoader->normals[face[3].normal_index]);
            }

            if(!m_objLoader->texCoords.empty()) {
                texCoords.push_back(m_objLoader->texCoords[face[0].texCord_index]);
                texCoords.push_back(m_objLoader->texCoords[face[1].texCord_index]);
                texCoords.push_back(m_objLoader->texCoords[face[2].texCord_index]);
                texCoords.push_back(m_objLoader->texCoords[face[0].texCord_index]);
                texCoords.push_back(m_objLoader->texCoords[face[2].texCord_index]);
                texCoords.push_back(m_objLoader->texCoords[face[3].texCord_index]);
            }
        }
        m_bunnyNumElements = m_objLoader->faces.size()*6;

        }


        glBindBuffer(GL_ARRAY_BUFFER, m_bunnyVBOPosition);
        if(m_bunnyWireframe) {
            float * data = (float *)malloc(lines.size() *3 * sizeof(float));
            auto x = data;
            for(auto p : lines) {
                *x = p.x;
                *(x+1)= p.y;
                *(x+2) = p.z;
                x+=3;
            }
            glBufferData(GL_ARRAY_BUFFER, lines.size()*3*sizeof(float), data, GL_STATIC_DRAW);
        } else {
            glBufferData(GL_ARRAY_BUFFER, vertices.size()*3*sizeof(float), &vertices[0][0], GL_STATIC_DRAW);
        }

        if(!normals.empty()) {
            glBindBuffer(GL_ARRAY_BUFFER, m_bunnyVBONormal);
            glBufferData(GL_ARRAY_BUFFER, normals.size()*3*sizeof(float), &normals[0][0], GL_STATIC_DRAW);
        }
        if(!texCoords.empty()) {
            glBindBuffer(GL_ARRAY_BUFFER, m_bunnyVBOTexCoord);
            glBufferData(GL_ARRAY_BUFFER, texCoords.size()*2*sizeof(float), &texCoords[0][0], GL_STATIC_DRAW);
        }

        glBindVertexArray(m_bunnyVAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_bunnyVBOPosition);
        if(m_bunnyWireframe)  {
            glVertexAttribPointer(m_bunnyVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        } else {
            glVertexAttribPointer(m_bunnyVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        }

        glBindBuffer(GL_ARRAY_BUFFER, m_bunnyVBONormal);
        glVertexAttribPointer(m_bunnyVertexAttribNormal, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glBindBuffer(GL_ARRAY_BUFFER, m_bunnyVBOTexCoord);
        glVertexAttribPointer(m_bunnyVertexAttribTexCoord, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(m_bunnyVertexAttribPosition);
        glEnableVertexAttribArray(m_bunnyVertexAttribNormal);
        glEnableVertexAttribArray(m_bunnyVertexAttribTexCoord);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



    if(!m_objLoader->materials.empty() && m_objLoader->materials[0].m_textureFiles.count("map_Ka")) {
        std::string textureFileName = m_objLoader->materials[0].m_textureFiles["map_Ka"];
        auto data = handycpp::image::readPngAsRgba(textureFileName);
        FUN_INFO("map_Ka: %d, %d", data.width, data.height);
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &m_colorTexutre);
        glBindTexture(GL_TEXTURE_2D, m_colorTexutre);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data.width, data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data.rgba_image[0]);
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

    return true;

}

void ObjMeshRenderer::Finalize() {
    if(m_bunnyProgramObj) {
        glDeleteProgram(m_bunnyProgramObj);
        m_bunnyProgramObj = 0;
    }
    if(m_bunnyVertexShader) {
        glDeleteShader(m_bunnyVertexShader);
        m_bunnyVertexShader = 0;
    }
    if(m_bunnyFragmentShader) {
        glDeleteShader(m_bunnyFragmentShader);
        m_bunnyFragmentShader = 0;
    }
    if(m_bunnyVAO) {
        glDeleteVertexArrays(1, &m_bunnyVAO);
        m_bunnyVAO = 0;
    }
    if(m_bunnyVBOPosition) {
        glDeleteBuffers(1, &m_bunnyVBOPosition);
        m_bunnyVBOPosition = 0;
    }
    if(m_bunnyVBONormal) {
        glDeleteBuffers(1, &m_bunnyVBONormal);
        m_bunnyVBONormal = 0;
    }
    if(m_bunnyVBOTexCoord) {
        glDeleteBuffers(1, &m_bunnyVBOTexCoord);
        m_bunnyVBOTexCoord = 0;
    }
}

bool ObjMeshRenderer::Draw(const Transform &transform) {
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
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    transform.GetMVPMatrix(m_bunnyMVPMatrix);
    glLineWidth(1.0f);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);


    if(m_colorTexutre!=0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_colorTexutre);
    }
    glUseProgram(m_bunnyProgramObj);
    glUniformMatrix4fv(m_bunnyMVPUniformLoc, 1, GL_FALSE, &m_bunnyMVPMatrix[0][0]);
    glBindVertexArray(m_bunnyVAO);
    if(m_bunnyWireframe) {
        glDrawArrays(GL_LINES, 0, m_bunnyNumElements);
        //glDrawArrays(GL_LINES, 0, 8);

    } else {
        /*
        static int start = 0;
        if(start < m_bunnyNumElements) {
            start ++;
        }
         */
        glDrawArrays(GL_TRIANGLES, 0, m_bunnyNumElements);
        /*
        int i = start/100*6;
        for(;i<start/100*6+6; i++) {
            FUN_INFO("vertex %d: %f %f %f",i, vertices[i].x, vertices[i].y, vertices[i].z );
        }
         */
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
#endif
    return true;
}

