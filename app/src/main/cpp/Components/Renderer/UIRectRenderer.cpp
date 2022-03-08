//
// Created by zhangfuwen on 2022/3/8.
//

#include <android/log.h>

#define LOG_TAG "ByteFlow"
#define FUN_PRINT(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, ##__VA_ARGS__)

#include "handycpp/logging.h"

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

    std::array<glm::vec3, 6> vertexPositions = {
            glm::vec3{m_rect.start.x, m_rect.start.y, m_layer},
            glm::vec3{m_rect.end.x, m_rect.start.y, m_layer},
            glm::vec3{m_rect.end.x, m_rect.end.y, m_layer},

            glm::vec3{m_rect.start.x, m_rect.start.y, m_layer},
            glm::vec3{m_rect.end.x, m_rect.end.y, m_layer},
            glm::vec3{m_rect.start.x, m_rect.end.y, m_layer},
    };
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 0, &vertexPositions[0][0]);


    if (m_texture != nullptr) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture->getId());
        m_program->setInt("u_Sampler", 0);
        std::array<glm::vec2, 6> values {
                 glm::vec2{m_texCoord.start.x, m_texCoord.start.y},
                 glm::vec2{m_texCoord.end.x, m_texCoord.start.y},
                 glm::vec2{m_texCoord.end.x, m_texCoord.end.y},
                 glm::vec2{m_texCoord.start.x, m_texCoord.start.y},
                 glm::vec2{m_texCoord.end.x, m_texCoord.end.y},
                glm::vec2{m_texCoord.start.x, m_texCoord.end.y}
        };
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 0, &values[0][0]);

    } else {
        std::array<glm::vec4, 6> values {
                m_rectColors.topLeft,
                m_rectColors.topRight,
                m_rectColors.bottomRight,
                m_rectColors.topLeft,
                m_rectColors.bottomRight,
                m_rectColors.bottomLeft
        };
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, 0, &values[0][0]);
    }

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    GO_CHECK_GL_ERROR("glDrawArrays");
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return 0;
}
