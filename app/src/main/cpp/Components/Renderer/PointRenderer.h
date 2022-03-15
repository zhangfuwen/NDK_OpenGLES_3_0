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
#include "Components/Renderer/RenderProgram.h"
#include "IRenderer.h"

__attribute__ ((packed)) struct Point {
    glm::vec3 pos;
    glm::vec4 color;
    float size;
};

static_assert(sizeof(Point)==32);
using PointLoader = std::function<int(std::vector<Point> &, int &)>;

class PointRenderer : public IRenderer {
private:
    std::unique_ptr<RenderProgram> m_renderProgram;
    GLint m_VertexAttribPosition;
    GLint m_attribLoc_Color;
    GLint m_attribLoc_PointSize;

    GLuint m_VBOArrayOfStruct;

    GLuint m_VAO;
    glm::mat4 m_MVPMatrix{1.0f};
    GLuint m_MVPUniformLoc;
    int m_NumElements;

    void printBunnyVars();

    std::vector<Point> points;

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
