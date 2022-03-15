//
// Created by zhangfuwen on 2022/3/2.
//

#ifndef NDK_OPENGLES_3_0_GAMEOBJECT_H
#define NDK_OPENGLES_3_0_GAMEOBJECT_H
#include <vector>
#include <memory>
#include <Components/Renderer/IRenderer.h>
#include "Components/PBOCanvas.h"


class GameObject {
public:
    int SetRenderer(IRenderer * renderer) {
        m_renderer = renderer;
        return 0;
    }
    Transform* transform() {
        return &m_self_transform;
    }
    int Update() {
        m_renderer->Update();
        for(auto child : m_children) {
            child->Update();
        }
        return 0;
    }
    int Draw(const Camera&camera, const std::vector<Light> & lights) {
        if(m_canvas) {
            m_canvas->Bind();
            m_canvas->Clear();
        }
        if(m_renderer) {
            m_renderer->Draw(m_RealtimeTransform, camera, lights);
        }
        for(auto child : m_children) {

            child->Draw(camera, lights);
        }
        if(m_canvas) {
            m_canvas->Unbind();
        }
        return 0;
    }
    int SetCanvas(std::shared_ptr<Canvas> canvas) {
        m_canvas = canvas;
        return 0;
    }
    GLuint GetCanvasTexture() {
        if(!m_canvas) {
            return 0;
        }
        return m_canvas->GetBackingStoreTexture();
    }
    int AddChild(std::shared_ptr<GameObject> child);
    std::vector<std::shared_ptr<GameObject>> GetChildren();

    int AddParentTransform(Transform transform) {
        m_RealtimeTransform = {
                m_self_transform.translation + transform.translation,
                m_self_transform.scale * transform.scale,
                m_self_transform.rotation + transform.rotation
        };
        for(auto child : m_children) {
            child->AddParentTransform(m_RealtimeTransform);
        }
        return 0;
    }

    virtual ~GameObject() {
        if(m_renderer) {
            m_renderer->Finalize();
            delete m_renderer;
            m_renderer = nullptr;
        }
    };

    void Move(float right, float forward, float  up) {
        m_self_transform.translation += Transform::right * right;
        m_self_transform.translation += Transform::front * forward;
        m_self_transform.translation += Transform::up * up;
    }

private:
    std::vector<std::shared_ptr<GameObject>> m_children;
    std::shared_ptr<Canvas> m_canvas = nullptr;
    IRenderer* m_renderer = nullptr;
    Transform m_self_transform = Transform{
            .translation = { 0.0f, 0.0f, 0.0f},
            .scale = { 1.0f, 1.0f, 1.0f},
            .rotation = { 0.0f, 0.0f, 0.0f},
    };
    Transform m_RealtimeTransform;

};


#endif //NDK_OPENGLES_3_0_GAMEOBJECT_H
