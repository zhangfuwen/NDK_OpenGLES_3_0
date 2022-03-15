//
// Created by zhangfuwen on 2022/3/2.
//

#ifndef NDK_OPENGLES_3_0_IRENDERER_H
#define NDK_OPENGLES_3_0_IRENDERER_H


#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
//#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <array>
#include <glm/glm.hpp>
#include "Components/Transform.h"
#include "Components/ObjLoader.h"
#include "Components/Camera.h"
#include "Components/Light.h"

class IRenderer {

public:
    /**
     * @return 0 on success, negative numbers on error
     */
    virtual int Init() = 0;
    /**
     * @return 0 on success, negative numbers on error
     */
    virtual int Finalize() = 0;
    /**
     * @return 0 on success, negative numbers on error
     */
    virtual int Draw(const Transform &transform, const Camera & camera, const std::vector<Light> & lights) = 0;

    /**
     * @param ret , returned negative number from Init/Finalize/Draw
     */
    virtual std::string errorStr(int ret) { return "error"; };

//    virtual void Update() { Dump();};
    virtual void Update() { };
    virtual void Dump() {};

    virtual ~IRenderer() {};
};


#endif //NDK_OPENGLES_3_0_IRENDERER_H
