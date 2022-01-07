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
    std::ifstream       m_ifs;

public:
    Parser(std::string vm_file_path);
    bool hasMoreCommands();
};

#endif