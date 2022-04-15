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

    // subroutine body
    printSubroutineBody(csr->body);

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

// '{' varDec* statements '}'
void OutputProgram::printSubroutineBody(JC_SubroutineBody* body)
{
    if(body == nullptr){
        std::cerr << "expect subroutinebody." << std::endl;
        exit(1);
    }

    m_xml.printDataHead("subroutineBody");

    m_xml.printDataLine("symbol", "{");

    // print VarDecs
    JC_VarDec* cur = body->vardec;
    while(cur)
    {
        printVarDec(cur);
        cur = cur->next;
    }

    // print statements
    printStatements(body->statements);

    m_xml.printDataLine("symbol", "}");

    m_xml.printDataTail("subroutineBody");
}

// 'var' type varName (',' varName) * ';'
void OutputProgram::printVarDec(JC_VarDec* vardec)
{
    m_xml.printDataHead("varDec");
    m_xml.printDataLine("keyword", "var");
    JC_Type* type = ((JC_Type*)(vardec->type));
    m_xml.printDataLine(type->is_keyword ? "keyword" : "identifier", type->type);

    JC_VarName* varname = vardec->varname;

    m_xml.printDataLine("identifier", varname->name);
    varname = varname->next;

    while(varname){
        m_xml.printDataLine("symbol", ",");
        m_xml.printDataLine("identifier", varname->name);
        varname = varname->next;
    }
    vardec = vardec->next;
    m_xml.printDataLine("symbol", ";");
    m_xml.printDataTail("varDec");
}

void OutputProgram::printMultipleStatements(JC_MultipleStatement* multi_statement)
{
    printStatements(multi_statement->statement_body);
 
    // m_xml.printDataHead("statements");

    // JC_Statement* cur = multi_statement->statement_body;
    // while(cur){
    //     switch(cur->type){
    //         case LET_STATEMENT:
    //             printLetStatement((JC_LetStatement*)cur);
    //             break;
    //         case DO_STATEMENT:
    //             printDoStatement((JC_DoStatement*)cur);
    //             break;
    //         case RETURN_STATEMENT:
    //             printReturnStatement((JC_ReturnStatement*)cur);
    //             break;
    //         case IF_STATEMENT:
    //             printIfStatement((JC_IfStatement*)cur);
    //             break;
    //     }
    //     cur = cur->next;
    // }

    // m_xml.printDataTail("statements");
}

void OutputProgram::printStatements(JC_Statement* statements)
{
    m_xml.printDataHead("statements");

    JC_Statement* cur = statements;
    while(cur){
        switch(cur->type){
            case LET_STATEMENT:
                printLetStatement((JC_LetStatement*)cur);
                break;
            case DO_STATEMENT:
                printDoStatement((JC_DoStatement*)cur);
                break;
            case RETURN_STATEMENT:
                printReturnStatement((JC_ReturnStatement*)cur);
                break;
            case IF_STATEMENT:
                printIfStatement((JC_IfStatement*)cur);
                break;
            case WHILE_STATEMENT:
                printWhileStatement((JC_WhileStatement*)cur);
                break;
        }
        cur = cur->next;
    }

    m_xml.printDataTail("statements");

}

// "let" varName ( '[' expression ']')? '=' expression ';'
void OutputProgram::printLetStatement(JC_LetStatement* letstatement)
{
    m_xml.printDataHead("letStatement");
    
    m_xml.printDataLine("keyword", "let");

    // print lhs
    JC_Variant* var = letstatement->lhs;
    m_xml.printDataLine("identifier", var->varname->name);

    // is array?
    if(var->exp){
        m_xml.printDataLine("symbol", "[");

        // printExpression(var->exp);

        m_xml.printDataLine("symbol", "]");
    }

    // print = 
    m_xml.printDataLine("symbol", "=");

    printExpression(letstatement->rhs);

    // print ;
    m_xml.printDataLine("symbol", ";");

    m_xml.printDataTail("letStatement");
}

