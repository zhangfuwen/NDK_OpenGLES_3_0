//
// Created by zhangfuwen on 2022/3/2.
//

#ifndef NDK_OPENGLES_3_0_CANVAS_H
#define NDK_OPENGLES_3_0_CANVAS_H

class Canvas {
public:

    virtual int Bind() { return 0; };

    virtual int Unbind() { return 0;};

    virtual ~Canvas() {};
};

#endif //NDK_OPENGLES_3_0_CANVAS_H
