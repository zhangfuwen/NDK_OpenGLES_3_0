//
// Created by zhangfuwen on 2022/3/16.
//

#ifndef FUNRENDERER_SHADERSTRING_H
#define FUNRENDERER_SHADERSTRING_H
#include <string>
#include <unordered_map>
#include <sstream>

std::string mapToMacros(const std::unordered_map<std::string, std::string> & m) {
    std::stringstream ret;
    for(const auto & [k, v] : m) {
        ret << "#define " << k << " " << v  << ";\n";
    }
    return ret.str();
}
/**
 * version: 0x30, 0x31, 0x32(GLES 3.2)
*/
std::string MakeShaderString(int version, std::string macros, std::string source)
{
    std::string versionString;
    switch (version) {
        case 0x30:
            versionString = "#version 300 es\n";
            break;
        case 0x31:
            versionString = "#version 310 es\n";
            break;
        case 0x32:
            versionString = "#version 320 es\n";
            break;
        default:
            return "";
    }
    return versionString + macros + source;
}

#endif //FUNRENDERER_SHADERSTRING_H
