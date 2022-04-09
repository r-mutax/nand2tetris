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

    // check subroutineDec
    JC_Subroutine subr_head;
    JC_Subroutine* subr_cur = &subr_head;
    while(subr_cur){
        subr_cur->next = compileSubroutine();
        subr_cur = subr_cur->next;
    }
    cls->classSubroutinDecs = subr_head.next;

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

// identifier
JC_SubroutineName* CompilationEngine::compileSubroutineName()
{
    if(jt.tokenType() == JackTokenizer::IDENTIFIER)
    {
        JC_SubroutineName* subname = new JC_SubroutineName();

        subname->name = jt.identifier();
        jt.advance();
        return subname;
    }
    return nullptr;
}

// ( (type varName) ( ',' type varName )* )?
JC_Parameter* CompilationEngine::compileParameterList()
{
    if (jt.tokenType() == JackTokenizer::SYMBOL
        && jt.symbol() == ")")
    {
        return nullptr;
    }

    JC_Parameter head;
    JC_Parameter* cur = &head;

    while(1){
        cur->next = new JC_Parameter();

        cur->next->type = compileType();
        cur->next->varname = compileVarName();

        if(jt.tokenType() == JackTokenizer::SYMBOL)
        {
            if(jt.symbol() == ")"){
                break;
            } else if(jt.symbol() == ","){
                jt.advance();
            } else {
                std::cerr << "JC_Parameter : unexpected error." << std::endl;
                exit(1);
            }
        }
        cur = cur->next;
    }

    return head.next;
}

// ('constructor' | 'function' | 'method')
// ('void' | type) subroutineName '('
//  parameterList ')'
// subroutineBody
JC_Subroutine* CompilationEngine::compileSubroutine()
{
    // is SubroutineDec?
    if (jt.tokenType() != JackTokenizer::KEYWORD
        || ( jt.keyword() != "constructor"
            && jt.keyword() != "function"
            &&  jt.keyword() != "method")
    ){
        return nullptr;
    }

    JC_Subroutine*  subroutine = new JC_Subroutine();
    
    // ('constructor' | 'function' | 'method')
    subroutine->subroutinetype = jt.keyword();
    jt.advance();

    // ('void' | type)
    if(jt.tokenType() == JackTokenizer::KEYWORD && jt.keyword() == "void"){
        subroutine->type = nullptr;
        jt.advance();
    } else {
        subroutine->type = compileType();
    }

    // subroutineName
    subroutine->name = compileSubroutineName();

    // check "("
    if(jt.tokenType() != JackTokenizer::SYMBOL
        || jt.symbol() != "(")
    {
        std::cerr << "Expect ( ." << std::endl;
        exit(1);
    }
    jt.advance();

    // paramaterList
    subroutine->parameterlist = compileParameterList();

    // check ")"
    if(jt.tokenType() != JackTokenizer::SYMBOL
        || jt.symbol() != ")")
    {
        std::cerr << "Expect ) ." << std::endl;
        exit(1);        
    }
    jt.advance();

    subroutine->body = compileSubroutineBody();

    return subroutine;
}

// '{' varDec* statements '}'
JC_SubroutineBody* CompilationEngine::compileSubroutineBody()
{
    // check "{"
    if(jt.tokenType() != JackTokenizer::SYMBOL
        || jt.symbol() != "{")
    {
        std::cerr << "Expect { ." << std::endl;
        exit(1);        
    }
    jt.advance();

    JC_SubroutineBody* subroutinebody = new JC_SubroutineBody();

    // varDec*

    JC_VarDec head_vardec;
    JC_VarDec* cur_vardec = &head_vardec;
    while(cur_vardec){
        cur_vardec->next = compileVarDec();
        cur_vardec = cur_vardec->next;
    }
    subroutinebody->vardec = head_vardec.next;


    // statements
    subroutinebody->statements = compileStatements();

    // check "}"
    if(jt.tokenType() != JackTokenizer::SYMBOL
        || jt.symbol() != "}")
    {
        std::cerr << "Expect } ." << std::endl;
        //exit(1);        
    }
    jt.advance();

    return subroutinebody;
}

