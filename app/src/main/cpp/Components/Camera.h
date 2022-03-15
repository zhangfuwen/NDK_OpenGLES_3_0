//
// Created by zhangfuwen on 2022/3/7.
//

#ifndef NDK_OPENGLES_3_0_CAMERA_H
#define NDK_OPENGLES_3_0_CAMERA_H

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <Components/Transform.h>

class Camera {
public:
    glm::mat4 GetProjection() const { return Projection; }
    glm::mat4 GetView() const { return View; }
    glm::vec3 GetViewPos() const {
        return m_self_transform.translation;
    }

    void Move(float right, float forward, float  up) {
        m_self_transform.translation += Transform::right * right;
        m_self_transform.translation += Transform::front * forward;
        m_self_transform.translation += Transform::up * up;
        View = glm::lookAt(
                m_self_transform.translation,
                glm::vec3(m_self_transform.GetModel() * glm::vec4(Transform::front, 1.0f)),
                glm::vec3(m_self_transform.GetModel() * glm::vec4(Transform::up, 1.0f))  // Head is up (set to 0,-1,0 to look upside-down)
        );
    }

    void Rotate(float x, float y, float z) {
        m_self_transform.rotation += glm::vec3{x, y, z};
    }


    Camera(bool ortho = true, float fov = 90) {
        m_self_transform.rotation = {0.0f, 180.0f, 0.0f};
        m_self_transform.translation = {0.0f, 0.0f, 0.8f};
        if(ortho) {
            Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.7f);
        } else {
            Projection = glm::perspective(
                    glm::radians(fov), // The vertical Field of View, in radians: the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
                    4.0f / 3.0f,       // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
                    0.001f,              // Near clipping plane. Keep as big as possible, or you'll get precision issues.
                    100.0f             // Far clipping plane. Keep as little as possible.
            );
        }
//        View = glm::lookAt(
//                m_self_transform.translation,
//                glm::vec3(m_self_transform.GetModel() * glm::vec4(Transform::front, 1.0f)),
//                glm::vec3(m_self_transform.GetModel() * glm::vec4(Transform::up, 1.0f))  // Head is up (set to 0,-1,0 to look upside-down)
//        );
        View = glm::lookAt(
                glm::vec3 { 0.0f, 0.0f, 0.8f},
                {0.0f, 0.0f, 0.0f},
                Transform::up // Head is up (set to 0,-1,0 to look upside-down)
        );

    }

private:
    Transform m_self_transform;
    glm::vec3 m_viewTarget = glm::vec3(0, 0, 0);
    // Projection matrix
    glm::mat4 Projection;
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 1.0f, 100);
    //glm::mat4 Projection = glm::perspective(45.0f,ratio, 0.1f,100.f);

    // View matrix
    glm::mat4 View;
};


#endif //NDK_OPENGLES_3_0_CAMERA_H
