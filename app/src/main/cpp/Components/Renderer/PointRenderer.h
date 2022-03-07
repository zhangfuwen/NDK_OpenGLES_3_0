//
// Created by zhangfuwen on 2022/3/2.
//

#ifndef NDK_OPENGLES_3_0_POINTRENDERER_H
#define NDK_OPENGLES_3_0_POINTRENDERER_H

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

using PointLoader = std::function<int(std::vector<glm::vec3> &, int &)>;

class PointRenderer : public IRenderer {
private:
    RenderProgram
    GLint m_VertexAttribPosition;

    GLuint m_VBOPosition;

    GLuint m_VAO;
    glm::mat4 m_MVPMatrix{1.0f};
    GLuint m_MVPUniformLoc;
    int m_NumElements;

    void printBunnyVars();

    std::vector<glm::vec3> points;

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

    int LoadPoints(PointLoader loader);
};

#endif //NDK_OPENGLES_3_0_WIREFRAMERENDERER_H