// 'var' type varName (',' varName) * ';'
JC_VarDec* CompilationEngine::compileVarDec()
{
    if (jt.tokenType() != JackTokenizer::KEYWORD
        || jt.keyword() != "var")
    {
        return nullptr;
    }
    jt.advance();

    JC_VarDec* vardec = new JC_VarDec();

    vardec->type = compileType();
    vardec->varname = compileVarName();

    JC_VarName* varname = vardec->varname;

    while(jt.tokenType() == JackTokenizer::SYMBOL
        && jt.symbol() == ",")
    {
        jt.advance();
        varname->next = compileVarName();
        varname = varname->next;
    }

    // check ";"
    if(jt.tokenType() != JackTokenizer::SYMBOL
        || jt.symbol() != ";")
    {
        std::cerr << "Expect ; ." << std::endl;
        exit(1);        
    }
    jt.advance();

    return vardec;
}

JC_Statement* CompilationEngine::compileStatements()
{
    JC_Statement head;
    JC_Statement* cur = &head;
    while(cur){
        cur->next = compileSingleStatement();
        cur = cur->next;
    }

    return head.next;
}

JC_Statement* CompilationEngine::compileSingleStatement()
{
    if(jt.tokenType() != JackTokenizer::KEYWORD)
    {
        return nullptr;
    }

    JC_Statement* retval = nullptr;

    if(jt.keyword() == "let")
    {
        retval = compileLet();
    }
    else
    {
        std::cerr << "errorStatement." << std::endl;
    }

    return retval;
}

// "let" varName ( '[' expression ']')? '=' expression ';'
JC_Statement* CompilationEngine::compileLet()
{
    JC_LetStatement* let = new JC_LetStatement();

    // skip "let"
    jt.advance();

    // varName [ expression ]
    JC_Variant* var = new JC_Variant();
    var->varname = compileVarName();

    if(jt.tokenType() == JackTokenizer::SYMBOL
        && jt.symbol() == "[")
    {
        var->exp = compileExpression();

        if(jt.tokenType() != JackTokenizer::SYMBOL
            || jt.symbol() == "]" )
        {
            std::cerr << "Expect ] ." << std::endl;
        }
        jt.advance();
    }
    let->lhs = var;

    // "=""
    if(jt.tokenType() == JackTokenizer::SYMBOL
        || jt.symbol() == "=")
    {
        std::cerr << "Expect = ." << std::endl;
    }
    jt.advance();

    let->rhs = compileExpression();

    // ";"
    if(jt.tokenType() == JackTokenizer::SYMBOL
        || jt.symbol() == ";")
    {
        std::cerr << "Expect ; ." << std::endl;
    }

    return let;
}

// term (op term)*
JC_Expression* CompilationEngine::compileExpression()
{
    JC_Expression* exp = new JC_Expression();
    
    exp->term = compileTerm();

    JC_Term* cur = exp->term;

    while(cur){
        if(jt.tokenType() != JackTokenizer::SYMBOL)
        {
            break;
        }

        // operand
        cur->op = jt.symbol();
        jt.advance();

        cur->next = compileTerm();

        cur = cur->next;
    }

    return exp;
}

// integerConstant | stringConstant
// | keywordConstant | varName | varName '[' expression ']'
// | subroutineCall | '(' expression ')' | unaryOp Term
JC_Term* CompilationEngine::compileTerm()
{
    JC_Term* term = new JC_Term();
    if(jt.tokenType() == JackTokenizer::INT_CONST)
    {
        term->termtype = INTEGER_CONST;
        term->integerVal = jt.intVal();
    } else if(jt.tokenType() == JackTokenizer::STRING_CONST) {
        term->termtype = STRING_CONST;
        term->stringVal = jt.stringVal();
    } else if(jt.tokenType() == JackTokenizer::KEYWORD){
        if( (jt.keyword() == "true")
            || (jt.keyword() == "false")
            || (jt.keyword() == "null")
            || (jt.keyword() == "this")){
            term->termtype = KEYWORD_CONST;
            term->stringVal = jt.keyword();
        } else {
            std::cerr << "error keyword." << std::endl;
            return nullptr;
        }
    } else if(jt.tokenType() == JackTokenizer::IDENTIFIER){
        std::string ident = jt.identifier();
        jt.advance();

        // is variant, array or subroutineCall?
        if(jt.tokenType() == JackTokenizer::SYMBOL){
            
            // array
            if(jt.symbol() == "["){
                
            }
        }

        term->termtype = VARIANT;
        term->var = new JC_Variant();
        term->var->varname = new JC_VarName();
        term->var->varname->name = ident;
    }

    return term;
}