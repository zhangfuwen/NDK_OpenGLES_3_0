//
// Created by zhangfuwen on 2022/3/7.
//

#ifndef NDK_OPENGLES_3_0_MATERIAL_H
#define NDK_OPENGLES_3_0_MATERIAL_H


class Material {
public:
    glm::vec3 getKa()  const {
        return Ka;
    }
    glm::vec3 getKd()  const {
        return Kd;
    }
    glm::vec3 getKs()  const {
        return Ks;
    }
    glm::vec3 getKe()  const {
        return Ke;
    }
    float getShininess() const {
        return m_shininess;
    }

    void setKa(const glm::vec3 &ka) {
        Ka = ka;
    }

    void setKd(const glm::vec3 &kd) {
        Kd = kd;
    }

    void setKs(const glm::vec3 &ks) {
        Ks = ks;
    }

    void setKe(const glm::vec3 &ke) {
        Ke = ke;
    }

    void setMShininess(float mShininess) {
        m_shininess = mShininess;
    }

private:
    glm::vec3 Ka; // ambient ratio
    glm::vec3 Kd; // diffuse ratio
    glm::vec3 Ks; // specular ratio
    glm::vec3 Ke; // emission ratio
    float m_shininess;

};


#endif //NDK_OPENGLES_3_0_MATERIAL_H
