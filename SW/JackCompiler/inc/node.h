#ifndef JACK_COMPILER_NODE_H
#define JACK_COMPILER_NODE_H

#include <string>

// forward definition.
class JC_Variant;
class JC_SubroutineCall;
class JC_Expression;


enum JC_NodeType {
    JC_NO_TYPE = 0,
    JC_PROGRAM,
    JC_CLASS,
    JC_TYPE,
    JC_VARNAME,
    JC_SUBROUTINENAME
};

class JC_Element
{
    public:
        JC_NodeType type;
};

class JC_Type : public JC_Element
{
    public:
        JC_Type(){
            type = JC_TYPE;
        };
        bool            is_keyword;
        std::string     type;
};

class JC_VarName : public JC_Element
{
    public:
        JC_VarName(){
            type = JC_VARNAME;
            next = nullptr;
        };
        std::string     name;
        JC_VarName*     next;
};

class JC_SubroutineName : public JC_Element
{
    public:
        JC_SubroutineName(){
            type = JC_SUBROUTINENAME;
        };
        std::string name;
};

class JC_Parameter : JC_Element
{
    public:
        JC_Parameter(){
            type = nullptr;
            varname = nullptr;
            next = nullptr;
        }
        JC_Type*        type;
        JC_VarName*     varname;
        JC_Parameter*   next;
};

class JC_VarDec : public JC_Element
{
    public:
        JC_VarDec(){
            next = nullptr;
            type = nullptr;
            varname = nullptr;
        }   
        JC_VarDec* next;
        JC_Type* type;
        JC_VarName* varname;
};

enum StatementType {
    LET_STATEMENT = 0
    , IF_STATEMENT
    , WHILE_STATEMENT
    , DO_STATEMENT
    , RETURN_STATEMENT
    , UNKNOWN_STATEMENT
};

class JC_Operand : public JC_Element {
    public:
        JC_Operand(){
        };
        std::string op;
};

enum TermType {
    INTEGER_CONST = 0
    , STRING_CONST
    , KEYWORD_CONST
    , VARIANT
    , SUBROUTINECALL
    , EXPRESSION
    , UNARYOP_TERM
};

class JC_Term : public JC_Element
{
    public:
        JC_Term(){
            op = nullptr;
            next = nullptr;
        };
        JC_Operand* op;
        JC_Term*    next;
        JC_Variant*  var;
        JC_SubroutineCall* subcall;
        JC_Expression* exp;

        TermType    termtype;
        long        integerVal;
        std::string stringVal;
};

class JC_Expression : public JC_Element
{
    public:
        JC_Expression(){
            term = nullptr;
            next = nullptr;
        };
        JC_Term* term;
        JC_Expression* next;
};

class JC_Variant : public JC_Element
{
    public:
        JC_Variant()
        {
            varname = nullptr;
            exp = nullptr;
        }
        JC_VarName* varname;
        JC_Expression* exp;
};

class JC_SubroutineCall : public JC_Element
{
    public:
        JC_SubroutineCall(){
            subroutine_name = nullptr;
            exp = nullptr;
            classname = "";
        };

        JC_SubroutineName* subroutine_name;
        std::string classname;
        JC_Expression* exp;
};

class JC_Statement : public JC_Element
{
    public:
    JC_Statement(){
        type = UNKNOWN_STATEMENT;
        next = nullptr;
    };
    StatementType   type;
    JC_Statement* next;
};

class JC_MultipleStatement : public JC_Element
{
    public:
        JC_MultipleStatement(){
            statement_body = nullptr;
        };
        JC_Statement* statement_body;
};

class JC_LetStatement : public JC_Statement
{
    public:
        JC_LetStatement(){
            type = LET_STATEMENT;
            lhs = nullptr;
            rhs = nullptr;
            next = nullptr;
        };
        JC_Variant* lhs;
        JC_Expression* rhs;

};

class JC_DoStatement : public JC_Statement
{
    public:
        JC_DoStatement(){
            type = DO_STATEMENT;
            subcall = nullptr;
        }
        JC_SubroutineCall* subcall;
};

class JC_ReturnStatement : public JC_Statement
{
    public:
        JC_ReturnStatement(){
            type = RETURN_STATEMENT;
            exp = nullptr;
        }
        JC_Expression* exp;
};

class JC_IfStatement : public JC_Statement
{
    public:
        JC_IfStatement(){
            type = IF_STATEMENT;
            cond = nullptr;
            true_statements = nullptr;
            false_statements = nullptr;
        };
        JC_Expression* cond;

        JC_MultipleStatement*    true_statements;
        JC_MultipleStatement*    false_statements;
};

class JC_WhileStatement : public JC_Statement
{
    public:
        JC_WhileStatement(){
            type = WHILE_STATEMENT;
            cond = nullptr;
            while_body = nullptr;
        };
        JC_Expression* cond;
        
        JC_MultipleStatement* while_body;
};

class JC_SubroutineBody : public JC_Element
{
    public:
        JC_SubroutineBody(){
            vardec = nullptr;
        };
        JC_VarDec* vardec;
        JC_Statement* statements;
};

class JC_Subroutine : public JC_Element
{
    public:
        std::string         subroutinetype;
        JC_Type*            type;
        JC_SubroutineName*  name;
        JC_SubroutineBody*  body;
        JC_Parameter*       parameterlist;
        JC_Subroutine*      next;
};

class JC_ClassVarDec : public JC_Element
{
    public:
        JC_ClassVarDec()
        {
            next = nullptr;
            type = nullptr;
            VarName = nullptr;
        };
        std::string         vartype;
        JC_ClassVarDec*     next;
        JC_Type*            type;
        JC_VarName*         VarName;
};

class JC_Class : public JC_Element
{
    public:
        std::string classname;
        JC_ClassVarDec* classVarDecs;
        JC_Subroutine* classSubroutinDecs;
};

class JC_Program : public JC_Element
{
    public:
        JC_Program(){
            type = JC_PROGRAM;
        }
        JC_Class* cls;
};


#endif