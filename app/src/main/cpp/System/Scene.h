//
// Created by zhangfuwen on 2022/3/15.
//

#ifndef LEARNRENDERER_SCENE_H
#define LEARNRENDERER_SCENE_H

#include <Entity/GameObject.h>
#include "memory"


class Scene {
public:
    void Init();
    void Draw();
    void Update();
    void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);
    void CameraMove(float x, float y, float z);
    void CameraRotate(float x, float y, float z);
private:
    std::shared_ptr<GameObject> m_gameObject;
    std::vector<Light> m_lights;
    std::vector<Camera> m_camera;

    std::shared_ptr<IRenderer> m_coordRenderer;

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;
};


#endif //LEARNRENDERER_SCENE_H
