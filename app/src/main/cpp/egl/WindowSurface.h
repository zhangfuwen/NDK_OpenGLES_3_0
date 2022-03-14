//
// Created by Administrator on 2018/3/21.
//

#ifndef EGL_HELPER_WINDOWSURFACE_H
#define EGL_HELPER_WINDOWSURFACE_H

#ifdef ANDROID
#include <android/native_window.h>
#endif
#include "EglSurfaceBase.h"

class WindowSurface : public EglSurfaceBase {

public:
#ifdef ANDROID
    WindowSurface(EglCore *eglCore, ANativeWindow *window, bool releaseSurface);
    WindowSurface(EglCore *eglCore, ANativeWindow *window);
#endif
    // 释放资源
    void release();
    // 重新创建
    void recreate(EglCore *eglCore);

private:
#ifdef ANDROID
    ANativeWindow  *mSurface;
#endif
    bool mReleaseSurface;
};


#endif //EGL_HELPER_WINDOWSURFACE_H
