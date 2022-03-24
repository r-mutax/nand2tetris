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

class JC_Subroutine : public JC_Element
{
    public:
        std::string         subroutinetype;
        JC_Type*            type;
        JC_SubroutineName*  name;
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