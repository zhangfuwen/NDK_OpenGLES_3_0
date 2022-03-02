//
// Created by zhangfuwen on 2022/3/2.
//

#include <handycpp/logging.h>
#include "PBOCanvas.h"

PBOCanvas::PBOCanvas(int width, int height) : m_width(width), m_height(height) {
    glGenFramebuffers(1, &m_FboId);
}

int PBOCanvas::InitFromTexture() {
    auto tex = std::make_shared<OwnedTexture>(m_width, m_height);
    if(tex == nullptr) {
        return -1;
    }

    resources.push_back(tex);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FboId);
    // TODO: not glBindTexture(GL_TEXTURE_2D, tex->getId());//
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_TEXTURE_2D, GL_COLOR_ATTACHMENT0, tex->getId(), 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER)!= GL_FRAMEBUFFER_COMPLETE) {
        FUN_ERROR("PBOSample::CreateFrameBufferObj glCheckFramebufferStatus status != GL_FRAMEBUFFER_COMPLETE");
        glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
        return -1;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);

    return 0;
}

int PBOCanvas::Bind() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FboId);
    return 0;
}

int PBOCanvas::Unbind() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GL_NONE);
    return 0;
}

PBOCanvas::~PBOCanvas() {
    if (m_FboId) {
        glDeleteFramebuffers(1, &m_FboId);
        m_FboId = 0;
    }
}
