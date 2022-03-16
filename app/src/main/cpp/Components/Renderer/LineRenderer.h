//
// Created by zhangfuwen on 2022/3/2.
//

#ifndef NDK_OPENGLES_3_0_WIREFRAMERENDERER_H
#define NDK_OPENGLES_3_0_WIREFRAMERENDERER_H

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include "glm/glm.hpp"
#include <array>
#include "Components//Transform.h"
#include "Components//ObjLoader.h"
#include "IRenderer.h"
#include "RenderProgram.h"

// std::vector<std::array<glm::vec3, 2>> &lines, int &numElements
using LinesType = std::vector<std::array<glm::vec3, 2>>;

inline int templateLineLoader(std::vector<std::array<glm::vec3, 2>> & lines) {
    // lines.push_back or emplace back
    return lines.size(); // return non-positive number of error
}

inline int templateLineLoader2(std::vector<std::array<glm::vec3, 2>> & lines, std::vector<std::array<glm::vec4, 2>> & colors) {
    // lines.push_back or emplace back
    // colors.push_back or emplace back
    return lines.size(); // return non-positive number of error
}

// return number of lines loaded
using LineLoader = std::function<decltype(templateLineLoader)>;
using LineLoader2 = std::function<decltype(templateLineLoader2)>;

class LineRenderer : public IRenderer {
private:
    std::unique_ptr<RenderProgram> m_program = nullptr;
    GLint m_VertexAttribPosition;
    GLint m_VertexAttribColor;

    GLuint m_VBOPosition;
    GLuint m_VBOColor;
    GLuint m_VAO;
    glm::mat4 m_MVPMatrix{1.0f};
    GLuint m_MVPUniformLoc{};
    GLuint m_UniformColor{};
    int m_NumElements{};

    void printBunnyVars();

    std::vector<std::array<glm::vec3, 2>> lines;
    std::vector<std::array<glm::vec4, 2>> colors;
    glm::vec4 m_sharedColor{1.0f, 1.0f, 1.0f, 1.0f};

public:
    /**
     * @return 0 on success, negative numbers on error
     */
    int Init() override;
    /**
     * @return 0 on success, negative numbers on error
     */
    int Finalize() override;
    /**
     * @return 0 on success, negative numbers on error
     */
    int Draw(const Transform &transform, const Camera & camera, const std::vector<Light> &lights) override;

    void Dump() override { printBunnyVars(); };

    int LoadLines(ObjLoader * loader);
    int LoadLines(LineLoader loader);
    int LoadLines(LineLoader2 loader2);

    void PrepareBuffers();
};

#endif //NDK_OPENGLES_3_0_WIREFRAMERENDERER_H
