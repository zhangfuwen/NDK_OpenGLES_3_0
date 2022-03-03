//
// Created by zhangfuwen on 2022/3/2.
//

#include "GameObject.h"

int GameObject::AddChild(std::shared_ptr<GameObject> child) {
    m_children.push_back(child);
    return 0;
}

std::vector<std::shared_ptr<GameObject>> GameObject::GetChildren() {
    return std::vector<std::shared_ptr<GameObject>>();
}
