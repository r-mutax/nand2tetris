#pragma region

#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <queue>
#include <regex>
#include <map>

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

        bool expect_token(TK_TYPE type, std::string str);
        bool expect_token(TK_TYPE type);

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
        void printMultipleStatements(JC_MultipleStatement* multi_statement);
        void printStatements(JC_Statement* statements);
        void printLetStatement(JC_LetStatement* letstatements);
        void printDoStatement(JC_DoStatement* dostatemene);
        void printReturnStatement(JC_ReturnStatement* returnstatement);
        void printIfStatement(JC_IfStatement* ifstatement);
        void printWhileStatement(JC_WhileStatement* whilestatement);
        void printExpression(JC_Expression* expression);
        void printExpressionList(JC_Expression* exp_list);
        void printTerm(JC_Term* term);
        void printVariant(JC_Variant* var);
        void printSubroutineCall(JC_SubroutineCall* subcall);
    public:
        OutputProgram(){};
        void SetFileName(std::string path);
        void printProgram(JC_Program*   program);
};

class SymbolTable{
    public:
        enum SYMBOL_KIND{
            STATIC = 0,
            FIELD ,
            ARG,
            VAR
        };
    private:
        class symbol_entity{
            public:
                std::string type;
                SYMBOL_KIND kind;
                int32_t index;
        };
        using scope_symbol_table = std::map<std::string, symbol_entity>;
        scope_symbol_table    class_scope;
        scope_symbol_table    func_scope;
        scope_symbol_table*   cur_scope;
        symbol_entity   getSymbolEntity(std::string name);

        bool is_class_scope = true;
    public:
        SymbolTable(){};
        void init();
        void startSubroutine();
        void define(std::string name, std::string type, SYMBOL_KIND kind);
        int32_t varCount(SYMBOL_KIND kind);
        SYMBOL_KIND kindOf(std::string name);
        std::string typeOf(std::string name);
        int32_t indexOf(std::string name);
        bool is_defined(std::string name);
};

class VMWriter{
        enum VM_Segment{
            CONST = 0,
            ARG,
            LOCAL,
            STATIC,
            THIS,
            THAT,
            POINTER,
            TEMP
        };
        enum VM_Command{
            ADD = 0,
            SUB,
            NEG,
            EQ,
            GT,
            LT,
            AND,
            OR,
            NOT
        };

        std::ofstream   m_ofs;
        std::string classname;
        int32_t while_cnt = 0;
        int32_t if_cnt = 0;

        // naming rule.
        // write~~() : writeVMFile function.
        // gen~~() : it has node element as argument, call write~~() functions.
        void writePush(VM_Segment seg, int32_t idx);
        void writePop(VM_Segment seg, int32_t idx);
        void writeArithmetic(VM_Command cmd);
        void writeLabel(std::string label);
        void writeGoto(std::string label);
        void writeIf(std::string label);
        void writeCall(std::string name, int32_t nArgs);
        void writeFunction(std::string name, int32_t nLocals);
        void writeReturn();
        void writeOperand(std::string op);

        void genConstructor(JC_Subroutine* constructor);
        void genFunction(JC_Subroutine* function);
        void genMethod(JC_Subroutine* method);
        void genMultipleStatements(JC_Statement* multi_statements);
        void genLetStatement(JC_LetStatement* let_stmt);
        void genIfStatement(JC_IfStatement* if_stmt);
        void genWhileStatement(JC_WhileStatement* while_stmt);
        void genDoStatement(JC_DoStatement* do_stmt);
        void genReturnStatement(JC_ReturnStatement* ret_stmt);
        void genExpression(JC_Expression* exp);
        void genTerm(JC_Term* term);
        void genSubcall(JC_SubroutineCall* subcall);
        void genVariant(JC_Variant* var);

        int32_t countParameter(JC_Subroutine* function);
        std::string getFunctionName(JC_Subroutine* function);
    public:
        VMWriter();
        void init();
        void setFileName(std::string filename);
        void genVMFile(JC_Program* prog);
        void Close();
};

class CompilationEngine
{
    private:
        JackTokenizer       jt;
        GenXMLFile          m_xml;
        OutputProgram       output_program;
        SymbolTable         symtbl;
        VMWriter            vmwriter;

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
        JC_Statement* compileReturn();
        JC_Statement* compileIf();
        JC_Statement* compileWhile();

        JC_Expression* compileExpression();
        JC_Term* compileTerm();
        JC_Variant* compileVariant();
        JC_Operand* compileOp();
        JC_Expression* compileExpressionList();
        JC_MultipleStatement* compileMultiStatement();
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
