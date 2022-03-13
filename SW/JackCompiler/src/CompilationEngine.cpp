#include "JackCompiler.h"

CompilationEngine::CompilationEngine(const std::string path)
{
    jt.Open(path);

    m_xml.openXMLFile(change_extension(path, "xml"));
}

CompilationEngine::~CompilationEngine()
{
}


// exec compile file.
void CompilationEngine::compile(void)
{
    compileClass();
}

// 'class' className '{' classVarDec* subroutineDec* '}'
void CompilationEngine::compileClass()
{
    m_xml.printDataHead("class");

    // check class
    if(jt.tokenType() == JackTokenizer::KEYWORD)
    {
        m_xml.printDataLine("keyword", jt.keyword());
        jt.advance();
    }

    // check className
    if(jt.tokenType() == JackTokenizer::IDENTIFIER)
    {
        m_xml.printDataLine("identifier", jt.identifier());
        jt.advance();
    }

    // check "{"
    if(jt.tokenType() == JackTokenizer::SYMBOL)
    {
        m_xml.printDataLine("symbol", jt.symbol());
        jt.advance();
    }

    // check classVarDec
    while (compileClassVarDec()){};

    // check subroutineDec
    while (compileSubroutine()) {};

    // check "}"
    if(jt.tokenType() == JackTokenizer::SYMBOL)
    {
        m_xml.printDataLine("symbol", jt.symbol());
        jt.advance();
    }

    m_xml.printDataTail("class");
}

// ( 'static' | 'field' ) type varName ( ',' varName )* ';'
bool CompilationEngine::compileClassVarDec()
{
    // is classVarDec?
    if(jt.tokenType() != JackTokenizer::KEYWORD
        || (jt.keyword() != "static" && jt.keyword() != "field")
    )
    {
        return false;
    }

    m_xml.printDataHead("classVarDec");

    // (static || field)
    m_xml.printDataLine("keyword", jt.keyword());
    jt.advance();

    // type
    if( !compileType() )
    {
        std::cerr << "This is not type." << std::endl;
        exit(1);
    }

    // varName
    if( !compileVarName() )
    {
        std::cerr << "This is not varName." << std::endl;
        exit(1);
    }

    // (',' varName) *
    do {
        if(jt.tokenType() != JackTokenizer::SYMBOL
            || jt.symbol() != ","
        ){
            break;
        }
        m_xml.printDataLine("symbol", jt.symbol());
        jt.advance();

        if ( !compileVarName() ){
            std::cerr << "This is not varName." << std::endl;
            exit(1);
        }

    } while (1);

    if(jt.tokenType() != JackTokenizer::SYMBOL
        || jt.symbol() != ";"
    )
    {
        std::cerr << "Expect \";\" ." << std::endl;
        exit(1);
    }
    m_xml.printDataLine("symbol", jt.symbol());
    jt.advance();

    m_xml.printDataTail("classVarDec");

    return true;
}

// 'int' | 'char' | 'boolean' | className
bool CompilationEngine::compileType()
{
    if(jt.tokenType() == JackTokenizer::KEYWORD)
    {
        if(jt.keyword() == "int"
            || jt.keyword() == "char"
            || jt.keyword() == "boolean"
        ){
            m_xml.printDataLine("keyword", jt.keyword());
            jt.advance();
            return true;
        }else{
            return false;
        }
    }

    if(jt.tokenType() == JackTokenizer::IDENTIFIER)
    {
        m_xml.printDataLine("identifier", jt.identifier());
        jt.advance();
        return true;
    }

    return false;
}

// identifier
bool CompilationEngine::compileVarName()
{
    if(jt.tokenType() == JackTokenizer::IDENTIFIER)
    {
        m_xml.printDataLine("identifier", jt.identifier());
        jt.advance();
        return true;
    }
    return false;
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