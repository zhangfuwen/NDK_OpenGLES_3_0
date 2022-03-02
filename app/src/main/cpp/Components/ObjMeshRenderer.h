//
// Created by zhangfuwen on 2022/2/24.
//

#ifndef NDK_OPENGLES_3_0_OBJMESHRENDERER_H
#define NDK_OPENGLES_3_0_OBJMESHRENDERER_H

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include "../glm/glm.hpp"
#include "Transform.h"
#include "ObjLoader.h"

class ObjMeshRenderer {


private:
    ObjLoader *m_objLoader = nullptr;
    GLuint m_bunnyProgramObj;
    GLuint m_bunnyVertexShader;
    GLuint m_bunnyFragmentShader;
    GLuint m_bunnyVertexAttribPosition;
    GLuint m_bunnyVertexAttribNormal;
    GLuint m_bunnyVertexAttribTexCoord;

    GLuint m_colorSampler;
    GLuint m_colorTexutre;

    GLuint m_bunnyVBOPosition;
    GLuint m_bunnyVBONormal;
    GLuint m_bunnyVBOTexCoord;

    GLuint m_bunnyVAO;
    glm::mat4 m_bunnyMVPMatrix{1.0f};
    GLuint m_bunnyMVPUniformLoc;
    int m_bunnyNumElements;
    bool m_bunnyWireframe = false;

    void printBunnyVars();

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;

    std::vector<glm::vec3> lines;

public:
    bool Init();

    bool Draw(const Transform &transform);

    void Finalize();
};


#endif //NDK_OPENGLES_3_0_OBJMESHRENDERER_H
