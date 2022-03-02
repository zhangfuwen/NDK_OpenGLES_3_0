//
// Created by zhangfuwen on 2022/3/2.
//

#ifndef NDK_OPENGLES_3_0_GAMEOBJECT_H
#define NDK_OPENGLES_3_0_GAMEOBJECT_H
#include <vector>
#include <memory>
#include "Components/PBOCanvas.h"


class GameObject {
public:
    int AddChild(std::shared_ptr<GameObject> child);
    std::vector<std::shared_ptr<GameObject>> GetChildren();
private:
    std::vector<std::shared_ptr<GameObject>> children;
    std::shared_ptr<PBOCanvas> canvas;

};


#endif //NDK_OPENGLES_3_0_GAMEOBJECT_H
