#include "assembler.hpp"

HackFileIO::HackFileIO(std::string asm_file_path){
    
    size_t extension_pos = asm_file_path.find_last_of('.');
    std::string extension = asm_file_path.substr(extension_pos + 1, asm_file_path.length() - extension_pos - 1);

    if(extension != "asm"){
        throw std::invalid_argument("file is not assembly file.");
    }

    std::string hack_file_path = asm_file_path.substr(0, extension_pos + 1) + "hack";

    m_ofs.open(hack_file_path, std::ios_base::out);

    return;
}
