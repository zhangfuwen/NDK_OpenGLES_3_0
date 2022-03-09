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


    struct RectGeo {
        glm::vec2 start;
        glm::vec2 end;
    };
    struct RectColors {
        glm::vec4 topLeft;
        glm::vec4 topRight;
        glm::vec4 bottomRight;
        glm::vec4 bottomLeft;
        static RectColors MakeRectColors(glm::vec4 color) {
            RectColors colors;
            colors.topLeft = color;
            colors.topRight = color;
            colors.bottomLeft = color;
            colors.bottomRight = color;
            return colors;
        }
    };

    struct Rect {
        RectGeo m_geo;
        bool hasColor = true;
        RectColors m_colors = RectColors::MakeRectColors({0.5f, 0.5f, 0.5f, 1.0f});
        float m_layer = -1.0f; // NDC Coord [-1, +1]
        RectGeo m_texCoord = {glm::vec2{0, 0}, glm::vec2{1, 1}};
        static Rect MakeRectNoColor(RectGeo geo) {
            Rect rect;
            rect.m_geo = geo;
            rect.hasColor = false;
            return rect;
        }
        static Rect MakeRect(RectGeo geo, glm::vec4 color  = {0.5f, 0.5f, 0.5f, 1.0f}) {
            Rect rect;
            rect.m_geo = geo;
            rect.m_colors = RectColors::MakeRectColors(color);
            return rect;
        }

    };
    virtual int Init();

    virtual int Finalize();

    virtual int
    Draw(const Transform &transform, const Camera &camera, const std::vector <Light> &lights);

    void SetProgram(std::shared_ptr<RenderProgram> shared_program) { m_program = shared_program; }
    void SetTexture(std::shared_ptr<OwnedResource> tex) { m_texture = tex; }
    void AddRect(Rect rect) { m_rects.push_back(rect);}

private:
    float m_layer = -0.1f;
    std::shared_ptr<OwnedResource> m_texture = nullptr;
    RectColors m_rectColors { glm::vec4{1.0f, 0.0f, 0.0f, 1.0f},
                              glm::vec4{0.0f, 1.0f, 0.0f, 1.0f},
                              glm::vec4{0.0f, 0.0f, 1.0f, 1.0f},
                              glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}};
    std::vector<Rect> m_rects;


    std::shared_ptr<RenderProgram> m_program;
};


#endif //NDK_OPENGLES_3_0_UIRECTRENDERER_H
