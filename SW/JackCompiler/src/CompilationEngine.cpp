#include "JackCompiler.h"

CompilationEngine::CompilationEngine(const std::string path)
{
    jt.Open(path);
    output_program.SetFileName(change_extension(path, "xml"));
}

CompilationEngine::~CompilationEngine()
{
}


// exec compile file.
void CompilationEngine::compile(void)
{
    JC_Program* prog = new JC_Program();
    prog->cls = compileClass();
    output_program.printProgram(prog);
}

// 'class' className '{' classVarDec* subroutineDec* '}'
JC_Class* CompilationEngine::compileClass()
{
    if (jt.tokenType() != JackTokenizer::KEYWORD
        || jt.keyword() != "class") {
        return nullptr;
    }
    jt.advance();
    
    JC_Class*   cls = new JC_Class();

    // check className
    if(jt.tokenType() == JackTokenizer::IDENTIFIER){
        cls->classname = jt.identifier();
        jt.advance();
    }

    // check "{"
    if(jt.tokenType() == JackTokenizer::SYMBOL && jt.symbol() == "{"){
        jt.advance();
    }

    // check classVarDec
    JC_ClassVarDec  cvd_head;
    JC_ClassVarDec* cvd_cur = &cvd_head;
    while(cvd_cur){
        cvd_cur->next = compileClassVarDec();
        cvd_cur = cvd_cur->next;
    }
    cls->classVarDecs = cvd_head.next;

    // // check subroutineDec
    // while (compileSubroutine()) {};

    // check "}"
    if(jt.tokenType() == JackTokenizer::SYMBOL && jt.symbol() == "}"){
        jt.advance();
    }

    return cls;
}

// ( 'static' | 'field' ) type varName ( ',' varName )* ';'
JC_ClassVarDec* CompilationEngine::compileClassVarDec()
{
    // is classVarDec?
    if(jt.tokenType() != JackTokenizer::KEYWORD
        || (jt.keyword() != "static" && jt.keyword() != "field")
    )
    {
        return nullptr;
    }

    JC_ClassVarDec* cvd = new JC_ClassVarDec();

    // (static || field)
    cvd->vartype = jt.keyword();
    jt.advance();

    // type
    cvd->type = compileType();
    if( cvd->type == nullptr )
    {
        std::cerr << "This is not type." << std::endl;
        exit(1);
    }

    // varName
    cvd->VarName = compileVarName();
    if( cvd->VarName == nullptr )
    {
        std::cerr << "This is not varname." << std::endl;
        exit(1);
    }

    JC_VarName* cur_varname = cvd->VarName;
    // (',' varName) *
    do {
        if(jt.tokenType() != JackTokenizer::SYMBOL
            || jt.symbol() != ","
        ){
            break;
        }
        jt.advance();

        cur_varname->next = compileVarName();
        if(cur_varname->next == nullptr)
        {
            std::cerr << "This is not varName." << std::endl;
            exit(1);
        }
        cur_varname = cur_varname->next;
    
    } while (1);

    if(jt.tokenType() != JackTokenizer::SYMBOL
        || jt.symbol() != ";"
    )
    {
        std::cerr << "Expect \";\" ." << std::endl;
        exit(1);
    }
    jt.advance();

    return cvd;
}

// 'int' | 'char' | 'boolean' | className
JC_Type* CompilationEngine::compileType()
{
    if(jt.tokenType() == JackTokenizer::KEYWORD)
    {
        if(jt.keyword() == "int"
            || jt.keyword() == "char"
            || jt.keyword() == "boolean")
        {
            JC_Type* ty = new JC_Type();
            ty->is_keyword = true;
            ty->type = jt.keyword();
            jt.advance();
            return ty;
        }
        else
        {
            return nullptr;
        }
    }
    else if (jt.tokenType() == JackTokenizer::IDENTIFIER)
    {
        JC_Type* ty = new JC_Type();
        ty->is_keyword = false;
        ty->type = jt.identifier();
        jt.advance();
        return ty;
    }

    return nullptr;
}

