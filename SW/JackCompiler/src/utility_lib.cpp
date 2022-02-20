#include "JackCompiler.h"

std::string change_extension(std::string path,std::string ext)
{
    int64_t pos = path.rfind(".");
    if(pos == std::string::npos){
        return "";
    }

    return path.substr(0, pos + 1) + ext;
}
