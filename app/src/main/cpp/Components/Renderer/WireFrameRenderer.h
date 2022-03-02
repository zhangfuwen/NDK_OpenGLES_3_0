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
#include "glm.hpp"
#include <array>
#include "../Transform.h"
#include "../ObjLoader.h"
#include "IRenderer.h"

class WireFrameRenderer : public IRenderer {
private:
    GLuint m_ProgramObj;
    GLuint m_VertexShader;
    GLuint m_FragmentShader;
    GLint m_VertexAttribPosition;

    GLuint m_VBOPosition;

    GLuint m_VAO;
    glm::mat4 m_MVPMatrix{1.0f};
    GLuint m_MVPUniformLoc;
    int m_NumElements;

    void printBunnyVars();

    std::vector<std::array<glm::vec3, 2>> lines;

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
    int Draw(const Transform &transform) override;

    int LoadLines(ObjLoader * loader);
};

#endif //NDK_OPENGLES_3_0_WIREFRAMERENDERER_H
