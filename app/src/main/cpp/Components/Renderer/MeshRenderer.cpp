//
// Created by zhangfuwen on 2022/2/24.
//
#define  LOG_TAG "ByteFlow"
#include <android/log.h>

#define  FUN_PRINT(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#include "MeshRenderer.h"
#include "../Transform.h"
#include <handycpp/logging.h>
#include <GLUtils.h>
#include <happly.h>
#include <ext.hpp>

void MeshRenderer::printBunnyVars() {
    FUN_INFO("program:%u", m_bunnyProgramObj);
    FUN_INFO("vertexshader:%d", m_bunnyVertexShader);
    FUN_INFO("fragment:%d", m_bunnyFragmentShader);
    FUN_INFO("Position:%d", m_bunnyVertexAttribPosition);
    FUN_INFO("VBO:%d, EBO:%d, VAO:%d", m_bunnyVBO, m_bunnyEBO, m_bunnyVAO);
    FUN_INFO("numElements:%u", m_bunnyNumElements);
    FUN_INFO("mvpLoc:%u", m_bunnyMVPUniformLoc);
    auto s = glm::to_string(m_bunnyMVPMatrix);
    FUN_INFO("mvp: %s", s.c_str());
}

bool MeshRenderer::Init() {
    const char bunnyVertexShaderSrc[] =
            "#version 300 es                            \n"
            "layout(location = 0) in vec4 a_Position;\n"
            "uniform mat4 u_MVPMatrix;                  \n"
            "out highp float zDepth; \n"
            "void main() {\n"
            "	gl_Position=u_MVPMatrix * a_Position;\n"
            "	zDepth = gl_Position.z/gl_Position.w;\n"
            "}\n";

    const char bunnyFragmentShaderSrc[] =
            "#version 300 es                            \n"
            "precision mediump float;\n"
            "in highp float zDepth; \n"
            "layout(location = 0) out vec4 outColor;\n"
            "void main(){\n"
            "	outColor = vec4(0.5*(1.0 - zDepth),0.5, 0.5,1.0);\n"
            "}\n";

    // 编译链接用于渲染兔子的着色器程序
    {
        m_bunnyProgramObj = GLUtils::CreateProgram(bunnyVertexShaderSrc, bunnyFragmentShaderSrc, m_bunnyVertexShader, m_bunnyFragmentShader);
        m_bunnyVertexAttribPosition = glGetAttribLocation(m_bunnyProgramObj, "a_Position");
        m_bunnyMVPUniformLoc = glGetUniformLocation(m_bunnyProgramObj, "u_MVPMatrix");
//		m_bunnyVertexAttribPosition = 0;
        glGenBuffers(1, &m_bunnyVBO);
        glGenBuffers(1, &m_bunnyEBO);
        glGenVertexArrays(1, &m_bunnyVAO);

#if 1
        happly::PLYData plyIn("/sdcard/Android/data/com.byteflow.app/files/Download/model/poly/bun_zipper.ply");
        std::vector<std::array<double, 3>> vPos = plyIn.getVertexPositions();
        std::vector<std::vector<size_t>> fInd = plyIn.getFaceIndices<size_t>();
        FUN_INFO("fInd.size %d", fInd.size());

        // vertices
        std::vector<std::array<float, 3>> vPosFloat(vPos.size());
        for(int i = 0; i< vPos.size();i++) {
            vPosFloat[i][0] = vPos[i][0];
            vPosFloat[i][1] = vPos[i][1];
            vPosFloat[i][2] = vPos[i][2];
        }

        glBindBuffer(GL_ARRAY_BUFFER, m_bunnyVBO);
        glBufferData(GL_ARRAY_BUFFER, vPosFloat.size()*3*sizeof(float), &vPosFloat[0][0], GL_STATIC_DRAW);

        if(m_bunnyWireframe) {
            std::vector<std::array<int, 2>> edges(fInd.size()*3); // every triange has three edges
            for(int i = 0; i< fInd.size(); i++) {
                edges[i*3+0] = { (int)fInd[i][0], (int)fInd[i][1]};
                edges[i*3+1] = { (int)fInd[i][1], (int)fInd[i][2]};
                edges[i*3+2] = { (int)fInd[i][2], (int)fInd[i][0]};
            }
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bunnyEBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, edges.size()*2*sizeof(int), &edges[0][0] , GL_STATIC_DRAW);
            m_bunnyNumElements = edges.size();
        }else {
            std::vector<std::array<int, 3>> fIndInt(fInd.size());
            for(int i = 0; i< fInd.size();i++) {
                fIndInt[i][0] = fInd[i][0];
                fIndInt[i][1] = fInd[i][1];
                fIndInt[i][2] = fInd[i][2];
            }
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bunnyEBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, fIndInt.size()*3*sizeof(int), &fIndInt[0][0] , GL_STATIC_DRAW);
            m_bunnyNumElements = fIndInt.size();
        }

