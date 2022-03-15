//
// Created by zhangfuwen on 2022/3/7.
//

#ifndef NDK_OPENGLES_3_0_CAMERA_H
#define NDK_OPENGLES_3_0_CAMERA_H

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <Components/Transform.h>
#include <handycpp/dyntype.h>

class Camera {
public:
    glm::mat4 GetProjection() const { return Projection; }
    glm::mat4 GetView() const { return View; }
    glm::vec3 GetViewPos() const {
        return m_self_transform.translation;
    }

    void Move(float right, float forward, float  up) {
        auto & trans = m_self_transform.translation;
        trans += Transform::right * right;
        trans += Transform::front * forward;
        trans += Transform::up * up;
        View = glm::lookAt(
                trans,
                glm::vec3(m_self_transform.GetModel() * glm::vec4(Transform::front, 1.0f)),
                glm::vec3(m_self_transform.GetModel() * glm::vec4(Transform::up, 1.0f))  // Head is up (set to 0,-1,0 to look upside-down)
        );
        FUN_INFO("camera transform: %s", glm::to_string(m_self_transform.translation).c_str());

    }
    void TryMove(float right, float up, float  forward) {
        if(right > 1.0f) { right = 1.0f;};
        if(up > 1.0f) { up = 1.0f;};
        if(forward > 1.0f) { forward = 1.0f;};
        if(right < -1.0f) { right = -1.0f;};
        if(up < -1.0f) { up = -1.0f;};
        if(forward < -1.0f) { forward = -1.0f;};
//        FUN_INFO("%f %f %f", right, up, forward);
        auto  & trans = m_self_transform;
        auto model = m_self_transform.GetModel();
        auto front =  glm::vec4(Transform::front, 1.0f) * model;
        auto left = glm::vec4(Transform::right, 1.0f) * model;
        auto top = glm::vec4(Transform::up, 1.0f) * model;
        front = front / front.w;
        left = left / left.w;
        top = top / top.w;
//        FUN_INFO("front: %s", glm::to_string(front).c_str());
//        FUN_INFO("left: %s", glm::to_string(left).c_str());
//        FUN_INFO("model: %s", glm::to_string(model).c_str());
        trans.translation += glm::vec3(front* forward) + glm::vec3(left * right) + glm::vec3(top *up);
//        trans.translation += glm::vec3(m_self_transform.GetModel() *glm::vec4 (Transform::right * right, 1.0));
//        trans.translation += glm::vec3 (m_self_transform.GetModel() * glm::vec4 (Transform::front * forward, 1.0));
//        trans.translation += glm::vec3 (m_self_transform.GetModel() *glm::vec4 (Transform::up * up , 1.0));
//        View = glm::lookAt(
//                glm::vec3(0.0f, 0.2f, 0.0f),
//                glm::vec3(trans.GetModel() * glm::vec4(Transform::front, 1.0f)),
//                glm::vec3(trans.GetModel() * glm::vec4(Transform::up, 1.0f))  // Head is up (set to 0,-1,0 to look upside-down)
//        );
//        View = glm::lookAt(
//                glm::vec3(0.0f, 0.2f, 0.0f),d
//                glm::vec3(0.0f, 0.0f, 0.0f),
//                glm::vec3(0.0f, 1.0f, 0.0f)  // Head is up (set to 0,-1,0 to look upside-down)
//        );
        trans.translation.y = height;
        View = glm::lookAt(
                trans.translation,
                glm::vec3(front),
                glm::vec3(top) // Head is up (set to 0,-1,0 to look upside-down)
        );
//        FUN_INFO("camera transform: %s", glm::to_string(trans.translation).c_str());
        FUN_INFO("camera pos(%f, %f, %f), front:(%f, %f, %f), up:(%f, %f, %f)",
                 trans.translation.x, trans.translation.y, trans.translation.z,
                 front.x, front.y, front.z,
                 top.x, top.y, top.z
        );
    }

    void Rotate(float x, float y, float z) {
        auto &  trans = m_self_transform;
        trans.rotation += glm::vec3{x, y, z};
        auto model = trans.GetModel();
        auto front =  glm::vec4(Transform::front, 1.0f) * model;
        auto left = glm::vec4(Transform::right, 1.0f) * model;
        auto top = glm::vec4(Transform::up, 1.0f) * model;
        front = front / front.w;
        left = left / left.w;
        top = top / top.w;
        trans.translation.y = height;
        View = glm::lookAt(
                trans.translation,
                glm::vec3(front),
                glm::vec3(top)  // Head is up (set to 0,-1,0 to look upside-down)
        );
        FUN_INFO("camera pos(%f, %f, %f), front:(%f, %f, %f), up:(%f, %f, %f)",
                 trans.translation.x, trans.translation.y, trans.translation.z,
                 front.x, front.y, front.z,
                 top.x, top.y, top.z
                 );
    }


    Camera(bool ortho = false, float fov = 90) {
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
                glm::vec3 { 0.5f, height, 0.5f},
                {0.0f, 0.0f, 0.0f},
                Transform::up // Head is up (set to 0,-1,0 to look upside-down)
        );
        m_self_transform.translation = { 0.5f, height, 0.5f};


    }

private:
    float height = 1.7/WORLD_SIZE; //1.7 meters
    Transform m_self_transform;
    // Projection matrix
    glm::mat4 Projection;
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 1.0f, 100);
    //glm::mat4 Projection = glm::perspective(45.0f,ratio, 0.1f,100.f);

    // View matrix
    glm::mat4 View;
};


#endif //NDK_OPENGLES_3_0_CAMERA_H
