#include "JackCompiler.h"

VMWriter::VMWriter(){

};

void VMWriter::init(){
}

void VMWriter::setFileName(std::string filename){
    m_ofs.open(filename);
}

void VMWriter::genVMFile(JC_Program* prog){
    
    JC_Class* cls = prog->cls;
    classname = cls->classname;

    JC_Subroutine* subroutine_cur = cls->classSubroutinDecs;

    while(subroutine_cur){

        // init label count.
        while_cnt = 0;
        if_cnt = 0;

        if(subroutine_cur->subroutinetype == "constructor"){
            genConstructor(subroutine_cur, cls->field_cnt);
        } else if(subroutine_cur->subroutinetype == "function"){
            genFunction(subroutine_cur);
        } else if(subroutine_cur->subroutinetype == "method"){
            genMethod(subroutine_cur);
        }
        subroutine_cur = subroutine_cur->next;
    }
}

void VMWriter::genConstructor(JC_Subroutine* constructor, int32_t size){

    std::string funcname = classname + "." + constructor->name->name;
    writeFunction(funcname, constructor->varcount);

    // alloc class field.
    writePush(CONST, size);
    writeCall("Memory.alloc", 1);
    writePop(POINTER, 0);

    genMultipleStatements(constructor->body->statements);
}

void VMWriter::genFunction(JC_Subroutine* function)
{
    std::string funcname = getFunctionName(function);

    writeFunction(funcname, function->varcount);

    genMultipleStatements(function->body->statements);
}

void VMWriter::genMethod(JC_Subroutine* method){

    std::string funcname = classname + "." + method->name->name;
    int32_t varcount = method->varcount;

    writeFunction(funcname, varcount);

    // pop argument to pointer 0
    writePush(ARG, 0);
    writePop(POINTER, 0);

    genMultipleStatements(method->body->statements);
}

void VMWriter::genMultipleStatements(JC_Statement* multi_statements)
{
    JC_Statement* cur = multi_statements;
    while(cur){
        switch(cur->type){
            case LET_STATEMENT:
                genLetStatement((JC_LetStatement*) cur);
                break;
            case IF_STATEMENT:
                genIfStatement((JC_IfStatement*) cur);
                break;
            case WHILE_STATEMENT:
                genWhileStatement((JC_WhileStatement*) cur);
                break;
            case DO_STATEMENT:
                genDoStatement((JC_DoStatement*) cur);
                break;
            case RETURN_STATEMENT:
                genReturnStatement((JC_ReturnStatement*) cur);
                break;
            case UNKNOWN_STATEMENT:
                break;
        }
        cur = cur->next;
    }

}

void VMWriter::genLetStatement(JC_LetStatement* let_stmt){

    JC_Variant* var = let_stmt->lhs;

    // if rhs is array.
    if(var->exp){
        genExpression(var->exp);

        switch(var->kind){
            case SymbolTable::STATIC:
                writePush(STATIC, var->index);
                break;
            case SymbolTable::FIELD:
                writePush(THIS, var->index);
                break;
            case SymbolTable::ARG:
                writePush(ARG, var->index);
                break;
            case SymbolTable::VAR:
                writePush(LOCAL, var->index);
                break;
        }
        writeOperand("+");
    }

    // push stack rhs.
    genExpression(let_stmt->rhs);

    // pop to lhs.
    if(var->exp){
        writePop(TEMP, 0);      // escap expression retval.
        writePop(POINTER, 1);   // set "THAT" pointer.
        writePush(TEMP, 0);     // return expression retval.
        writePop(THAT, 0);      // pop to lhs.
    } else {
        switch(var->kind){
            case SymbolTable::STATIC:
                writePop(STATIC, var->index);
                break;
            case SymbolTable::FIELD:
                writePop(THIS, var->index);
                break;
            case SymbolTable::ARG:
                writePop(ARG, var->index);
                break;
            case SymbolTable::VAR:
                writePop(LOCAL, var->index);
                break;
        }
    }
}

