#include "assembler.hpp"
#include <algorithm>
#include <cctype>

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

Parser::Parser(std::string asm_file_path){
    m_ifs.open(asm_file_path);

    if(!m_ifs){
        throw std::invalid_argument(".asm file is not existed.");
        return;
    }

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

Parser::COMMAND_TYPE Parser::commandType(){
    return m_command_type;
}

std::string Parser::symbol(){
    return m_current_symbol;
}

std::string Parser::dest(){
    return m_cur_dest;
}

std::string Parser::comp(){
    return m_cur_comp;
}

std::string Parser::jump(){
    return m_cur_jump;
}

void Parser::reset(){
    m_ifs.clear();
    m_ifs.seekg(0, std::ios_base::beg);
    readCommand();
}

void Parser::readCommand(){
    std::string s;

    while(1){
        try{
            readLine(s);
            
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

    analyzeCommand();
}

void Parser::readLine(std::string& s){
    std::getline(m_ifs, s);    
    if(m_ifs.eof()){
        throw std::out_of_range("end of asm file.");
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
    
    return;
}

void Parser::removeComment(std::string& s)
{
    s = s.substr(0, s.find_first_of(std::string("//")));
}

void Parser::analyzeCommand()
{
    if(m_cur_command[0] == '@'){
        m_command_type = A_COMMAND;
        m_current_symbol = m_cur_command.substr(1, m_cur_command.size() - 1);
    } else if(m_cur_command[0] == '('){
        if(m_cur_command[m_cur_command.length() - 1] == ')'){
            m_command_type = L_COMMAND;
            m_current_symbol = m_cur_command.substr(1, m_cur_command.size() - 2);
        } else {
            m_command_type = FORMAT_ERROR;
        }
    } else {
        // read dest
        long eq_pos = m_cur_command.find('=');
        if(eq_pos != std::string::npos){
            m_cur_dest = m_cur_command.substr(0, eq_pos);
            eq_pos++;
        } else {
            eq_pos = 0;
            m_cur_dest = "";
        }

        // read jump
        long jp_pos = m_cur_command.find(';');
        if(jp_pos != std::string::npos){
            m_cur_jump = m_cur_command.substr(jp_pos + 1, m_cur_command.length() - jp_pos);
        } else {
            jp_pos = m_cur_command.length();
            m_cur_jump = "";
        }

        // read comp
        m_cur_comp = m_cur_command.substr(eq_pos, jp_pos - eq_pos);

        m_command_type = C_COMMAND;
    }
}
