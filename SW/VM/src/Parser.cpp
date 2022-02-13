#include "VM.h"
#include <cctype>
#include <algorithm>
#include <sstream>

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

static inline std::vector<std::string> split(std::string str)
{
    std::vector<std::string> elems;
    std::string item = "";

    for(char c : str){
        if(isspace(c)){
            elems.emplace_back(item);
            item = "";
        } else {
            item = item + c;
        }
    }

    if(!item.empty()){
        elems.emplace_back(item);
    }

    return elems;
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

std::string Parser::arg1(){
    return m_arg1;
}

std::string Parser::arg2(){
    return m_arg2;
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
            break;
        } catch(std::out_of_range &e){
            throw e;
            return;
        }
    }

    m_cur_command = s;

    analyzeCommand();
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

void Parser::analyzeCommand(){
    // split command
    auto buf = split(m_cur_command);

    // command type
    m_command = buf[0];
    analyzeCommandType();

    if(buf.size() > 1){
        m_arg1 = buf[1];
    } else {
        m_arg1 = "";
    }

    if(m_command_type == C_ARITHEMIC){
        m_arg1 = buf[0];
    }

    if(buf.size() > 2){
        m_arg2 = buf[2];
    } else {
        m_arg2 = "";
    }
}

void Parser::analyzeCommandType(){
    if(
        (m_command == "add")
        || (m_command == "sub")
        || (m_command == "neg")
        || (m_command == "eq")
        || (m_command == "gt")
        || (m_command == "lt")
        || (m_command == "and")
        || (m_command == "or")
        || (m_command == "not")
    ){
        m_command_type = C_ARITHEMIC;
    } else if(m_command == "push"){
        m_command_type = C_PUSH;
    } else if(m_command == "pop"){
        m_command_type = C_POP;
    } else if(m_command == "label"){
        m_command_type = C_LABEL;
    } else if(m_command == "goto"){
        m_command_type = C_GOTO;
    } else if(m_command == "if-goto"){
        m_command_type = C_IF;
    } else if(m_command == "function"){
        m_command_type = C_FUNCTION;
    } else if(m_command == "return"){
        m_command_type = C_RETURN;
    }
}
