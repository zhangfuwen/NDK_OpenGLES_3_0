//
// Created by zhangfuwen on 2022/3/2.
//

#define LOG_TAG "ByteFlow"
#define FUN_PRINT(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, ##__VA_ARGS__)
#include <android/log.h>

#include <handycpp/logging.h>
#include <handycpp/image.h>

#include "PBOCanvas.h"

PBOCanvas::PBOCanvas(int width, int height) : m_width(width), m_height(height) {
    glGenFramebuffers(1, &m_FboId);
}
GLuint PBOCanvas::GetColorAttachmentTextureId() {
    return std::dynamic_pointer_cast<OwnedTexture>(resources[0])->getId();
}

int PBOCanvas::InitFromTexture() {
    auto tex = std::make_shared<OwnedTexture>(m_width, m_height);
    if(tex == nullptr) {
        return -1;
    }
    resources.push_back(tex);

    auto renderbuffer = std::make_shared<OwnedRenderbuffer>(m_width, m_height);
    if(renderbuffer == nullptr) {
        return -2;
    }
    resources.push_back(renderbuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, m_FboId);
    glBindTexture(GL_TEXTURE_2D, tex->getId());//
    FUN_INFO("tex id %d", tex->getId());
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_TEXTURE_2D, GL_COLOR_ATTACHMENT0, tex->getId(), 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer->getId());
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER)!= GL_FRAMEBUFFER_COMPLETE) {
        FUN_ERROR("glCheckFramebufferStatus status != GL_FRAMEBUFFER_COMPLETE");
        glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
        return -1;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);

    return 0;
}

int PBOCanvas::Bind() {
    glViewport(0, 0, m_width, m_height);
    FUN_INFO("binding to fbo %d, WIDTH", m_FboId);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FboId);
    return 0;
}

int PBOCanvas::Unbind() {
    FUN_INFO("unbinding from fbo %d", m_FboId);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GL_NONE);
    return 0;
}

int PBOCanvas::DownloadPixels(std::string filePath) {
    int dataSize = m_height * m_width * 4;
    GLuint pack_buffer_id;
    glGenBuffers(1, &pack_buffer_id);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pack_buffer_id);
    glBufferData(GL_PIXEL_PACK_BUFFER, dataSize, nullptr, GL_STREAM_READ);
    glReadPixels(0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    auto *bufPtr = static_cast<GLubyte *>(glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0,
                                                              dataSize,
                                                              GL_MAP_READ_BIT));
    if (bufPtr) {
        handycpp::image::writeBmp(filePath, bufPtr, m_width, m_height, 4);
        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    }
    return 0;
}

PBOCanvas::~PBOCanvas() {
    if (m_FboId) {
        glDeleteFramebuffers(1, &m_FboId);
        m_FboId = 0;
    }
}
