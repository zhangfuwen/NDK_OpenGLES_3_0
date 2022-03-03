//
// Created by zhangfuwen on 2022/3/2.
//
#define EGL_EGLEXT_PROTOTYPES 1
#define GL_GLEXT_PROTOTYPES 1
#define __ANDROID_API__ 29
#undef EGL_ANDROID_get_native_client_buffer
#define LOG_TAG "ByteFlow"
#define FUN_PRINT(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, ##__VA_ARGS__)
#include <android/log.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLUtils.h>

#include <android/hardware_buffer.h>

#include <handycpp/logging.h>
#include <handycpp/image.h>

#include "PBOCanvas.h"

PBOCanvas::PBOCanvas(int width, int height) : m_width(width), m_height(height) {
    glGenFramebuffers(1, &m_FboId);
}
GLuint PBOCanvas::GetColorAttachmentTextureId() {
    return m_FboTextureId;
}


class OwnedAhardwareBuffer : public OwnedResource {
public:
    OwnedAhardwareBuffer(int width, int height) {
        desc.width = width;
        desc.height = width;
        desc.usage = AHARDWAREBUFFER_USAGE_GPU_SAMPLED_IMAGE
                     | AHARDWAREBUFFER_USAGE_GPU_COLOR_OUTPUT
                     | AHARDWAREBUFFER_USAGE_GPU_FRAMEBUFFER
                     | AHARDWAREBUFFER_USAGE_CPU_READ_OFTEN
                     | AHARDWAREBUFFER_USAGE_CPU_WRITE_OFTEN;
        desc.layers = 1;
        desc.format = AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM;
        int ret = AHardwareBuffer_allocate(&desc, &hardwareBuffer);
        if(ret != 0) {
            FUN_ERROR("failed to allocate buffer %d", ret);
        }
        AHardwareBuffer_describe(hardwareBuffer, &desc);
    }

    std::unique_ptr<char[]> Read(int32_t fence = -1) {
        void *ptr;
        int ret = AHardwareBuffer_lock(hardwareBuffer, AHARDWAREBUFFER_USAGE_CPU_READ_OFTEN
                                            | AHARDWAREBUFFER_USAGE_CPU_WRITE_OFTEN, fence, nullptr, &ptr);
        if (ret != 0) {
            FUN_ERROR("failed, %d", ret);
            return nullptr;
        }
        if (desc.usage & AHARDWAREBUFFER_USAGE_PROTECTED_CONTENT) {
            FUN_ERROR("Surface is protected, unable to copy from it");
            return nullptr;
        }

        auto res = std::unique_ptr<char[]>((char*)malloc(desc.stride * desc.height * 4));
        memcpy(res.get(), ptr, desc.stride * desc.height * 4);
        FUN_INFO("width:%d, height:%d, stride %d", desc.width, desc.height, desc.stride);
        AHardwareBuffer_unlock(hardwareBuffer, nullptr);
        return res;
    }

    AHardwareBuffer_Desc GetDesc() {
        return desc;
    }
    AHardwareBuffer * get() {
        return hardwareBuffer;
    }

    ~OwnedAhardwareBuffer() override {
        if(hardwareBuffer) {
            AHardwareBuffer_release(hardwareBuffer);
        }

    }

private:
    AHardwareBuffer_Desc desc = {};
    AHardwareBuffer * hardwareBuffer = nullptr;

};

int PBOCanvas::Init(BackingStore backingStore) {
    m_backingStore = backingStore;
    switch (backingStore) {
        case BackingStore::TEXTURE:
            return InitFromTexture();
        case BackingStore::AHARDWARE_BUFFER:
            return InitFromAhardwareBuffer();
    }
}


