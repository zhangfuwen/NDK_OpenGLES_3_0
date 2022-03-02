//
// Created by zhangfuwen on 2022/3/2.
//

#ifndef NDK_OPENGLES_3_0_IRENDERER_H
#define NDK_OPENGLES_3_0_IRENDERER_H


#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <array>
#include "glm.hpp"
#include "../Transform.h"
#include "../ObjLoader.h"

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
    virtual int Draw(const Transform &transform) = 0;

    /**
     * @param ret , returned negative number from Init/Finalize/Draw
     */
    virtual std::string errorStr(int ret) { return "error"; };

    virtual ~IRenderer() {};
};


#endif //NDK_OPENGLES_3_0_IRENDERER_H
