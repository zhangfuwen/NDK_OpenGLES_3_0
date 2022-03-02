//
// Created by zhangfuwen on 2022/2/28.
//

#ifndef NDK_OPENGLES_3_0_OBJLOADER_H
#define NDK_OPENGLES_3_0_OBJLOADER_H

#include <string>
#include <vector>
#include <array>
#include <map>
#include "../glm/glm.hpp"

class ObjLoader {
public:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords = {};
    std::vector<glm::vec3> normals = {};
    struct FaceVertexAttr {
        int vertex_index;
        int texCord_index;
        int normal_index;
    };
    std::vector<std::array<FaceVertexAttr, 4>> faces = {};
    std::string mtllib;

    struct Material {
        Material(std::string name) : m_name(name) {}

        std::string m_name;
        float m_Ns;
        float m_Ni;
        float m_d;
        float m_Tr;
        float m_Tf;
        int m_illum;
        glm::vec3 m_Ka;
        glm::vec3 m_Kd;
        glm::vec3 m_Ks;
        glm::vec3 m_Ke;
        std::map<std::string, std::string> m_textureFiles;

    };

    std::vector<Material> materials;

public:
    void Dump();

    int LoadObjFile(
            std::string filePath = "/sdcard/Android/data/com.byteflow.app/files/Download/model/poly/Apricot_02_hi_poly.obj");

    int LoadMtlLib(std::string path);
};


#endif //NDK_OPENGLES_3_0_OBJLOADER_H