void VMWriter::genIfStatement(JC_IfStatement* if_stmt){

    int32_t if_label_cnt = if_cnt++;
    std::string true_label = "IF_TRUE" + std::to_string(if_label_cnt);
    std::string false_label = "IF_FALSE" + std::to_string(if_label_cnt);
    std::string end_label = "IF_END" + std::to_string(if_label_cnt);

    genExpression(if_stmt->cond);

    m_ofs << "if-goto " << true_label << std::endl;
    writeGoto(false_label);

    // true stmts.
    writeLabel(true_label);
    genMultipleStatements(if_stmt->true_statements->statement_body);
    if(if_stmt->false_statements){
        writeGoto(end_label);
    }

    writeLabel(false_label);
    // false stmts.
    if(if_stmt->false_statements){    
        genMultipleStatements(if_stmt->false_statements->statement_body);
        writeLabel(end_label);
    }
    
}

void VMWriter::genWhileStatement(JC_WhileStatement* while_stmt){

    int32_t while_label_cnt = while_cnt++;
    std::string while_head = "WHILE_EXP" + std::to_string(while_label_cnt);
    std::string break_point = "WHILE_END" + std::to_string(while_label_cnt);

    writeLabel(while_head);
    genExpression(while_stmt->cond);
    m_ofs << "not" << std::endl;

    m_ofs << "if-goto " << break_point << std::endl;
    genMultipleStatements(while_stmt->while_body->statement_body);
    writeGoto(while_head);

    writeLabel(break_point);
}

void VMWriter::genDoStatement(JC_DoStatement* do_stmt)
{
    genSubcall(do_stmt->subcall);
    writePop(TEMP, 0);
}

void VMWriter::genReturnStatement(JC_ReturnStatement* ret_stmt){

    if(ret_stmt->exp){
        genExpression(ret_stmt->exp);
    } else {
        writePush(CONST, 0);
    }

    writeReturn();

    return;
}

void VMWriter::genExpression(JC_Expression* exp){
    
    JC_Term* term = (JC_Term*)exp->term;

    // has rhs and lhs
    if(term->op){
        genTerm(term);
        genTerm((JC_Term*)term->next);
        writeOperand(term->op->op);
    } else {
        genTerm(term);        
    }
}

void VMWriter::genTerm(JC_Term* term){
    switch(term->termtype){
        case INTEGER_CONST:
            writePush(CONST, term->integerVal);
            break;
        case STRING_CONST:
        {
            int length = term->stringVal.length();
            writePush(CONST, length);
            writeCall("String.new", 1);

            for(int i = 0; i < length; i++){
                writePush(CONST, (int32_t)(term->stringVal[i]));
                writeCall("String.appendChar", 2);
            }
        }
            break;
        case KEYWORD_CONST:
            if(term->stringVal == "true"){
                writePush(CONST, 0);
                m_ofs << "not" << std::endl;
            } else if(term->stringVal == "false"){
                writePush(CONST, 0);
            } else if(term->stringVal == "null"){
                writePush(CONST, 0);
            } else if(term->stringVal == "this"){
                writePush(POINTER, 0);
            }
            break;
        case VARIANT:
        {
            bool isArray = false;
            if(((JC_Variant*)(term->var))->exp){
                isArray = true;
            }

            if(isArray){
                genExpression(((JC_Variant*)(term->var))->exp);
            }
            genVariant((JC_Variant*)term->var);
            if(isArray){
                writeOperand("+");
                writePop(POINTER, 1);
                writePush(THAT, 0);
            }
        }
            break;
        case SUBROUTINECALL:
            genSubcall(term->subcall);
            break;
        case EXPRESSION:
            genExpression(term->exp);
            break;
        case UNARYOP_TERM:
            genTerm((JC_Term*)term->next);
            if(term->unary_op->op == "-"){
                m_ofs << "neg" << std::endl;
            } else if (term->unary_op->op == "~"){
                m_ofs << "not" << std::endl;
            }
            break;
    }
}

