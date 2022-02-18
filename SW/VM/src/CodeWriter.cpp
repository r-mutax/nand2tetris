#include "VM.h"

CodeWriter::CodeWriter(void)
{
}

void CodeWriter::setAsmFileName(std::string asm_file_path)
{
    // asm file open
    if(m_ofs.is_open()){
        m_ofs.close();
    }
    m_ofs.open(asm_file_path);

    prologue();
}

void CodeWriter::setFileName(std::string filename)
{
    m_filename = filename;
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
        std::string label = getlabel();

        pop_stack();
        m_ofs << "D=M\n";
        pop_stack();
        m_ofs << "D=D-M\n";

        m_ofs << "@" << "TRUE." << label << "\n";
        m_ofs << "D;JEQ\n";

        // false
        m_ofs << "D=0\n";
        m_ofs << "@EQ_END." << label << "\n";
        m_ofs << "0;JMP\n";

        // true
        m_ofs << "(" << "TRUE." << label << ")\n";
        m_ofs << "D=-1\n";

        m_ofs << "(EQ_END." << label << ")\n";
        push_stack();
    } else if(command == "gt"){
        std::string label = getlabel();

        pop_stack();
        m_ofs << "D=M\n";
        pop_stack();
        m_ofs << "D=M-D\n";

        m_ofs << "@" << "TRUE." << label << "\n";
        m_ofs << "D;JGT\n";

        // false
        m_ofs << "D=0\n";
        m_ofs << "@GT_END." << label << "\n";
        m_ofs << "0;JMP\n";

        // true
        m_ofs << "(TRUE." << label << ")\n";
        m_ofs << "D=-1\n";

        m_ofs << "(GT_END." << label << ")\n";
        push_stack();
    } else if(command == "lt"){
        // if x < y then push true.
        std::string label = getlabel();

        pop_stack();
        m_ofs << "D=M\n";           // y
        pop_stack();
        m_ofs << "D=M-D\n";         // x - y

        m_ofs << "@TRUE." << label << "\n";
        m_ofs << "D;JLT\n";

        // false
        m_ofs << "D=0\n";
        m_ofs << "@LT_END." << label << "\n";
        m_ofs << "0;JMP\n";

        // true
        m_ofs << "(TRUE." << label << ")\n";
        m_ofs << "D=-1\n";

        m_ofs << "(LT_END." << label << ")\n";
        // return push
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
        } else if(segment == "temp"){
            int index_i = std::stol(index);
            genPushTemp(index_i + 5);
        } else if(segment == "pointer"){
            int index_i = std::stol(index);
            genPushPointer(index_i);
        } else if(segment == "static"){
            m_ofs << "@" << m_filename << "." << index << "\n";
            m_ofs << "D=M\n";
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
        } else if(segment == "temp"){
            int index_i = std::stol(index);
            genPopTemp(index_i + 5);
        } else if(segment == "pointer"){
            int index_i = std::stol(index);
            genPopPointer(index_i);
        } else if(segment == "static"){
            pop_stack();
            m_ofs << "D=M\n";
            m_ofs << "@" << m_filename << "." << index << "\n";
            m_ofs << "M=D\n";
        }
    }
}

void CodeWriter::writeLabel(std::string label)
{
    m_ofs << "(" << label << ")\n";
}

void CodeWriter::writeIf(std::string label)
{
    pop_stack();
    m_ofs << "D=M\n";

    m_ofs << "@" << label << "\n";
    m_ofs << "D;JNE\n";
}

void CodeWriter::writeGoto(std::string label)
{
    m_ofs << "@" << label << "\n";
    m_ofs << "0;JEQ\n";
}

void CodeWriter::writeFunction(std::string function, std::string argnum)
{
    m_label = 0;
    m_funcname = function;

    m_ofs << "(" << function << ")\n";
    for(int i = 0; i < stol(argnum); i++)
    {
        writePushPop(Parser::C_PUSH, "constant", "0");
    }
}

