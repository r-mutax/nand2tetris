#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

class Parser{
public:
    enum COMMAND_TYPE{
        A_COMMAND = 0
        , C_COMMAND
        , L_COMMAND
        , FORMAT_ERROR
    };

private:
    std::ifstream   m_ifs;
    bool            m_eof;
    std::string     m_cur_command;
    std::string     m_current_symbol;
    COMMAND_TYPE    m_command_type;

    void readCommand();
    void readLine(std::string& s);
    void removeComment(std::string& s);
    void analyzeCommand();
public:
    
    Parser(std::string asm_file_path);
    bool hasMoreCommands();
    void advance();
    COMMAND_TYPE commandType();
    std::string getCommandsString();
    std::string symbol();
};

#endif
