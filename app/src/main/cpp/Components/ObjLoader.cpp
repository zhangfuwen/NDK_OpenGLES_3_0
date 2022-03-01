//
// Created by zhangfuwen on 2022/2/28.
//

#include <string>
#include <iostream>
#include "ObjLoader.h"
#include "handycpp/file.h"
#include "handycpp/string.h"
#include <android/log.h>
#include <libgen.h>
#define LOG_TAG "ByteFlow"
#define FUN_PRINT(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, ##__VA_ARGS__)
#include "handycpp/logging.h"
#include <libgen.h>

//inline std::istringstream& operator>> (std::istringstream&& strm, float& v) {
//    return strm;
//}
inline std::istream& operator>> (std::istream&& strm, glm::vec3& v) {
    strm >> v[0];
    strm >> v[1];
    strm >> v[2];
    return strm;
}

#include <sstream>

#define PARSE(var) \
    if(handycpp::string::starts_with(line, #var)) { \
        line = line.substr(std::string(#var).size()+1);                      \
        auto val = handycpp::string::trim(line);            \
        std::stringstream(val) >> material->m_##var;  \
    }



int ObjLoader::LoadMtlLib(std::string path) {
    FUN_INFO("loading from %s", path.c_str());
    Material * material = nullptr;
    int ret = handycpp::file::for_each_line(path, [&](int lineNo, std::string line) {
        if(handycpp::string::starts_with(line, "newmtl")) {
            if(material != nullptr) {
                materials.push_back(*material);
            }
            auto name = line.substr(7);
            name = handycpp::string::trim(name);
            material = new Material(name);
        } else if (line[0] == '\t'){
            line = line.substr(1);
            PARSE(Ns)  else
            PARSE(Ni) else
            PARSE(d) else
            PARSE(Tr) else
            PARSE(Tf) else
            PARSE(illum) else
            PARSE(Ka) else
            PARSE(Kd) else
            PARSE(Ks) else
            PARSE(Ke) else
            if(handycpp::string::starts_with(line, "map_") || handycpp::string::starts_with(line, "bump")) {
                auto spacePos = line.find(' ');
                material->m_textureFiles[line.substr(0, spacePos)] = std::string(dirname(path.c_str())) + "/" + line.substr(spacePos+1);
            }

        }

    });
    if(material!= nullptr) {
        materials.push_back(*material);
    }
    if(ret < 0) {
        FUN_ERROR("failed to read file %s", path.c_str());
    } else {
        FUN_INFO("loaded mtllib %d lines", ret);
    }
    return 0;
}

int ObjLoader::LoadObjFile(std::string filePath) {
    handycpp::file::for_each_line(filePath, [this](int i, std::string line) {
        if(line[0] == 'v') {
            if(line[1] == ' ') {
                float x,y,z;
                sscanf(line.substr(3).c_str(), "%f %f %f", &x, &y, &z);
                vertices.emplace_back(x, y, z);
            } else if (line[1] == 'n') {
                float x,y,z;
                sscanf(line.substr(3).c_str(), "%f %f %f", &x, &y, &z);
                normals.emplace_back(x, y, z);
            } else if(line[1] == 't') {
                float u,v, z;
                sscanf(line.substr(3).c_str(), "%f %f %f", &u, &v, &z);
                texCoords.emplace_back(u, v);
            }
        } else if(line[0] == 'f') {
            int v1,t1,n1,v2,t2,n2,v3,t3,n3, v4,t4,n4;
            sscanf(line.substr(2).c_str(), "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
                   &v1, &t1, &n1
                    ,&v2, &t2, &n2
                    ,&v3, &t3, &n3
                    ,&v4, &t4, &n4
            );
            faces.push_back(std::array<FaceVertexAttr, 4>{
                                       FaceVertexAttr{v1,t1,n1},
                                       FaceVertexAttr{v2,t2,n2},
                                       FaceVertexAttr{v3,t3,n3},
                                       FaceVertexAttr{v4,t4,n4}});
        } else if(handycpp::string::starts_with(line, "mtllib")) {
            mtllib = line.substr(7);
        }
    });

    if(!mtllib.empty()) {
        std::string path = dirname(filePath.c_str());
        int ret = LoadMtlLib(path + "/" + mtllib);
        if(ret < 0) {
            return ret;
        }
    }
    return 0;
}

void ObjLoader::Dump() {
    FUN_INFO("number of vertices:%d", vertices.size());
    FUN_INFO("number of texCoords:%d", texCoords.size());
    FUN_INFO("number of normals:%d", normals.size());
    FUN_INFO("number of faces:%d", faces.size());
    FUN_INFO("mtllib:%s", mtllib.c_str());
    for(const auto & material : materials) {
        FUN_INFO("material name:%s", material.m_name.c_str());
        FUN_INFO("\tNs:%f", material.m_Ns);
        FUN_INFO("\tNi:%f", material.m_Ni);
        FUN_INFO("\td:%f", material.m_d);
        FUN_INFO("\tTr:%f", material.m_Tr);
        FUN_INFO("\tTf:%f", material.m_Tf);

        FUN_INFO("\tillum:%d", material.m_illum);
        FUN_INFO("\tm_Ka:%f, %f, %f", material.m_Ka[0], material.m_Ka[1],material.m_Ka[2]);
        FUN_INFO("\tm_Kd:%f, %f, %f", material.m_Kd[0], material.m_Kd[1],material.m_Kd[2]);
        FUN_INFO("\tm_Ks:%f, %f, %f", material.m_Ks[0], material.m_Ks[1],material.m_Ks[2]);
        FUN_INFO("\tm_Ke:%f, %f, %f", material.m_Ke[0], material.m_Ke[1],material.m_Ke[2]);
        for(const auto & kv : material.m_textureFiles) {
            FUN_INFO("%s:%s", kv.first.c_str(), kv.second.c_str());
        }
    }

}