// identifier
JC_VarName* CompilationEngine::compileVarName()
{
    if(jt.tokenType() == JackTokenizer::IDENTIFIER)
    {
        JC_VarName* varname = new JC_VarName();

        varname->name = jt.identifier();
        jt.advance();
        return varname;
    }
    return nullptr;
}

bool CompilationEngine::compileSubroutineName()
{
    if(jt.tokenType() == JackTokenizer::IDENTIFIER)
    {
        m_xml.printDataLine("identifier", jt.identifier());
        jt.advance();
        return true;
    }
    return false;
}

bool CompilationEngine::compileParameterList()
{
    m_xml.printDataHead("parameterList");

    if (jt.tokenType() != JackTokenizer::SYMBOL
        || jt.symbol() != ")")
    {
        while(1)
        {
            compileType();
            compileVarName();

            if (jt.tokenType() != JackTokenizer::SYMBOL
                || jt.symbol() != ",")
            {
                break;
            }
            else
            {
                m_xml.printDataLine("symbol", jt.symbol());
                jt.advance();
            }
        }
    }

    m_xml.printDataTail("parameterList");

    return true;
}

// ('constructor' | 'function' | 'method')
// ('void' | type) subroutineName '('
//  parameterList ')'
// subroutineBody
bool CompilationEngine::compileSubroutine()
{
    // is SubroutineDec?
    if (jt.tokenType() != JackTokenizer::KEYWORD
        || ( jt.keyword() != "constructor"
            && jt.keyword() != "function"
            &&  jt.keyword() != "method")
    ){
        return false;
    }

    m_xml.printDataHead("subroutineDec");
 
    // ('constructor' | 'function' | 'method')
    m_xml.printDataLine("keyword", jt.keyword());
    jt.advance();

    // ('void' | type)
    if(jt.tokenType() == JackTokenizer::KEYWORD && jt.keyword() == "void"){
        m_xml.printDataLine("keyword", jt.keyword());
        jt.advance();
    } else {
        compileType();
    }

    // subroutineName
    compileSubroutineName();

    // check "("
    if(jt.tokenType() == JackTokenizer::SYMBOL)
    {
        m_xml.printDataLine("symbol", jt.symbol());
        jt.advance();
    }

    // paramaterList
    compileParameterList();

    // check ")"
    if(jt.tokenType() == JackTokenizer::SYMBOL)
    {
        m_xml.printDataLine("symbol", jt.symbol());
        jt.advance();
    }

    compileSubroutineBody();

    m_xml.printDataTail("subroutineDec");

    return true;
}

// '{' varDec* statements '}'
bool CompilationEngine::compileSubroutineBody()
{
    m_xml.printDataHead("subroutineBody");

    // check "{"
    if(jt.tokenType() == JackTokenizer::SYMBOL)
    {
        m_xml.printDataLine("symbol", jt.symbol());
        jt.advance();
    }

    // check VarDec
    while (compileVarDec()){};




    // check "}"
    if(jt.tokenType() == JackTokenizer::SYMBOL)
    {
        m_xml.printDataLine("symbol", jt.symbol());
        jt.advance();
    }

    m_xml.printDataTail("subroutineBody");

    return true;
}

// 'var' type varName (',' varName) * ';'
bool CompilationEngine::compileVarDec()
{
    if (jt.tokenType() != JackTokenizer::KEYWORD
        || jt.keyword() != "var")
    {
        return false;
    }

    m_xml.printDataHead("varDec");

    m_xml.printDataLine("keyword", jt.keyword());
    jt.advance();

    compileType();
    while(1)
    {
        compileVarName();
        if(jt.tokenType() != JackTokenizer::SYMBOL
            || jt.symbol() != ",")
        {
            break;
        }
        else
        {
            m_xml.printDataLine("symbol", jt.symbol());
            jt.advance();
        }
    }

    // check ;
    m_xml.printDataLine("symbol", jt.symbol());
    jt.advance();

    m_xml.printDataTail("varDec");

    return true;
}