void VMWriter::genSubcall(JC_SubroutineCall* subcall){
    
    std::string subcall_cls = "";

    if(subcall->is_var){
        subcall_cls = subcall->tyname;
    } else if(subcall->is_method && subcall->classname == ""){
        subcall_cls = classname;
    } else {
        subcall_cls = subcall->classname;
    }

    std::string funcname = subcall_cls + "." + subcall->subroutine_name->name;

    // if call "method", push class variable pointer.
    if(subcall->is_method){
        if(subcall->classname == ""){
            writePush(POINTER, 0);            
        } else {
            switch(subcall->kind){
                case SymbolTable::STATIC:
                    writePush(STATIC, subcall->index);
                    break;
                case SymbolTable::FIELD:
                    writePush(THIS, subcall->index);
                    break;
                case SymbolTable::ARG:
                    writePush(ARG, subcall->index);
                    break;
                case SymbolTable::VAR:
                    writePush(LOCAL, subcall->index);
                    break;
                }
        }
    }

    // push arguments.
    int32_t nLocals = 0;
    JC_Expression* cur = subcall->exp;
    while(cur){
        nLocals++;
        genExpression(cur);
        cur = cur->next;
    }
    if(subcall->is_method){
        nLocals++;
    }

    writeCall(funcname, nLocals);

    return ;
}

void VMWriter::genVariant(JC_Variant* var){

    switch(var->kind){
        case SymbolTable::STATIC:
            writePush(STATIC, var->index);
            break;
        case SymbolTable::FIELD:
            writePush(THIS, var->index);
            break;
        case SymbolTable::ARG:
            writePush(ARG, var->index);
            break;
        case SymbolTable::VAR:
            writePush(LOCAL, var->index);
            break;
    }
}

int32_t VMWriter::countParameter(JC_Subroutine* function){

    int32_t count = 0;
    JC_Parameter* param = function->parameterlist;
    while(param){
        count++;
        param = param->next;
    }
    return count;
}

std::string VMWriter::getFunctionName(JC_Subroutine* function){
    return classname + "." + function->name->name;
}

void VMWriter::writePush(VM_Segment seg, int32_t idx){
    switch(seg){
        case CONST:
            m_ofs << "push constant " << idx << std::endl;
            break;
        case ARG:
            m_ofs << "push argument " << idx << std::endl;
            break;
        case LOCAL:
            m_ofs << "push local " << idx << std::endl;
            break;
        case STATIC:
            m_ofs << "push static " << idx << std::endl;
            break;
        case THIS:
            m_ofs << "push this " << idx << std::endl;
            break;
        case THAT:
            m_ofs << "push that " << idx << std::endl;
            break;
        case POINTER:
            m_ofs << "push pointer " << idx << std::endl;
            break;
        case TEMP:
            m_ofs << "push temp " << idx << std::endl;
            break;
    }
}

void VMWriter::writePop(VM_Segment seg, int32_t idx){
       switch(seg){
        case ARG:
            m_ofs << "pop argument " << idx << std::endl;
            break;
        case LOCAL:
            m_ofs << "pop local " << idx << std::endl;
            break;
        case STATIC:
            m_ofs << "pop static " << idx << std::endl;
            break;
        case TEMP:
            m_ofs << "pop temp " << idx << std::endl;
            break;
        case POINTER:
            m_ofs << "pop pointer " << idx << std::endl;
            break;
        case THIS:
            m_ofs << "pop this " << idx << std::endl;
            break;
        case THAT:
            m_ofs << "pop that " << idx << std::endl;
            break;
    }
 
}

void VMWriter::writeLabel(std::string label){
    m_ofs << "label " << label << std::endl;
}

void VMWriter::writeGoto(std::string label){
    m_ofs << "goto " << label << std::endl;
}

void VMWriter::writeFunction(std::string name, int32_t nLocals)
{
    m_ofs << "function " << name << " " << nLocals << std::endl;
}

void VMWriter::writeReturn(){
    m_ofs << "return" << std::endl;
}

void VMWriter::writeCall(std::string funcname, int32_t nArgs){
    m_ofs << "call " << funcname << " " << nArgs << std::endl;
}

void VMWriter::writeOperand(std::string op){
    if(op == "+"){
        m_ofs << "add" << std::endl;
    } else if(op == "-"){
        m_ofs << "sub" << std::endl;
    } else if(op == "*"){
        m_ofs << "call Math.multiply 2" << std::endl;
    } else if(op == "/"){
        m_ofs << "call Math.divide 2" << std::endl;
    } else if(op == "&"){
        m_ofs << "and" << std::endl;
    } else if(op == "|"){
        m_ofs << "or" << std::endl;
    } else if(op == "<"){
        m_ofs << "lt" << std::endl;
    } else if(op == ">"){
        m_ofs << "gt" << std::endl;
    } else if(op == "="){
        m_ofs << "eq" << std::endl;
    } else {

    }
}