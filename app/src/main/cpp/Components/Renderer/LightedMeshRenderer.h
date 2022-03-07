//
// Created by zhangfuwen on 2022/3/2.
//

#ifndef NDK_OPENGLES_3_0_LIGHTEDMESHRENDERER_H
#define NDK_OPENGLES_3_0_LIGHTEDMESHRENDERER_H


#include "Components/Transform.h"
#include "IRenderer.h"
#include "RenderProgram.h"
#include "Components/Camera.h"
#include "Material.h"

class LightedMeshRenderer : public IRenderer {
public:
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 tex_coord;
    };

    struct Triangle {
        Vertex v1, v2, v3;
    };


private:
    std::unique_ptr<RenderProgram> m_program;
    std::unique_ptr<Camera> m_camera;

    Material m_material;

    GLuint m_VertexAttribPosition;
    GLuint m_VertexAttribNormal;
    GLuint m_VertexAttribTexCoord;

    GLuint m_colorTexuture;

    GLuint m_VBOPosition;

    GLuint m_VAO;

    glm::mat4 m_MVPMatrix{1.0f};

    void printBunnyVars();


    std::vector<Triangle> triangles;
    int m_NumElements;

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
    int Draw(const Transform &transform, const Camera & camera, const std::vector<Light> &lights) override;


};


#endif
