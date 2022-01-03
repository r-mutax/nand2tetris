#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

class Parser{
    std::ifstream   m_ifs;
    bool            m_eof;
    std::string     m_cur_command;
    void readCommand();
    void readLine(std::string& s);
    void removeComment(std::string& s);
public:
    Parser(std::string asm_file_path);
    bool hasMoreCommands();
    void advance();
    std::string getCommandsString();
};

#endif
