#include "VM.h"
#include <cctype>
#include <algorithm>

static inline void ltrim(std::string& s){
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](char ch){
        return !std::isspace(ch);
    }));
}

static inline void rtrim(std::string& s){
    s.erase(std::find_if(s.rbegin(),s.rend(), [](char ch){
        return !std::isspace(ch);
    }).base(), s.end());
}

// ---------------------------------------------------------
// public member
Parser::Parser(std::string vm_file_path)
{
    m_ifs.open(vm_file_path);

    if(!m_ifs){
        throw std::invalid_argument(".asm file is not existed.");
    }

    readCommand();
}

bool Parser::hasMoreCommands()
{
    return !m_eof;
}

void Parser::advance(){
    readCommand();
}

Parser::COMMAND_TYPE Parser::commandType(){
    return m_command_type;
}

// ---------------------------------------------------------
// private member
void Parser::readCommand(){
    std::string s;
    while(1){
        try{
            readLine(s);
            if(s == ""){
                continue;
            }
        } catch(std::out_of_range &e){
            throw e;
            return;
        }
    }

    m_cur_command = s;
}

void Parser::removeComment(std::string& s)
{
    s = s.substr(0, s.find_first_of(std::string("//")));
}

void Parser::readLine(std::string& s){
    std::getline(m_ifs, s);
    if(m_ifs.eof()){
        throw std::out_of_range("end of vm file.");
        return;
    }

    ltrim(s);
    removeComment(s);
    rtrim(s);
    
    // remove CR
    // when use gcc, std::getline() function remove only LF at end of the line.
    if(s[s.length()-1] == '\r'){
        s = s.substr(0, s.length() - 1);
    }
}