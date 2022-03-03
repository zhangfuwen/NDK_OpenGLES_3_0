//
// Created by zhangfuwen on 2022/3/2.
//

#ifndef NDK_OPENGLES_3_0_CANVAS_H
#define NDK_OPENGLES_3_0_CANVAS_H

#include <string>

class Canvas {
public:

    virtual int Bind() = 0;

    virtual int Unbind() = 0;
    virtual int Clear() { return 0;};
    virtual uint32_t GetBackingStoreTexture() {return 0;};
    virtual int DownloadPixels(std::string filePath) { return 0;};

    virtual ~Canvas() {};
};

#endif //NDK_OPENGLES_3_0_CANVAS_H