void OutputProgram::printDoStatement(JC_DoStatement* dostatement)
{
    m_xml.printDataHead("doStatement");

    m_xml.printDataLine("keyword", "do");

    JC_SubroutineCall* subcall = dostatement->subcall;

    if(subcall->classname != ""){
        m_xml.printDataLine("identifier", subcall->classname);
        m_xml.printDataLine("symbol", ".");
    }

    m_xml.printDataLine("identifier", subcall->subroutine_name->name);
    m_xml.printDataLine("symbol", "(");

    printExpressionList(subcall->exp);

    m_xml.printDataLine("symbol", ")");

    m_xml.printDataLine("symbol", ";");

    m_xml.printDataTail("doStatement");
}

void OutputProgram::printReturnStatement(JC_ReturnStatement* returnstatement)
{
    m_xml.printDataHead("returnStatement");

    m_xml.printDataLine("keyword", "return");

    if(returnstatement->exp){
        printExpression(returnstatement->exp);
    }

    m_xml.printDataLine("symbol", ";");

    m_xml.printDataTail("returnStatement");
}

void OutputProgram::printIfStatement(JC_IfStatement* ifstatement)
{
    m_xml.printDataHead("ifStatement");

    m_xml.printDataLine("keyword", "if");
    m_xml.printDataLine("symbol", "(");

    printExpression(ifstatement->cond);
    m_xml.printDataLine("symbol", ")");

    m_xml.printDataLine("symbol", "{");
    printMultipleStatements(ifstatement->true_statements);
    m_xml.printDataLine("symbol", "}");

    if(ifstatement->false_statements){
        m_xml.printDataLine("keyword", "else");
        
        m_xml.printDataLine("symbol", "{");
       printMultipleStatements(ifstatement->false_statements);
        m_xml.printDataLine("symbol", "}");
    }

    m_xml.printDataTail("ifStatement");
}

void OutputProgram::printWhileStatement(JC_WhileStatement* whilestatement)
{
    m_xml.printDataHead("whileStatement");

    m_xml.printDataLine("keyword", "while");

    m_xml.printDataLine("symbol", "(");
    printExpression(whilestatement->cond);
    m_xml.printDataLine("symbol", ")");

    m_xml.printDataLine("symbol", "{");
    printMultipleStatements(whilestatement->while_body);
    m_xml.printDataLine("symbol", "}");

    m_xml.printDataTail("whileStatement");
}

// integerConstant | stringConstant
// | keywordConstant | varName | varName '[' expression ']'
// | subroutineCall | '(' expression ')' | unaryOp Term
void OutputProgram::printTerm(JC_Term* term)
{
    m_xml.printDataHead("term");

    switch(term->termtype)
    {
        case INTEGER_CONST:
            m_xml.printDataLine("integerConstant", std::to_string(term->integerVal));
            break;
        case STRING_CONST:
            m_xml.printDataLine("stringConstant", term->stringVal);
            break;
        case VARIANT:
            printVariant(term->var);
            break;
        case KEYWORD_CONST:
            m_xml.printDataLine("keyword", term->stringVal);
            break;
        case EXPRESSION:
            printExpression(term->exp);
            break;
        case UNARYOP_TERM:
            break;
    }

    m_xml.printDataTail("term");
}

// term (op term)*
void OutputProgram::printExpression(JC_Expression* expression)
{
    m_xml.printDataHead("expression");

    printTerm(expression->term);

    m_xml.printDataTail("expression");
}

void OutputProgram::printExpressionList(JC_Expression* exp_list)
{
    m_xml.printDataHead("expressionList");

    if(exp_list){
        printExpression(exp_list);

        JC_Expression* cur = exp_list->next;
        
        while(cur){
            m_xml.printDataLine("symbol", ",");
            printExpression(cur);
            cur = cur->next;
        }
    }
    m_xml.printDataTail("expressionList");
}

void OutputProgram::printVariant(JC_Variant* var)
{
    m_xml.printDataLine("identifier", var->varname->name);

    if(var->exp){
        m_xml.printDataLine("symbol", "[");
        printExpression(var->exp);
        m_xml.printDataLine("symbol", "]");
    }   
}