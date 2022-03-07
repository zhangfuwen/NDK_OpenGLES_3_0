//
// Created by zhangfuwen on 2022/3/7.
//

#ifndef NDK_OPENGLES_3_0_CAMERA_H
#define NDK_OPENGLES_3_0_CAMERA_H

#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Camera {
public:
    glm::mat4 GetProjection() const { return Projection; }
    glm::mat4 GetView() const { return View; }
    glm::vec3 GetViewPos() const {
        return m_viewPos;
    }

    Camera() {
        Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.7f);
        View = glm::lookAt(
                m_viewPos,
                m_viewTarget,
                glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
        );

    }

private:
    glm::vec3 m_viewPos = glm::vec3(0.0f, 0.0f, 0.8f);
    glm::vec3 m_viewTarget = glm::vec3(0, 0, 0);
    // Projection matrix
    glm::mat4 Projection;
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 1.0f, 100);
    //glm::mat4 Projection = glm::perspective(45.0f,ratio, 0.1f,100.f);

    // View matrix
    glm::mat4 View;
};


#endif //NDK_OPENGLES_3_0_CAMERA_H
