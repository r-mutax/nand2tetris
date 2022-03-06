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