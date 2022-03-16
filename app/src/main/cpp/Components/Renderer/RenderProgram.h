//
// Created by zhangfuwen on 2022/3/7.
//

#ifndef NDK_OPENGLES_3_0_RENDERPROGRAM_H
#define NDK_OPENGLES_3_0_RENDERPROGRAM_H
#ifdef ANDROID
#include <android/log.h>

#define LOG_TAG "ByteFlow"
#define FUN_PRINT(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, ##__VA_ARGS__)
#endif
#include <glm/glm.hpp>
#include <handycpp/logging.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class RenderProgram
{
public:
    unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    int InitWithPath(const char* vertexPath, const char* fragmentPath) {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            // open files
            vShaderFile.open(m_vertexShaderSourcePath);
            fShaderFile.open(m_fragmentShaderSourcePath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
            // if geometry shader path is present, also load a geometry shader
        }
        catch (std::ifstream::failure &e) {
            FUN_ERROR("failed to read shader source %s", e.what());
            return -1;
        }
        return InitWithSource(vertexCode, fragmentCode);
    }

    int InitWithSource(std::string vertexCode, std::string fragmentCode) {
        const char* vShaderCode = vertexCode.c_str();
        FUN_INFO("compile vertex source:%s", vertexCode.c_str());
        const char * fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        GO_CHECK_GL_ERROR();
        if(checkCompileErrors(vertex, "VERTEX") < 0) {
            FUN_ERROR("failed to compile vertex shader");
            return -1;
        }
        // fragment RenderProgram
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        GO_CHECK_GL_ERROR();
        if(checkCompileErrors(fragment, "FRAGMENT") < 0) {
            FUN_ERROR("failed to compile fragment shader");
            return -1;
        }
        // if geometry shader is given, compile geometry shader
        unsigned int geometry;
        // shader RenderProgram
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        GO_CHECK_GL_ERROR();
        if(checkCompileErrors(ID, "PROGRAM") < 0) {
            FUN_ERROR("failed to link program");
            return -1;
        }
        GO_CHECK_GL_ERROR();
        // delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader(vertex);
        GO_CHECK_GL_ERROR();
        glDeleteShader(fragment);
        GO_CHECK_GL_ERROR();
        return 0;
    }
    ~RenderProgram() {
        glDeleteProgram(ID);
    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use()
    {
        glUseProgram(ID);
    }

    GLuint uniformLocation(const std::string & name) {
        return glGetUniformLocation(ID, name.c_str());
    }

    GLuint attribLocation(const std::string &name) {
        return glGetAttribLocation(ID, name.c_str());
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string &name, const glm::vec2 &value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string &name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, const glm::vec4 &value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string &name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    std::string m_vertexShaderSourcePath;
    std::string m_fragmentShaderSourcePath;
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    int checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if(type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                FUN_ERROR("failed to compile shader, %s, %s", type.c_str(), infoLog);
                return -1;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if(!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                FUN_ERROR("failed to link shader, %s, %s", type.c_str(), infoLog);
                return -1;
            }
        }
        return 0;
    }
};

#endif //NDK_OPENGLES_3_0_RENDERPROGRAM_H