int PBOCanvas::InitFromAhardwareBuffer() {
    // 创建并初始化 FBO 纹理
    auto tex = std::make_shared<OwnedTexture>(m_width, m_height, nullptr, false);
    if(tex == nullptr) {
        return -1;
    }
    m_FboTextureId = tex->getId();
    resources.push_back(tex);

    // 创建并初始化 FBO
    FUN_INFO("fbo %d", m_FboId);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FboId);
    glBindTexture(GL_TEXTURE_2D, m_FboTextureId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FboTextureId, 0);

    auto ahardwarebuffer = std::make_shared<OwnedAhardwareBuffer>(m_width, m_height);
    resources.push_back(ahardwarebuffer);

    EGLClientBuffer native_buffer = nullptr;
    native_buffer = eglGetNativeClientBufferANDROID(ahardwarebuffer->get());
    EGLint attrs[] = {EGL_IMAGE_PRESERVED_KHR, EGL_TRUE, EGL_NONE, EGL_NONE, EGL_NONE};
    auto image =
            eglCreateImageKHR(eglGetCurrentDisplay(), EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_ANDROID, native_buffer, attrs);

    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, image);
    void (*glEGLImageTargetTexStorageEXT1)(unsigned int, void *, const int *) = nullptr;
    glEGLImageTargetTexStorageEXT1 = (decltype(glEGLImageTargetTexStorageEXT1))eglGetProcAddress("glEGLImageTargetTexStorageEXT");
    if(glEGLImageTargetTexStorageEXT1 == nullptr) {
        FUN_ERROR("failed to find xxxxxx");
        exit(1);
    }
    glEGLImageTargetTexStorageEXT1(GL_TEXTURE_2D, image, nullptr);

    eglDestroyImageKHR(eglGetCurrentDisplay(), image);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER)!= GL_FRAMEBUFFER_COMPLETE) {
        FUN_ERROR("PBOSample::CreateFrameBufferObj glCheckFramebufferStatus status != GL_FRAMEBUFFER_COMPLETE");
        return false;
    }
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
    return 0;
}

int PBOCanvas::InitFromTexture() {
    auto tex = std::make_shared<OwnedTexture>(m_width, m_height);
    if(tex == nullptr) {
        return -1;
    }
    m_FboTextureId = tex->getId();
    resources.push_back(tex);

    auto renderbuffer = std::make_shared<OwnedRenderbuffer>(m_width, m_height);
    if(renderbuffer == nullptr) {
        return -2;
    }
    resources.push_back(renderbuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, m_FboId);
    glBindTexture(GL_TEXTURE_2D, tex->getId());//
    FUN_INFO("tex id %d", tex->getId());
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->getId(), 0);
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
    if(m_backingStore == TEXTURE) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FboId);
        int dataSize = m_height * m_width * 4;
        GLuint pack_buffer_id;
        glGenBuffers(1, &pack_buffer_id);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, pack_buffer_id);
        glBufferData(GL_PIXEL_PACK_BUFFER, dataSize, nullptr, GL_STREAM_READ);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glReadPixels(0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        auto *bufPtr = static_cast<GLubyte *>(glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0,
                                                               dataSize,
                                                               GL_MAP_READ_BIT));
        if (bufPtr) {
            handycpp::image::saveRgbaToPng(filePath, bufPtr, m_width, m_height);
            glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
        }
        glBindBuffer(GL_PIXEL_PACK_BUFFER, GL_NONE);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, GL_NONE);
    } else if(m_backingStore == AHARDWARE_BUFFER) {
        std::shared_ptr<OwnedAhardwareBuffer> buf = std::dynamic_pointer_cast<OwnedAhardwareBuffer>(resources[1]);
        glFinish();
        auto data = buf->Read();
        FUN_INFO("zhangfuwen stride:%d", buf->GetDesc().stride);
        handycpp::image::saveRgbaToPng(filePath, (unsigned char *)data.get(), buf->GetDesc().stride, m_height);
    }
    return 0;
}

PBOCanvas::~PBOCanvas() {
    if (m_FboId) {
        glDeleteFramebuffers(1, &m_FboId);
        m_FboId = 0;
    }
}
