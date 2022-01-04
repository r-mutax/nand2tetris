#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <map>

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
    std::string     m_cur_dest;
    std::string     m_cur_comp;
    std::string     m_cur_jump;
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
    std::string dest();
    std::string comp();
    std::string jump();
    void reset();
};

class Code{
public:
    std::string dest(std::string code);
    std::string comp(std::string code);
    std::string jump(std::string code);
    std::string symbol(std::string code);
};

class HackFileIO{
    std::ofstream       m_ofs;
public:
    HackFileIO(std::string asm_file_path);
    HackFileIO& operator<<(std::string str){
        m_ofs << str;
        return *this;
    }
};

class SymbolTable{
    std::map<std::string, int64_t>      m_table;
public:
    SymbolTable();
    bool contains(std::string symbol);
    int64_t getAddress(std::string symbol);
    void addEntry(std::string symbol, int64_t address);
    void addVariable(std::string symbol);
};

#endif
