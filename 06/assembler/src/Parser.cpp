#include "assembler.hpp"

Parser::Parser(std::string asm_file_path){
    m_ifs.open(asm_file_path);

    if(!m_ifs){
        throw std::invalid_argument(".asm file is not existed.");
        return;
    }

    // ファイルが開けたら1行読んでおく
    readLine();
}

bool Parser::hasMoreCommands(){
    return !m_eof;
}

std::string Parser::getCommandsString(){
    return m_cur_command;
}

void Parser::advance(){
    readLine();
}

void Parser::readLine(){
    std::getline(m_ifs, m_cur_command);
    if(m_ifs.eof()){
        m_eof = true;
    }
    return;
}

