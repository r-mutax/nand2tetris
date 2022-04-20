#include "JackCompiler.h"

#define CHCEK_ERROR(a, b)   do{if(!jt.expect_token(a, b)){ std::cerr << "Expect " << b << "." << std::endl;}}while(0)

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
    symtbl.init();
    JC_Program* prog = new JC_Program();
    prog->cls = compileClass();
    output_program.printProgram(prog);
}

// 'class' className '{' classVarDec* subroutineDec* '}'
JC_Class* CompilationEngine::compileClass()
{
    if(!jt.expect_token(JackTokenizer::KEYWORD, "class"))
    {
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
    if(jt.expect_token(JackTokenizer::SYMBOL, "{")){
        jt.advance();
    }

    // check classVarDec
    JC_ClassVarDec  cvd_head;
    while(JC_ClassVarDec* cur = compileClassVarDec()){
        cvd_head.combine(cur);
    }

    cls->classVarDecs = cvd_head.next;
    cvd_head.next = nullptr;

    // check subroutineDec
    JC_Subroutine subr_head;
    JC_Subroutine* subr_cur = &subr_head;
    while(subr_cur){
        subr_cur->next = compileSubroutine();
        subr_cur = subr_cur->next;
    }
    cls->classSubroutinDecs = subr_head.next;
    subr_head.next = nullptr;

    // check "}"
    if(jt.expect_token(JackTokenizer::SYMBOL, "}")){
        jt.advance();
    }


    return cls;
}

// ( 'static' | 'field' ) type varName ( ',' varName )* ';'
JC_ClassVarDec* CompilationEngine::compileClassVarDec()
{
    std::string var_type;
    JC_Type* type;

    // is classVarDec?
    if(jt.tokenType() != JackTokenizer::KEYWORD
        || (jt.keyword() != "static" && jt.keyword() != "field")) {
        return nullptr;
    }

    // (static || field)
    var_type = jt.keyword();
    jt.advance();

    // judge symbol kind.
    SymbolTable::SYMBOL_KIND kind;
    if(var_type == "static"){
        kind = SymbolTable::SYMBOL_KIND::STATIC;
    } else if(var_type == "field"){
        kind = SymbolTable::SYMBOL_KIND::FIELD;
    }

    // type
    type = compileType();
    if( type == nullptr ){
        std::cerr << "This is not type." << std::endl;
        exit(1);
    }

    // varName
    JC_ClassVarDec head_cvd;
    while(1){ 
        JC_ClassVarDec* cur_cvd = new JC_ClassVarDec();
        cur_cvd->vartype = var_type;

        cur_cvd->type = new JC_Type();
        cur_cvd->type->type = type->type;
        cur_cvd->type->is_keyword = type->is_keyword;

        cur_cvd->VarName = compileVarName();

        symtbl.define(cur_cvd->VarName->name,
                         cur_cvd->type->type, kind);

        cur_cvd->index = symtbl.indexOf(cur_cvd->VarName->name);
        cur_cvd->kind = kind;

        head_cvd.combine(cur_cvd);

        if(jt.expect_token(JackTokenizer::SYMBOL, ";")){
            jt.advance();
            break;
        }

        if(jt.expect_token(JackTokenizer::SYMBOL, ",")){
            jt.advance();
        } else {
            // If .jack program is correct, don't reach here.
        }
    }

    JC_ClassVarDec* ret = head_cvd.next;
    head_cvd.next = nullptr;

    return ret;
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
    if(jt.expect_token(JackTokenizer::SYMBOL, ")")){
        return nullptr;
    }

    JC_Parameter head;
    JC_Parameter* cur = &head;

    while(1){
        cur->next = new JC_Parameter();

        cur->next->type = compileType();
        cur->next->varname = compileVarName();

        symtbl.define(cur->next->varname->name
                        , cur->next->type->type, SymbolTable::ARG);
        
        cur->next->index = symtbl.indexOf(cur->next->varname->name);
        cur->next->kind = SymbolTable::ARG;

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
    JC_Parameter* ret = head.next;
    head.next = nullptr;

    return ret;
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

    symtbl.startSubroutine();

    JC_Subroutine*  subroutine = new JC_Subroutine();
    
    // ('constructor' | 'function' | 'method')
    subroutine->subroutinetype = jt.keyword();
    jt.advance();

    // ('void' | type)
    if(jt.expect_token(JackTokenizer::KEYWORD, "void")){
        subroutine->type = nullptr;
        jt.advance();
    } else {
        subroutine->type = compileType();
    }

    // subroutineName
    subroutine->name = compileSubroutineName();

    // check "("
    if(!jt.expect_token(JackTokenizer::SYMBOL, "(")){
        std::cerr << "Expect ( ." << std::endl;
        exit(1);
    }
    jt.advance();

    // paramaterList
    subroutine->parameterlist = compileParameterList();

    // check ")"
    if(!jt.expect_token(JackTokenizer::SYMBOL, ")")){
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
    if(!jt.expect_token(JackTokenizer::SYMBOL, "{")){
        std::cerr << "Expect } ." << std::endl;
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
    head_vardec.next = nullptr;


    // statements
    subroutinebody->statements = compileStatements();

    // check "}"
    if(!jt.expect_token(JackTokenizer::SYMBOL, "}")){
        std::cerr << "Expect } ." << std::endl;
        //exit(1);        
    }
    jt.advance();

    return subroutinebody;
}

// 'var' type varName (',' varName) * ';'
JC_VarDec* CompilationEngine::compileVarDec()
{
    if(!jt.expect_token(JackTokenizer::KEYWORD, "var")){
        return nullptr;
    }
    jt.advance();

    JC_Type* type = compileType();

    JC_VarDec head_vardec;
    while(1){
        JC_VarDec* vardec = new JC_VarDec();

        vardec->type = new JC_Type();
        vardec->type->is_keyword = type->is_keyword;
        vardec->type->type = type->type;

        vardec->varname = compileVarName();

        symtbl.define(vardec->varname->name,
                    vardec->type->type, SymbolTable::VAR);
        
        vardec->index = symtbl.indexOf(vardec->varname->name);
        vardec->kind = symtbl.kindOf(vardec->varname->name);

        head_vardec.combine(vardec);

        if(jt.expect_token(JackTokenizer::SYMBOL, ";")){
            jt.advance();
            break;
        }

        if(jt.expect_token(JackTokenizer::SYMBOL, ",")){
            jt.advance();
        } else {
            // If .jack program is correct, don't reach here.
        }
    }

    JC_VarDec* ret = head_vardec.next;
    head_vardec.next = nullptr;

    delete type;

    return ret;
}

JC_Statement* CompilationEngine::compileStatements()
{
    JC_Statement head;
    JC_Statement* cur = &head;
    while(cur){
        cur->next = compileSingleStatement();
        cur = cur->next;
    }

    JC_Statement* ret = head.next;
    head.next = nullptr;

    return ret;
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
    else if(jt.keyword() == "do")
    {
        retval = compileDo();
    }
    else if(jt.keyword() == "return")
    {
        retval = compileReturn();
    }
    else if(jt.keyword() == "if")
    {
        retval = compileIf();
    }
    else if(jt.keyword() == "while")
    {
        retval = compileWhile();
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

    if(jt.expect_token(JackTokenizer::SYMBOL, "[")){
        jt.advance();
        var->exp = compileExpression();

        if(!jt.expect_token(JackTokenizer::SYMBOL, "]")){
            std::cerr << "Expect ] ." << std::endl;
        }
        jt.advance();
    }
    let->lhs = var;

    // "="
    if(!jt.expect_token(JackTokenizer::SYMBOL, "=")){
        std::cerr << "Expect = ." << std::endl;
    }
    jt.advance();

    let->rhs = compileExpression();

    // ";"
    if(!jt.expect_token(JackTokenizer::SYMBOL, ";")){
        std::cerr << "Expect ; ." << std::endl;
    }
    jt.advance();

    return let;
}

JC_Expression* CompilationEngine::compileExpressionList()
{
    JC_Expression* exp = compileExpression();
    JC_Expression* cur = exp;

    while(cur){
        if(!jt.expect_token(JackTokenizer::SYMBOL, ",")){
            break;
        }
        jt.advance();

        cur->next = compileExpression();
        cur = cur->next;
    }

    return exp;
}

JC_SubroutineCall* CompilationEngine::compileSubroutineCall()
{
    JC_SubroutineCall* subcall = new JC_SubroutineCall();

    if(jt.tokenType() != JackTokenizer::IDENTIFIER){
        std::cerr << "[error] expect identifier." << std::endl;
        exit(1);
    }

    // is classmethod ?
    if((jt.next_tokenType() == JackTokenizer::SYMBOL)
        && (jt.next_symbol() == "."))
    {
        subcall->classname = jt.identifier();
        jt.advance();

        // skip ".".
        jt.advance();
    }

    // subroutine name
    subcall->subroutine_name = compileSubroutineName();

    // "("
    if(!jt.expect_token(JackTokenizer::SYMBOL, "(")){
        std::cerr << "Expect = (" << std::endl;
    }
    jt.advance();

    // has expressio list?
    if(!jt.expect_token(JackTokenizer::SYMBOL, ")")){
        subcall->exp = compileExpressionList();
    }

    // ")"
    if(!jt.expect_token(JackTokenizer::SYMBOL, ")")){
        std::cerr << "Expect = )" << std::endl;
    }
    jt.advance();

    return subcall;
}

JC_Statement* CompilationEngine::compileDo()
{
    JC_DoStatement* DoStatement = new JC_DoStatement();

    // skip "Do".
    jt.advance();

    DoStatement->subcall = compileSubroutineCall();

    // ";"
    if(!jt.expect_token(JackTokenizer::SYMBOL, ";")){
        std::cerr << "Expect ; ." << std::endl;
    }
    jt.advance();

    return DoStatement;
}
JC_Statement* CompilationEngine::compileReturn()
{
    JC_ReturnStatement* ret = new JC_ReturnStatement();

    // skip "return".
    jt.advance();

    // ";"
    if(!jt.expect_token(JackTokenizer::SYMBOL, ";")){
        // next token is not ";".
        // there is expected having expression.
        ret->exp = compileExpression();
    }

    // Here the token should be ";".
    jt.advance();

    return ret;
}


JC_MultipleStatement* CompilationEngine::compileMultiStatement()
{
    JC_MultipleStatement* multi = new JC_MultipleStatement();
    multi->statement_body = compileStatements();

    return multi;
}

// 'if' '(' expression ')'
// '{' statements '}'
// 'else' '{' statements '}'
JC_Statement* CompilationEngine::compileIf()
{
    JC_IfStatement* if_statement = new JC_IfStatement();

    // skip "if"
    jt.advance();

    if(!jt.expect_token(JackTokenizer::SYMBOL, "(")) {
        std::cerr << "Expect ( ." << std::endl;
    }
    jt.advance();

    if_statement->cond = compileExpression();
    CHCEK_ERROR(JackTokenizer::SYMBOL, ")");
    jt.advance();

    CHCEK_ERROR(JackTokenizer::SYMBOL, "{");
    jt.advance();

    if_statement->true_statements = compileMultiStatement();
    CHCEK_ERROR(JackTokenizer::SYMBOL, "}");
    jt.advance();

    if(jt.expect_token(JackTokenizer::KEYWORD, "else"))
    {
        jt.advance();

        CHCEK_ERROR(JackTokenizer::SYMBOL, "{");
        jt.advance();

        if_statement->false_statements = compileMultiStatement();
        CHCEK_ERROR(JackTokenizer::SYMBOL, "}");
        jt.advance();
    }

    return if_statement;
}

JC_Statement* CompilationEngine::compileWhile()
{
    if(!jt.expect_token(JackTokenizer::KEYWORD, "while")){
        return nullptr;
    }
    jt.advance();

    JC_WhileStatement* whilestatement = new JC_WhileStatement();

    CHCEK_ERROR(JackTokenizer::SYMBOL, "(");
    jt.advance();
    whilestatement->cond = compileExpression();
    CHCEK_ERROR(JackTokenizer::SYMBOL, ")");
    jt.advance();

    CHCEK_ERROR(JackTokenizer::SYMBOL, "{");
    jt.advance();
    whilestatement->while_body = compileMultiStatement();
    CHCEK_ERROR(JackTokenizer::SYMBOL, "}");
    jt.advance();

    return whilestatement;
}

JC_Operand* CompilationEngine::compileOp()
{
    if(jt.tokenType() != JackTokenizer::SYMBOL)
    {
        return nullptr;
    }

    if((jt.symbol() == "+")
        || (jt.symbol() == "-")
        || (jt.symbol() == "*")
        || (jt.symbol() == "/")
        || (jt.symbol() == "&")
        || (jt.symbol() == "|")
        || (jt.symbol() == "<")
        || (jt.symbol() == ">")
        || (jt.symbol() == "=")){

            JC_Operand* ope = new JC_Operand();
            ope->op = jt.symbol();
            jt.advance();
            return ope;
    }

    return nullptr;
}

// term (op term)*
JC_Expression* CompilationEngine::compileExpression()
{
    JC_Expression* exp = new JC_Expression();
    
    exp->term = compileTerm();

    JC_Term* cur = (JC_Term*)(exp->term);

    while(cur){
        if(jt.tokenType() != JackTokenizer::SYMBOL)
        {
            break;
        }

        // operand
        cur->op = compileOp();
        if(cur->op == nullptr){
            break;
        }

        cur->next = compileTerm();
        cur = (JC_Term*)(cur->next);
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
        jt.advance();
    } else if(jt.tokenType() == JackTokenizer::STRING_CONST) {
        term->termtype = STRING_CONST;
        term->stringVal = jt.stringVal();
        jt.advance();
    } else if(jt.tokenType() == JackTokenizer::KEYWORD){
        if( (jt.keyword() == "true")
            || (jt.keyword() == "false")
            || (jt.keyword() == "null")
            || (jt.keyword() == "this")){
            term->termtype = KEYWORD_CONST;
            term->stringVal = jt.keyword();
            jt.advance();
        } else {
            std::cerr << "error keyword." << std::endl;
            return nullptr;
        }
    } else if(jt.expect_token(JackTokenizer::SYMBOL, "(")){
        jt.advance();

        term->exp = compileExpression();
        term->termtype = EXPRESSION;

        if(!jt.expect_token(JackTokenizer::SYMBOL, ")")){
            delete term;
            return nullptr;
        }
        jt.advance();
    } else if(jt.expect_token(JackTokenizer::SYMBOL, "-")
            || jt.expect_token(JackTokenizer::SYMBOL, "~")){
        term->unary_op = new JC_Operand();
        term->unary_op->op = jt.symbol();
        term->termtype = UNARYOP_TERM;

        jt.advance();
        
        term->next = compileTerm();
    } else if(jt.tokenType() == JackTokenizer::IDENTIFIER){

        if(jt.next_tokenType() == JackTokenizer::SYMBOL
            && (jt.next_symbol() == "." || (jt.next_symbol() == "(")))
        {
            // identifier '.' identifier '(' expressions? ')'
            // | identifier '(' expressions? ')'
            term->termtype = SUBROUTINECALL;
            term->subcall = compileSubroutineCall();
        } else {
            // array or variant
            term->termtype = VARIANT;
            term->var = compileVariant();
        }
    } else {
        delete term;
        term = nullptr;
    }


    return term;
}

// varName | varName '[' expression ']'
JC_Variant* CompilationEngine::compileVariant()
{
    JC_Variant* var = new JC_Variant();

    var->varname = compileVarName();

    // is array?
    if(jt.expect_token(JackTokenizer::SYMBOL, "[")){
        jt.advance();
        var->exp = compileExpression();

        if(jt.expect_token(JackTokenizer::SYMBOL, "]")){
            jt.advance();
        } else {
            std::cerr << "[error] expected ']'." << std::endl;
        }
    }
    return var;
}