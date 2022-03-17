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
#include "Components/GLESHelpers/OwnedResources.h"

class LightedMeshRenderer : public IRenderer {
public:
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 tex_coord;
    };

    struct VertexTB {
        glm::vec3 T;
        glm::vec3 B;
    };

    struct Triangle {
        Vertex v1, v2, v3;
    };

    struct TriangleTB {
        VertexTB tb1, tb2, tb3;
    };


private:

    bool m_normalMap = false;
    std::unique_ptr<RenderProgram> m_program;

    Material m_material;

    GLint m_VertexAttribPosition;
    GLint m_VertexAttribNormal;
    GLint m_VertexAttribTexCoord;

    GLuint m_colorTexuture;
    std::unique_ptr<OwnedTexture> m_bumpTexture = nullptr;

    GLuint m_VBOPosition;

    GLuint m_VAO;

    glm::mat4 m_MVPMatrix{1.0f};

    void printBunnyVars();


    std::vector<Triangle> triangles;
    std::vector<TriangleTB> trianglesTB;
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
    void Dump() override { printBunnyVars();}

    static TriangleTB calcTB(Triangle tri);
    LightedMeshRenderer(bool normalMap) : m_normalMap(normalMap) {}
};


#endif
