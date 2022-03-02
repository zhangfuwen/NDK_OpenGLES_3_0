//
// Created by zhangfuwen on 2022/3/2.
//

#ifndef NDK_OPENGLES_3_0_TEXTUREDMESHRENDERER_H
#define NDK_OPENGLES_3_0_TEXTUREDMESHRENDERER_H


#include "../Transform.h"
#include "IRenderer.h"

class TexturedMeshRenderer : public IRenderer {
private:
    GLuint m_ProgramObj;
    GLuint m_VertexShader;
    GLuint m_FragmentShader;
    GLuint m_VertexAttribPosition;
    GLuint m_VertexAttribNormal;
    GLuint m_VertexAttribTexCoord;

    GLuint m_colorSampler;
    GLuint m_colorTexutre;

    GLuint m_VBOPosition;

    GLuint m_VAO;
    glm::mat4 m_MVPMatrix{1.0f};
    GLuint m_MVPUniformLoc;
    int m_NumElements;

    void printBunnyVars();

    struct Vertex {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 tex_coord;
    };

    struct Triangle {
        Vertex v1, v2, v3;
    };

    std::vector<Triangle> triangles;

public:
    int LoadTexturedMesh(ObjLoader * loader);

    /**
     * @return 0 on success, negative numbers on error
     */
    int Init() override;
    /**
     * @return 0 on success, negative numbers on error
     */
    int Finalize() override;
    /**
     * @return 0 on success, negative numbers on error
     */
    int Draw(const Transform &transform) override;


};


#endif //NDK_OPENGLES_3_0_TEXTUREDMESHRENDERER_H
