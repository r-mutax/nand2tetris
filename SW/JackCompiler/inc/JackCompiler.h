#pragma region

#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <queue>
#include <regex>

#include "node.h"


// utility library
std::string change_extension(std::string path,std::string ext);

class GenXMLFile 
{
    private:
        std::ofstream   m_ofs;
        int32_t lebel = 0;
    public:
        GenXMLFile(){};
        ~GenXMLFile();
        void openXMLFile(const std::string& path);
        void printDataHead(std::string node_name);
        void printDataTail(std::string node_name);
        void printDataLine(std::string node_name, std::string value);
};

class JackTokenizer
{
    public:
        enum TK_TYPE {
            KEYWORD = 0
            , SYMBOL
            , IDENTIFIER
            , INT_CONST
            , STRING_CONST
            , NO_TYPE 
        };
        void printDataHead(std::string node_name) { m_xml.printDataHead(node_name); };
        void printDataTail(std::string node_name) { m_xml.printDataTail(node_name); };
        void printDataLine(std::string node_name, std::string value) { m_xml.printDataLine(node_name, value); };

        JackTokenizer(const std::string path);
        JackTokenizer(void);
        void Open(const std::string path);
        bool hasMoreTokens(void);
        TK_TYPE tokenType(void);
        void advance(void);
        std::string keyword();
        std::string symbol();
        int32_t intVal();
        std::string stringVal();
        std::string identifier();

        TK_TYPE next_tokenType(void);
        std::string next_keyword();
        std::string next_symbol();
        int32_t next_intVal();
        std::string next_stringVal();
        std::string next_identifier();
    private:
        typedef struct _Token
        {
            JackTokenizer::TK_TYPE      type;   // token type
            std::string                 str;    // string (ex. identifer, symbol, string_const)
            int32_t                     data;   // integer value(ex. int_const)
            void reset(){
                type = JackTokenizer::TK_TYPE::NO_TYPE;
                str = "";
                data = 0;
            };

            _Token()
            {
                reset();
            };
        } Token;

        std::ifstream   m_ifs;
        GenXMLFile      m_xml;
        std::deque<Token>  tokens;
        Token           stash_token;
        int lebel = 0;
        bool isInComment = false;
        bool hasToken = true;
        void tokenizeLine(std::string buf);
        bool isKeyWord(std::string buf, int32_t pos, std::string& str);
        bool isSymbol(std::string buf, int32_t pos, std::string& str);
        void readTokens();
        bool isident1(char c);
        void stash_front_token();
        void stash_pop_front_token();
};

class OutputProgram
{
        GenXMLFile  m_xml;
        void printClass(JC_Class* cls);
        void printClassVarDec(JC_ClassVarDec* cvd);
        void printClassSubroutine(JC_Subroutine* csr);
        void printParameterList(JC_Parameter* param);
        void printSubroutineBody(JC_SubroutineBody* body);
        void printVarDec(JC_VarDec* vardec);
        void printStatements(JC_Statement* statements);
        void printLetStatement(JC_LetStatement* letstatements);
        void printDoStatement(JC_DoStatement* dostatemene);
        void printExpression(JC_Expression* expression);
        void printExpressionList(JC_Expression* exp_list);
        void printTerm(JC_Term* term);
    public:
        OutputProgram(){};
        void SetFileName(std::string path);
        void printProgram(JC_Program*   program);
};

class CompilationEngine
{
    private:
        JackTokenizer       jt;
        GenXMLFile          m_xml;
        OutputProgram       output_program;

        JC_Class* compileClass();
        JC_ClassVarDec* compileClassVarDec();
        JC_Type* compileType();
        JC_VarName* compileVarName();
        JC_Subroutine* compileSubroutine();
        JC_SubroutineName* compileSubroutineName();
        JC_Parameter* compileParameterList();
        JC_SubroutineBody* compileSubroutineBody();
        JC_SubroutineCall* compileSubroutineCall();
        JC_VarDec* compileVarDec();
        JC_Statement* compileStatements();
        JC_Statement* compileSingleStatement();
        JC_Statement* compileLet();
        JC_Statement* compileDo();
        JC_Expression* compileExpression();
        JC_Term* compileTerm();
        JC_Variant* compileVariant();
        JC_Operand* compileOp();
        JC_Expression* compileExpressionList();
    public:
        CompilationEngine(const std::string path);
        ~CompilationEngine(void);
        void compile(void);
};

class JackAnalyzer
{
    private:
        
    public:
        JackAnalyzer(const std::string path);
        void genTokenToXML(const std::string path);
        void runCompileJackFile(const std::string path);
};
