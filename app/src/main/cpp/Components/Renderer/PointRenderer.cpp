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
#include "PointRenderer.h"


void PointRenderer::printBunnyVars() {
    FUN_INFO("program:%u", m_renderProgram->ID);
    FUN_INFO("PositionLoc:%d", m_VertexAttribPosition);
    FUN_INFO("ColorLoc:%d", m_attribLoc_Color);
    FUN_INFO("SizeLoc:%d", m_attribLoc_PointSize);
    FUN_INFO("VBO:%d, VAO:%d", m_VBOArrayOfStruct, m_VAO);
    FUN_INFO("numElements:%u", m_NumElements);
    FUN_INFO("mvpLoc:%u", m_MVPUniformLoc);
    auto s = glm::to_string(m_MVPMatrix);
    FUN_INFO("mvp: %s", s.c_str());
}


int PointRenderer::LoadPoints(PointLoader loader) {
    auto ret = loader(points, m_NumElements);
    if(ret <= 0) {
        return ret;
    }
    glGenBuffers(1, &m_VBOArrayOfStruct);
    glGenVertexArrays(1, &m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBOArrayOfStruct);
    glBufferData(GL_ARRAY_BUFFER, points.size() * 8 * sizeof(float), &points[0].pos[0], GL_STATIC_DRAW);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBOArrayOfStruct);
    glVertexAttribPointer(m_VertexAttribPosition, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *) 0);
    glVertexAttribPointer(m_attribLoc_Color, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *) 12);
    glVertexAttribPointer(m_attribLoc_PointSize, 1, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *) 28);
    glEnableVertexAttribArray(m_VertexAttribPosition);
    glEnableVertexAttribArray(m_attribLoc_Color);
    glEnableVertexAttribArray(m_attribLoc_PointSize);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return points.size();
}

int PointRenderer::Init() {
    const char VertexShaderSrc[] =
            "#version 300 es                            \n"
            "layout(location = 0) in vec3 a_Position;\n"
            "layout(location = 1) in vec4 a_Color;\n"
            "layout(location = 2) in float a_PointSize; \n"
            "out vec4 v_PointColor; \n"
            "void main() {\n"
            "   gl_PointSize = a_PointSize;\n"
            "   v_PointColor = a_Color;\n"
            "	gl_Position= vec4(a_Position, 1.0f);\n"
            "}\n";
    //"	gl_Position=u_MVPMatrix * vec4(a_Position, 1.0f);\n"
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
            "in vec4 v_PointColor;\n"
            "void main(){\n"
            "   outColor = v_PointColor; \n"
            "}\n";

    // 编译链接用于渲染兔子的着色器程序
    m_renderProgram = std::make_unique<RenderProgram>();
    auto ret = m_renderProgram->InitWithSource(VertexShaderSrc, FragmentShaderSrc);
    if(ret < 0) {
        FUN_ERROR("failed to initialize program");
        return -1;
    }

    m_VertexAttribPosition = glGetAttribLocation(m_renderProgram->ID, "a_Position");
    m_attribLoc_Color= glGetAttribLocation(m_renderProgram->ID, "a_Color");
    m_attribLoc_PointSize = glGetAttribLocation(m_renderProgram->ID, "a_PointSize");
    m_MVPUniformLoc = glGetUniformLocation(m_renderProgram->ID, "u_MVPMatrix");

    PointRenderer::printBunnyVars();
    return 0;

}

int PointRenderer::Finalize() {
    if (PointRenderer::m_VAO) {
        glDeleteVertexArrays(1, (const GLuint *)&m_VAO);
        PointRenderer::m_VAO = 0;
    }
    if (PointRenderer::m_VBOArrayOfStruct) {
        glDeleteBuffers(1, &m_VBOArrayOfStruct);
        PointRenderer::m_VBOArrayOfStruct = 0;
    }
    return 0;
}

int PointRenderer::Draw(const Transform &transform, const Camera & camera, const std::vector<Light> & lights) {
#if 0
    LoadPoints([&](std::vector<Point> &points, int & numElements) -> int {
        points.push_back(Point{glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 30.0f});
        points.push_back(Point{glm::vec3{1.0f, 1.0f, 0.0f}, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 30.0f});
        points.push_back(Point{glm::vec3{-1.0f, -1.0f, 0.0f}, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 30.0f});
        numElements = points.size();
        return points.size();
    });
    m_renderProgram->use();
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOArrayOfStruct);
//    glBufferSubData(GL_ARRAY_BUFFER, 0, )
    glDrawArrays(GL_POINTS, 0, m_NumElements);
    glBindVertexArray(0);
//    glEnable(GL_POLYGON_OFFSET_FILL);
//    glDisable(GL_DEPTH_TEST);
//    glDisable(GL_CULL_FACE);
#else
//	x += 1;
    glDisable(GL_POLYGON_OFFSET_FILL);
//    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);


    m_renderProgram->use();

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_POINTS, 0, m_NumElements);
    glBindVertexArray(0);
//    glEnable(GL_POLYGON_OFFSET_FILL);
//    glDisable(GL_DEPTH_TEST);
//    glDisable(GL_CULL_FACE);
#endif
    return 0;
}