void CodeWriter::writeReturn()
{
    //m_ofs << "// return" << m_funcname << "\n";
    // get return address
    m_ofs << "@LCL\n";
    m_ofs << "D=M\n";
    m_ofs << "@R13\n";
    m_ofs << "M=D\n";
    m_ofs << "@5\n";
    // なんかへん？？？
    m_ofs << "D=D-A\n";
    m_ofs << "A=D\n";
    m_ofs << "D=M\n";
    m_ofs << "@R14\n";
    m_ofs << "M=D\n";

    // set retval
    pop_stack();
    m_ofs << "D=M\n";
    m_ofs << "@ARG\n";
    m_ofs << "A=M\n";
    m_ofs << "M=D\n";

    // set SP
    m_ofs << "@ARG\n";
    m_ofs << "D=M+1\n";
    m_ofs << "@SP\n";
    m_ofs << "M=D\n";

    // set THAT
    m_ofs << "@R13\n";
    m_ofs << "M=M-1\n";
    m_ofs << "A=M\n";
    m_ofs << "D=M\n";
    m_ofs << "@THAT\n";
    m_ofs << "M=D\n";

    // set THIS
    m_ofs << "@R13\n";
    m_ofs << "M=M-1\n";
    m_ofs << "A=M\n";
    m_ofs << "D=M\n";
    m_ofs << "@THIS\n";
    m_ofs << "M=D\n";

    // set arg
    m_ofs << "@R13\n";
    m_ofs << "M=M-1\n";
    m_ofs << "A=M\n";
    m_ofs << "D=M\n";
    m_ofs << "@ARG\n";
    m_ofs << "M=D\n";

    // set LCL
    m_ofs << "@R13\n";
    m_ofs << "M=M-1\n";
    m_ofs << "A=M\n";
    m_ofs << "D=M\n";
    m_ofs << "@LCL\n";
    m_ofs << "M=D\n";

    // return
    m_ofs << "@R14\n";
    m_ofs << "A=M\n";
    m_ofs << "0;JEQ\n";

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

    // set SP
    m_ofs << "@256\n";
    m_ofs << "D=A\n";
    m_ofs << "@SP\n";
    m_ofs << "M=D\n"; 

    // call Sys.init
    writeFuncCall("Sys.init", "0");
}

std::string CodeWriter::getlabel()
{
    return m_filename + std::string(".") + std::to_string(m_label++);
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

void CodeWriter::genPushTemp(int32_t address)
{
    m_ofs << "@" << address << "\n";
    m_ofs << "D=M\n";
}

void CodeWriter::genPopTemp(int32_t address)
{
    m_ofs << "@" << address << "\n";
    m_ofs << "D=A\n";
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


void CodeWriter::genPushPointer(int32_t index)
{
    // this
    if(index == 0) {
        m_ofs << "@THIS\n";
    } else if(index == 1){
        m_ofs << "@THAT\n";
    }
    m_ofs << "D=M\n";
}

void CodeWriter::genPopPointer(int32_t index)
{
    pop_stack();
    m_ofs << "D=M\n";

    if(index == 0){
        m_ofs << "@THIS\n";
    } else if(index == 1){
        m_ofs << "@THAT\n";
    }
    
    m_ofs << "M=D\n";
}

void CodeWriter::writeFuncCall(std::string func, std::string cnt)
{
    std::string return_label = "RETURN." + m_funcname + "." + getlabel();

    // setting for return
    m_ofs << "@" << return_label << "\n";
    m_ofs << "D=A\n";
    push_stack();

    m_ofs << "@LCL\n";
    m_ofs << "D=M\n";
    push_stack();

    m_ofs << "@ARG\n";
    m_ofs << "D=M\n";
    push_stack();

    m_ofs << "@THIS\n";
    m_ofs << "D=M\n";
    push_stack();

    m_ofs << "@THAT\n";
    m_ofs << "D=M\n";
    push_stack();

    // set ARG
    m_ofs << "@SP\n";
    m_ofs << "D=M\n";
    m_ofs << "@5\n";
    m_ofs << "D=D-A\n";
    m_ofs << "@" << cnt << "\n";
    m_ofs << "D=D-A\n";
    m_ofs << "@ARG\n";
    m_ofs << "M=D\n";

    // change LCL
    m_ofs << "@SP\n";
    m_ofs << "D=M\n";
    m_ofs << "@LCL\n";
    m_ofs << "M=D\n";

    writeGoto(func);
    m_ofs << "(" << return_label << ")\n";
}