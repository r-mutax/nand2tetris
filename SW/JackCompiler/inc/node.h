#ifndef JACK_COMPILER_NODE_H
#define JACK_COMPILER_NODE_H

#include <string>

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
        ~JC_VarName(){
            if(next) delete next;
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
        };
        ~JC_Parameter(){
            if(type) delete type;
            if(varname) delete varname;
            if(next) delete next;
        };
        JC_Type*        type;
        JC_VarName*     varname;
        JC_Parameter*   next;

        int             index;
        int             kind;
};

class JC_VarDec : public JC_Element
{
    public:
        JC_VarDec(){
            next = nullptr;
            type = nullptr;
            varname = nullptr;
        };
        ~JC_VarDec(){
            if(next) delete next;
            if(type) delete type;
            if(varname) delete varname;
        };

        void combine(JC_VarDec* rhs){
            JC_VarDec* cur = this;
            while(cur->next){
                cur = cur->next;
            }
            cur->next = rhs;
        };

        JC_VarDec* next;
        JC_Type* type;
        JC_VarName* varname;
        int                 index;
        int                 kind;
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

class JC_VariantBase : public JC_Element
{
};

class JC_TermBase : public JC_Element
{
    public:
};

class JC_Expression : public JC_Element
{
    public:
        JC_Expression(){
            term = nullptr;
            next = nullptr;
        };
        ~JC_Expression(){
            if(term) delete term;
            if(next) delete next;
        }
        JC_TermBase* term;
        JC_Expression* next;
};

class JC_SubroutineCall : public JC_Element
{
    public:
        JC_SubroutineCall(){
            subroutine_name = nullptr;
            exp = nullptr;
            classname = "";
            is_method = false;
        };
        ~JC_SubroutineCall(){
            if(subroutine_name) delete subroutine_name;
            if(exp) delete exp;
        };

        JC_SubroutineName* subroutine_name;
        std::string classname;
        JC_Expression* exp;
        bool is_method;
};

class JC_Term : public JC_TermBase
{
    public:
        JC_Term(){
            op = nullptr;
            next = nullptr;
            unary_op = nullptr;
        };
        ~JC_Term(){
            if(unary_op) delete unary_op;
            if(op) delete op;
            if(next) delete next;
            if(var) delete var;
            if(subcall) delete subcall;
            if(exp) delete exp;
        };
        JC_Operand* unary_op;
        JC_Operand* op;
        JC_TermBase*    next;
        JC_VariantBase*  var;
        JC_SubroutineCall* subcall;
        JC_Expression* exp;

        TermType    termtype;
        long        integerVal;
        std::string stringVal;
};


class JC_Variant : public JC_VariantBase
{
    public:
        JC_Variant()
        {
            varname = nullptr;
            exp = nullptr;
        };
        ~JC_Variant(){
            if(varname) delete varname;
            if(exp) delete exp;
        };
        JC_VarName* varname;
        JC_Expression* exp;
};


class JC_Statement : public JC_Element
{
    public:
    JC_Statement(){
        type = UNKNOWN_STATEMENT;
        next = nullptr;
    };
    ~JC_Statement(){
        if(next) delete next;
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
        ~JC_MultipleStatement(){
            if(statement_body) delete statement_body;
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
        ~JC_LetStatement(){
            if(lhs) delete lhs;
            if(rhs) delete rhs;
            if(next) delete next;
        }
        JC_Variant* lhs;
        JC_Expression* rhs;

};

class JC_DoStatement : public JC_Statement
{
    public:
        JC_DoStatement(){
            type = DO_STATEMENT;
            subcall = nullptr;
        };
        ~JC_DoStatement(){
            if(subcall) delete subcall;
        };
        JC_SubroutineCall* subcall;
};

class JC_ReturnStatement : public JC_Statement
{
    public:
        JC_ReturnStatement(){
            type = RETURN_STATEMENT;
            exp = nullptr;
        };
        ~JC_ReturnStatement(){
            if(exp) delete exp;
        };

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
        ~JC_IfStatement(){
            if(cond) delete cond;
            if(true_statements) delete true_statements;
            if(false_statements) delete false_statements;
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
        ~JC_WhileStatement(){
            if(cond) delete cond;
            if(while_body) delete while_body;
        };
        JC_Expression* cond;
        
        JC_MultipleStatement* while_body;
};

class JC_SubroutineBody : public JC_Element
{
    public:
        JC_SubroutineBody(){
            vardec = nullptr;
            statements = nullptr;
        };
        ~JC_SubroutineBody(){
            if(vardec) delete vardec;
            if(statements) delete statements;
        };
        JC_VarDec* vardec;
        JC_Statement* statements;
};

class JC_Subroutine : public JC_Element
{
    public:
        JC_Subroutine(){
            type = nullptr;
            name = nullptr;
            body = nullptr;
            parameterlist = nullptr;
            next = nullptr;
        };

        ~JC_Subroutine(){
            if(type) delete type;
            if(name) delete name;
            if(body) delete body;
            if(parameterlist) delete parameterlist;
            if(next) delete next;
        };

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
        ~JC_ClassVarDec(){
            if(next) delete next;
            if(type) delete type;
            if(VarName) delete VarName;
        };
        void combine(JC_ClassVarDec* rhs){
            JC_ClassVarDec* cur = this;
            while(cur->next){
                cur = cur->next;
            }
            cur->next = rhs;
        };

        std::string         vartype;
        JC_ClassVarDec*     next;
        JC_Type*            type;
        JC_VarName*         VarName;

        int                 index;
        int                 kind;

};

class JC_Class : public JC_Element
{
    public:
        JC_Class(){
            classVarDecs = nullptr;
            classSubroutinDecs = nullptr;
        };
        ~JC_Class(){
            if(classVarDecs) delete classVarDecs;
            if(classSubroutinDecs) delete classSubroutinDecs;
        };
        std::string classname;
        JC_ClassVarDec* classVarDecs;
        JC_Subroutine* classSubroutinDecs;
};

class JC_Program : public JC_Element
{
    public:
        JC_Program(){
            type = JC_PROGRAM;
            cls = nullptr;
        };
        ~JC_Program(){
            if(cls) delete cls;
        }
        JC_Class* cls;
};


#endif