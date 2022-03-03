//
// Created by zhangfuwen on 2022/3/2.
//

#ifndef NDK_OPENGLES_3_0_OWNEDRESOURCES_H
#define NDK_OPENGLES_3_0_OWNEDRESOURCES_H


#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>

class OwnedResource {
public:
    virtual ~OwnedResource() {};
};

class OwnedTexture : public OwnedResource {
public:
    OwnedTexture(int width, int height, unsigned  char * data = nullptr);
    ~OwnedTexture();
    GLuint getId() {
        return m_ImageTextureId;
    }

private:
    GLuint m_ImageTextureId = 0;

};

class OwnedRenderbuffer : public OwnedResource {
public:
    OwnedRenderbuffer(int width, int height, GLenum internalFormat = GL_DEPTH24_STENCIL8);
    ~OwnedRenderbuffer();
    GLuint getId() {
        return m_renderBufferId;
    }
private:
    GLuint m_renderBufferId = 0;
};


#endif //NDK_OPENGLES_3_0_OWNEDRESOURCES_H
