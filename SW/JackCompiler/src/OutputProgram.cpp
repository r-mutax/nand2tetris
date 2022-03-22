#include "JackCompiler.h"

void OutputProgram::SetFileName(std::string path)
{
    m_xml.openXMLFile(path);
}

void OutputProgram::printProgram(JC_Program* program)
{
    printClass(program->cls);
}

void OutputProgram::printClass(JC_Class* cls)
{
    m_xml.printDataHead("class");

    m_xml.printDataLine("keyword", "class");
    m_xml.printDataLine("identifier", cls->classname);
    m_xml.printDataLine("symbol", "{");

    JC_ClassVarDec* cur = cls->classVarDecs;
    while(cur){
        PrintClassVarDec((JC_ClassVarDec*)cur);
        cur = cur->next;
    }

    m_xml.printDataLine("symbol", "}");

    m_xml.printDataTail("class");
}

// ( 'static' | 'field' ) type varName ( ',' varName )* ';'
void OutputProgram::PrintClassVarDec(JC_ClassVarDec* clsvardec)
{
    m_xml.printDataHead("classVarDec");

    m_xml.printDataLine("keyword", clsvardec->vartype);

    JC_Type* type = ((JC_Type*)(clsvardec->type));
    m_xml.printDataLine(type->is_keyword ? "keyword" : "identifier", type->type);

    JC_VarName* varname = (JC_VarName*)(clsvardec->VarName);
    m_xml.printDataLine("identifier", varname->name);

    varname = (JC_VarName*)varname->next;
    while(varname){
        m_xml.printDataLine("symbol", ",");
        m_xml.printDataLine("identifier", varname->name);
        varname = (JC_VarName*)varname->next;
    }

    m_xml.printDataLine("symbol", ";");
    m_xml.printDataTail("classVarDec");
}