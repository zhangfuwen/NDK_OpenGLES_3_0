//
// Created by zhangfuwen on 2022/3/2.
//

#ifndef NDK_OPENGLES_3_0_PBOCANVAS_H
#define NDK_OPENGLES_3_0_PBOCANVAS_H

#include <vector>
#include <memory>

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>

#include "Canvas.h"
#include "GLESHelpers/OwnedResources.h"

class PBOCanvas : public Canvas {
public:
    int InitFromTexture() ;
    GLuint GetColorAttachmentTextureId();
    int Bind() override;
    int Unbind() override;
    int DownloadPixels(std::string filePath) override;
    PBOCanvas(int width, int height);
    ~PBOCanvas() override;

private:
    int m_width;
    int m_height;
    GLuint m_FboId;

    std::vector<std::shared_ptr<OwnedResource>> resources;


};


#endif //NDK_OPENGLES_3_0_PBOCANVAS_H
