#include "assembler.hpp"
#include <algorithm>
#include <cctype>

static inline void ltrim(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](char ch){
        return !std::isspace(ch);
    }));
}

Parser::Parser(std::string asm_file_path){
    m_ifs.open(asm_file_path);

    if(!m_ifs){
        throw std::invalid_argument(".asm file is not existed.");
        return;
    }

    // ファイルが開けたら1行読んでおく
    readCommand();
}

bool Parser::hasMoreCommands(){
    return !m_eof;
}

std::string Parser::getCommandsString(){
    return m_cur_command;
}

void Parser::advance(){
    readCommand();
}

void Parser::readCommand(){
    std::string s;

    while(1){
        try{
            readLine(s);
            removeComment(s);
            if(s == ""){
                continue;
            }
            break;
        } catch (std::out_of_range& e){
            throw e;
            return;
        }
    }

    m_cur_command = s;
}

void Parser::readLine(std::string& s){
    std::getline(m_ifs, s);    
    if(m_ifs.eof()){
        throw std::out_of_range("end of asm file.");
        return;
    }

    ltrim(s);
    return;
}

void Parser::removeComment(std::string& s)
{
    s = s.substr(0, s.find_first_of(std::string("//")));
}
