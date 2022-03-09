//
// Created by zhangfuwen on 2022/3/8.
//

#include <android/log.h>

#define LOG_TAG "ByteFlow"
#define FUN_PRINT(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, ##__VA_ARGS__)

#include "handycpp/logging.h"
#include "handycpp/dyntype.h"

using namespace handycpp::dyntype;

#include "UIRectRenderer.h"

const char VertexShaderSrc[] =
        "#version 300 es                            \n"
        "layout(location = 0) in vec3 a_Position;\n"
        "layout(location = 1) in vec4 a_Color;\n"
        "out vec4 v_PointColor; \n"
        "void main() {\n"
        "   v_PointColor = a_Color;\n"
        "	gl_Position= vec4(a_Position, 1.0f);\n"
        "}\n";

const char FragmentShaderSrc[] =
        "#version 300 es                            \n"
        "precision mediump float;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "in vec4 v_PointColor;\n"
        "void main(){\n"
        "   outColor = v_PointColor; \n"
        "}\n";

const char TexVertexShaderSrc[] =
        "#version 300 es                            \n"
        "layout(location = 0) in vec3 a_Position;\n"
        "layout(location = 1) in vec2 a_TexCoord;\n"
        "out vec2 v_TexCoord; \n"
        "void main() {\n"
        "   v_TexCoord = a_TexCoord;\n"
        "	gl_Position= vec4(a_Position, 1.0f);\n"
        "}\n";

const char TexFragmentShaderSrc[] =
        "#version 300 es                            \n"
        "precision mediump float;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "uniform sampler2D u_Sampler;\n"
        "out vec2 v_TexCoord; \n"
        "void main(){\n"
        "   outColor = texture(u_Sampler, v_TexCoord); \n"
        "}\n";

int UIRectRenderer::Init() {
    if (!m_program) {
        m_program = std::make_shared<RenderProgram>();
        int ret;
        if (m_texture) {
            ret = m_program->InitWithSource(TexVertexShaderSrc, TexFragmentShaderSrc);
        } else {
            ret = m_program->InitWithSource(VertexShaderSrc, FragmentShaderSrc);
        }

        if (ret < 0) {
            FUN_ERROR("failed to init program");
            return -1;
        }
    }

    return 0;
}

int UIRectRenderer::Finalize() {
    return 0;
}

int UIRectRenderer::Draw(const Transform &transform, const Camera &camera,
                         const std::vector<Light> &lights) {
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);


    m_program->use();
    glBindVertexArray(0);

    std::vector<std::array<glm::vec3, 6>> geo_rects;
    std::vector<std::array<glm::vec2, 6>> texCoords_rects;
    std::vector<std::array<glm::vec4, 6>> color_rects;
    for(const auto & rect : m_rects) {
        std::array<glm::vec3, 6> vertexPositions = {
                glm::vec3{rect.m_geo.start.x, rect.m_geo.start.y, rect.m_layer},
                glm::vec3{rect.m_geo.end.x, rect.m_geo.start.y, rect.m_layer},
                glm::vec3{rect.m_geo.end.x, rect.m_geo.end.y, rect.m_layer},

                glm::vec3{rect.m_geo.start.x, rect.m_geo.start.y, rect.m_layer},
                glm::vec3{rect.m_geo.end.x, rect.m_geo.end.y, rect.m_layer},
                glm::vec3{rect.m_geo.start.x, rect.m_geo.end.y, rect.m_layer},
        };
        geo_rects.push_back(vertexPositions);

        if (m_texture != nullptr) {
            std::array<glm::vec2, 6> values {
                    glm::vec2{rect.m_texCoord.start.x, rect.m_texCoord.start.y},
                    glm::vec2{rect.m_texCoord.end.x, rect.m_texCoord.start.y},
                    glm::vec2{rect.m_texCoord.end.x, rect.m_texCoord.end.y},
                    glm::vec2{rect.m_texCoord.start.x, rect.m_texCoord.start.y},
                    glm::vec2{rect.m_texCoord.end.x, rect.m_texCoord.end.y},
                    glm::vec2{rect.m_texCoord.start.x, rect.m_texCoord.end.y}
            };
            texCoords_rects.push_back(values);

        } else {
            std::array<glm::vec4, 6> values;
            if(!rect.hasColor) {
                values = {
                    m_rectColors.topLeft,
                            m_rectColors.topRight,
                            m_rectColors.bottomRight,
                            m_rectColors.topLeft,
                            m_rectColors.bottomRight,
                            m_rectColors.bottomLeft
                };
            } else {
                values = {
                        rect.m_colors.topLeft,
                        rect.m_colors.topRight,
                        rect.m_colors.bottomRight,
                        rect.m_colors.topLeft,
                        rect.m_colors.bottomRight,
                        rect.m_colors.bottomLeft

                };
            }
            color_rects.push_back(values);
        }
    }


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 0, &geo_rects[0][0][0]);
    if(!color_rects.empty()) {
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, 0, &color_rects[0][0][0]);
    }
    if(!texCoords_rects.empty()) {
        if(m_texture) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_texture->getId());
            m_program->setInt("u_Sampler", 0);
        }

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 0, &texCoords_rects[0][0][0]);
    }

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, m_rects.size() * 6);
//    glDrawArrays(GL_TRIANGLES, 0, 6);
    GO_CHECK_GL_ERROR("glDrawArrays");
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return 0;
}