#else

        static float tableVerticesWithTriangles[] = {
				// Triangle1
				-0.5f,
				-0.5f,

				0.5f,
				0.5f,

				-0.5f,
				0.5f,
				// Triangle2
				-0.5f,
				-0.5f,

				0.5f,
				-0.5f,

				0.5f,
				0.5f,
		};
		static int elements[] = {
				1,2,3,
				4,5,6,
				7,8,9,
				10,11, 12
		};
		glBindBuffer(GL_ARRAY_BUFFER, m_bunnyVBO);
		glBufferData(GL_ARRAY_BUFFER, 12*sizeof(float), tableVerticesWithTriangles, GL_STATIC_DRAW);
		GO_CHECK_GL_ERROR();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bunnyEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(int), elements , GL_STATIC_DRAW);
		m_bunnyNumElements = 2;
#endif

        glBindVertexArray(m_bunnyVAO);

        glVertexAttribPointer(m_bunnyVertexAttribPosition, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(m_bunnyVertexAttribPosition);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    }
    return true;

}

void MeshRenderer::Finalize() {
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
    if(m_bunnyVBO) {
        glDeleteBuffers(1, &m_bunnyVBO);
        m_bunnyVBO = 0;
    }
    if(m_bunnyEBO) {
        glDeleteBuffers(1, &m_bunnyEBO);
        m_bunnyEBO = 0;
    }
}

bool MeshRenderer::Draw(const Transform &transform) {
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


    glUseProgram(m_bunnyProgramObj);
    glUniformMatrix4fv(m_bunnyMVPUniformLoc, 1, GL_FALSE, &m_bunnyMVPMatrix[0][0]);
    glBindVertexArray(m_bunnyVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bunnyEBO);
    if(m_bunnyWireframe) {
        glDrawElements(GL_LINES, m_bunnyNumElements, GL_UNSIGNED_INT, 0);

    } else {
        glDrawElements(GL_TRIANGLES, m_bunnyNumElements*3, GL_UNSIGNED_INT, 0);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
#endif
    return true;
}

bool MeshRenderer::Draw() {
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
    glLineWidth(1.0f);
    glDisable(GL_POLYGON_OFFSET_FILL);


    glUseProgram(m_bunnyProgramObj);
    glUniformMatrix4fv(m_bunnyMVPUniformLoc, 1, GL_FALSE, &m_bunnyMVPMatrix[0][0]);
    glBindVertexArray(m_bunnyVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bunnyEBO);
    if(m_bunnyWireframe) {
        glDrawElements(GL_LINES, m_bunnyNumElements, GL_UNSIGNED_INT, 0);

    } else {
        glDrawElements(GL_TRIANGLES, m_bunnyNumElements*3, GL_UNSIGNED_INT, 0);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glEnable(GL_POLYGON_OFFSET_FILL);
#endif
    return true;
}
