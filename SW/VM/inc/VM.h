#ifndef VM_H
#define VM_H

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

class Parser {
public:
    enum COMMAND_TYPE{
        C_ARITHEMIC = 0
        , C_PUSH
        , C_POP
        , C_LABEL
        , C_GOTO
        , C_IF
        , C_FUNCTION
        , C_RETURN
        , C_CALL
    };
private:
    std::ifstream              m_ifs;
    std::string                m_cur_command;
    bool                       m_eof;
    Parser::COMMAND_TYPE       m_command_type;

    void readCommand();
    void readLine(std::string& s);
    void removeComment(std::string& s);
public:
    Parser(std::string vm_file_path);
    bool hasMoreCommands();
    void advance();
    Parser::COMMAND_TYPE commandType();
};

#endif