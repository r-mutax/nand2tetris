#include "VM.h"

CodeWriter::CodeWriter(std::string asm_file_path){

    m_ofs.open(asm_file_path);

    prologue();
}

void CodeWriter::writeArithmetic(std::string command){

    std::string asm_command;
    if(command == "add"){
        pop_stack();
        m_ofs << "D=M\n";
        pop_stack();
        m_ofs << "D=D+M\n";
        push_stack();
    } else if(command == "sub"){
        pop_stack();
        m_ofs << "D=M\n";
        pop_stack();
        m_ofs << "D=M-D\n";
        push_stack();
    } else if(command == "neg"){
        pop_stack();
        m_ofs << "D=-D\n";
        push_stack();
    } else if(command == "eq"){
        int64_t label = getlabel();

        pop_stack();
        m_ofs << "D=M\n";
        pop_stack();
        m_ofs << "D=D-M\n";

        m_ofs << "@" << "TRUE." << label << "\n";
        m_ofs << "D;JEQ\n";

        // false
        m_ofs << "D=0\n";
        push_stack();
        
        // true
        m_ofs << "(" << "TRUE." << label << ")\n";
        m_ofs << "D=-1\n";
        push_stack();
    } else if(command == "gt"){
        int64_t label = getlabel();

        pop_stack();
        m_ofs << "D=M\n";
        pop_stack();
        m_ofs << "D=M-D\n";

        m_ofs << "@" << "TRUE." << label << "\n";
        m_ofs << "D;JGT\n";

        // false
        m_ofs << "D=0\n";

        // true
        m_ofs << "(TRUE." << label << ")\n";
        m_ofs << "D=-1\n";
        push_stack();
    } else if(command == "lt"){
        int64_t label = getlabel();

        pop_stack();
        m_ofs << "D=M\n";
        pop_stack();
        m_ofs << "D=M-D\n";

        m_ofs << "@TRUE." << label << "\n";
        m_ofs << "D;JLT\n";

        // false
        m_ofs << "D=0\n";

        // true
        m_ofs << "(TRUE." << label << ")\n";
        m_ofs << "D=-1\n";
        push_stack();
    } else if(command == "and"){
        pop_stack();
        m_ofs << "D=M\n";
        pop_stack();
        m_ofs << "D=D&M\n";

        push_stack();
    }
}

void CodeWriter::writePushPop(Parser::COMMAND_TYPE command, std::string segment, std::string index){

    if(command == Parser::C_PUSH){

        if(segment == "argument"){

        } else if(segment == "constant"){
            m_ofs << "@" << index << "\n";
            m_ofs << "D=A\n";
        }
        push_stack();
    } else if(command == Parser::C_POP){

    }
}

// push from DRegister to Stack top.
void CodeWriter::push_stack(){
    m_ofs << "@SP\n";
    m_ofs << "A=M\n";
    m_ofs << "M=D\n";
    m_ofs << "@SP\n";
    m_ofs << "M=M+1\n";
}

// pop from Stack top.
void CodeWriter::pop_stack(){
    m_ofs << "@SP\n";
    m_ofs << "M=M-1\n";
    m_ofs << "A=M\n";
}

void CodeWriter::prologue(){

    // initialize SP
    m_ofs << "@256\n";
    m_ofs << "D=A\n";
    m_ofs << "@SP\n";
    m_ofs << "M=D\n";
}

int64_t CodeWriter::getlabel()
{
    return m_label++;
}
