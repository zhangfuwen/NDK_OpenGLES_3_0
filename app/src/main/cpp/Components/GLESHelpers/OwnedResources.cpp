//
// Created by zhangfuwen on 2022/3/2.
//

#include "OwnedResources.h"

OwnedTexture::OwnedTexture(int width, int height, unsigned char * data, bool alloc) {
    glGenTextures(1, &m_ImageTextureId);
    glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if(alloc) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    }
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

}

OwnedTexture::~OwnedTexture() {
    if(m_ImageTextureId) {
        glDeleteTextures(1, &m_ImageTextureId);
        m_ImageTextureId = 0;
    }

}

OwnedRenderbuffer::OwnedRenderbuffer(int width, int height, GLenum internalFormat) {
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    m_renderBufferId = rbo;
}

OwnedRenderbuffer::~OwnedRenderbuffer() {
    glDeleteFramebuffers(1, &m_renderBufferId);
    m_renderBufferId = 0;
}
