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
        printClassVarDec((JC_ClassVarDec*)cur);
        cur = cur->next;
    }

    JC_Subroutine* csr = cls->classSubroutinDecs;
    while(csr){
        printClassSubroutine(csr);
        csr = csr->next;
    }

    m_xml.printDataLine("symbol", "}");

    m_xml.printDataTail("class");
}

// ( 'static' | 'field' ) type varName ( ',' varName )* ';'
void OutputProgram::printClassVarDec(JC_ClassVarDec* clsvardec)
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

// ('constructor' | 'function' | 'method')
// ('void' | type) subroutineName '('
//  parameterList ')'
// subroutineBody
void OutputProgram::printClassSubroutine(JC_Subroutine* csr)
{
    m_xml.printDataHead("subroutineDec");

    // subroutine type
    m_xml.printDataLine("keyword", csr->subroutinetype);

    // retval type
    if (!csr->type){
        m_xml.printDataLine("keyword", "void");
    } else {
        if(csr->type->is_keyword){
            m_xml.printDataLine("keyword", csr->type->type);
        } else {
            m_xml.printDataLine("identifier", csr->type->type);
        }
    }

    // subroutine name
    m_xml.printDataLine("identifier", csr->name->name);

    // parameter list
    m_xml.printDataLine("symbol", "(");

    printParameterList(csr->parameterlist);

    m_xml.printDataLine("symbol", ")");


    m_xml.printDataTail("subroutineDec");
}

// ( (type varName) ( ',' type varName )* )?
void OutputProgram::printParameterList(JC_Parameter* param)
{
    m_xml.printDataHead("parameterList");
    while(param)
    {
        JC_Type* ty = param->type;
        JC_VarName* varname = param->varname;

        if(ty->is_keyword)
        {
            m_xml.printDataLine("keyword", ty->type);
        }
        else
        {
            m_xml.printDataLine("identifier", ty->type);
        }

        m_xml.printDataLine("identifier", varname->name);

        if(param->next){
            m_xml.printDataLine("symbol", ",");
        }
        param = param->next;
    }
    m_xml.printDataTail("parameterList");
}