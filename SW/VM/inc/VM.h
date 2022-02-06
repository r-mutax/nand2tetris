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
    std::string                m_command;
    std::string                m_arg1;
    std::string                m_arg2;

    bool                       m_eof;
    Parser::COMMAND_TYPE       m_command_type;

    void readCommand();
    void readLine(std::string& s);
    void removeComment(std::string& s);
    void analyzeCommand();
    void analyzeCommandType();
public:
    Parser(std::string vm_file_path);
    bool hasMoreCommands();
    void advance();
    Parser::COMMAND_TYPE commandType();
    std::string arg1();
    std::string arg2();
};

class CodeWriter{
private:
    std::ofstream           m_ofs;
    std::string             m_filename;
    std::string             m_funcname;
    int64_t                 m_label;

    void push_stack();
    void pop_stack();
    void prologue();
    void set_this_pointer();
    int64_t getlabel();
    void genPushSegment(std::string segment, std::string index);
    void genPopSegment(std::string segment, std::string index);
    void genPushTemp(int32_t address);
    void genPopTemp(int32_t address);
    void genPushPointer(int32_t index);
    void genPopPointer(int32_t index);
public:
    CodeWriter(void);
    void writeArithmetic(std::string command);
    void writePushPop(Parser::COMMAND_TYPE command, std::string segment, std::string index);
    void setFileName(std::string filename);
};

#endif