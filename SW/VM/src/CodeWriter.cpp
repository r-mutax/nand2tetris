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
    } else if(command == "or"){
        pop_stack();
        m_ofs << "D=M\n";
        pop_stack();
        m_ofs << "D=D|M\n";
        push_stack();
    } else if(command == "not"){
        pop_stack();
        m_ofs << "D=!D\n";
        push_stack();
    }
}

void CodeWriter::writePushPop(Parser::COMMAND_TYPE command, std::string segment, std::string index){

    if(command == Parser::C_PUSH){

        if(segment == "argument"){
            genPushSegment("ARG", index);
        } else if(segment == "constant"){
            m_ofs << "@" << index << "\n";
            m_ofs << "D=A\n";
        } else if(segment == "local"){
            genPushSegment("LCL", index);
        } else if(segment == "this"){
            genPushSegment("THIS", index);
        } else if(segment == "that"){
            genPushSegment("THAT", index);
        }
        push_stack();
    } else if(command == Parser::C_POP){
        if(segment == "argument"){
            genPopSegment("ARG", index);
        } else if(segment == "local"){
            genPopSegment("LCL", index);
        } else if(segment == "this"){
            genPopSegment("THIS", index);
        } else if(segment == "that"){
            genPopSegment("THAT", index);
        }
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

    // initialize LCL
    m_ofs << "@300\n";
    m_ofs << "D=A\n";
    m_ofs << "@LCL\n";
    m_ofs << "M=D\n";

    // initialize ARG
    m_ofs << "@400\n";
    m_ofs << "D=A\n";
    m_ofs << "@ARG\n";
    m_ofs << "M=D\n";

    // initialize THIS
    m_ofs << "@3000\n";
    m_ofs << "D=A\n";
    m_ofs << "@THIS\n";
    m_ofs << "M=D\n";

    // initialize THAT
    m_ofs << "@3010\n";
    m_ofs << "D=A\n";
    m_ofs << "@THAT\n";
    m_ofs << "M=D\n";

}

int64_t CodeWriter::getlabel()
{
    return m_label++;
}

void CodeWriter::genPushSegment(std::string segment, std::string index)
{
    m_ofs << "@" << segment << "\n";
    m_ofs << "D=M\n";
    m_ofs << "@" << index << "\n";
    m_ofs << "A=D+A\n";
    m_ofs << "D=M\n";
}

void CodeWriter::genPopSegment(std::string segment, std::string index)
{
    // calcsegment address
    m_ofs << "@" << segment << "\n";
    m_ofs << "D=M\n";
    m_ofs << "@" << index << "\n";
    m_ofs << "D=D+A\n";
    m_ofs << "@R13\n";
    m_ofs << "M=D\n";

    // pop stack
    pop_stack();
    m_ofs << "D=M\n";

    // restore local segment address
    m_ofs << "@R13\n";
    m_ofs << "A=M\n";

    // pop to local segment
    m_ofs << "M=D\n";
}

