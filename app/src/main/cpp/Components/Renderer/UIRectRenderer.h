//
// Created by zhangfuwen on 2022/3/8.
//

#ifndef NDK_OPENGLES_3_0_UIRECTRENDERER_H
#define NDK_OPENGLES_3_0_UIRECTRENDERER_H

#include <PBOSample.h>
#include "IRenderer.h"
#include "../Transform.h"
#include "../Camera.h"
#include "RenderProgram.h"


class UIRectRenderer : public IRenderer {
public:
    struct Rect {
        glm::vec2 start;
        glm::vec2 end;
    };
    struct RectColors {
        glm::vec4 topLeft;
        glm::vec4 topRight;
        glm::vec4 bottomRight;
        glm::vec4 bottomLeft;
    };
    virtual int Init();

    virtual int Finalize();

    virtual int
    Draw(const Transform &transform, const Camera &camera, const std::vector <Light> &lights);

    void SetProgram(std::shared_ptr<RenderProgram> shared_program) { m_program = shared_program; }
    void SetTexture(std::shared_ptr<OwnedResource> tex) { m_texture = tex; }
    void SetRect(Rect rect) { m_rect = rect;}
    void SetTexCoord(Rect texCoord) { m_texCoord = texCoord; }

private:
    float m_layer = -0.1f;
    std::shared_ptr<OwnedResource> m_texture = nullptr;
    Rect m_rect = { glm::vec2{-1.0f,-1.0f}, glm::vec2{1.0f, 1.0f}};
    RectColors m_rectColors { glm::vec4{1.0f, 0.0f, 0.0f, 1.0f},
                              glm::vec4{0.0f, 1.0f, 0.0f, 1.0f},
                              glm::vec4{0.0f, 0.0f, 1.0f, 1.0f},
                              glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}};
    Rect m_texCoord = {glm::vec2{0, 0}, glm::vec2{1, 1}};


    std::shared_ptr<RenderProgram> m_program;
};


#endif //NDK_OPENGLES_3_0_UIRECTRENDERER_H
