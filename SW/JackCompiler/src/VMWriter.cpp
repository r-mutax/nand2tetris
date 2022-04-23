#include "JackCompiler.h"

VMWriter::VMWriter(){

};

void VMWriter::setFileName(std::string filename){
    m_ofs.open(filename);
}

void VMWriter::genVMFile(JC_Program* prog){
    
    JC_Class* cls = prog->cls;
    classname = cls->classname;

    JC_Subroutine* subroutine_cur = cls->classSubroutinDecs;

    while(subroutine_cur){
        
        if(subroutine_cur->subroutinetype == "constructor"){
            genConstructor(subroutine_cur);
        } else if(subroutine_cur->subroutinetype == "function"){
            genFunction(subroutine_cur);
        } else if(subroutine_cur->subroutinetype == "method"){
            genMethod(subroutine_cur);
        }
        subroutine_cur = subroutine_cur->next;
    }
}

void VMWriter::genConstructor(JC_Subroutine* constructor){

}

void VMWriter::genFunction(JC_Subroutine* function)
{
    int param_num = countParameter(function);
    std::string funcname = getFunctionName(function);

    writeFunction(funcname, param_num);

    genMultipleStatements(function->body->statements);
}

void VMWriter::genMethod(JC_Subroutine* method){

}

void VMWriter::genMultipleStatements(JC_Statement* multi_statements)
{
    JC_Statement* cur = multi_statements;
    while(cur){
        switch(cur->type){
            case LET_STATEMENT:
                break;
            case IF_STATEMENT:
                break;
            case WHILE_STATEMENT:
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

void VMWriter::genDoStatement(JC_DoStatement* do_stmt)
{
    // if call "method", push "this" pointer.
    if(do_stmt->subcall->is_method){
        writePush(POINTER, 0);
    }

    // push arguments.
    int32_t nLocals = 0;
    JC_Expression* cur = do_stmt->subcall->exp;
    while(cur){
        nLocals++;
        genExpression(cur);
        cur = cur->next;
    }

    // function call.
    std::string funcname = do_stmt->subcall->subroutine_name->name;
    if(do_stmt->subcall->is_method){
        nLocals++;
    }

    if(do_stmt->subcall->classname != ""){
        funcname = do_stmt->subcall->classname + "." + funcname;
    }

    writeCall(funcname, nLocals);
}

void VMWriter::genReturnStatement(JC_ReturnStatement* ret_stmt){

    if(ret_stmt->exp){
        genExpression(ret_stmt->exp);
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
        writeOperand(term->op);
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
            break;
        case KEYWORD_CONST:
            break;
        case VARIANT:
            break;
        case SUBROUTINECALL:
            break;
        case EXPRESSION:
            genExpression(term->exp);
            break;
        case UNARYOP_TERM:
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
        case LOCAL:
        case STATIC:
        case THIS:
        case THAT:
            break;
        case POINTER:
            m_ofs << "push pointer " << idx << std::endl;
            break;
        case TEMP:
            break;
    }
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

void VMWriter::writeOperand(JC_Operand* op){
    if(op->op == "+"){
        m_ofs << "add" << std::endl;
    } else if(op->op == "-"){
        m_ofs << "sub" << std::endl;
    } else if(op->op == "*"){
        m_ofs << "call Math.multiply 2" << std::endl;
    } else if(op->op == "/"){
        m_ofs << "call Math.divide 2" << std::endl;
    } else if(op->op == "&"){
        m_ofs << "and" << std::endl;
    } else if(op->op == "|"){
        m_ofs << "or" << std::endl;
    } else if(op->op == "<"){
        m_ofs << "lt" << std::endl;
    } else if(op->op == ">"){
        m_ofs << "gt" << std::endl;
    } else if(op->op == "="){
        m_ofs << "eq" << std::endl;
    } else {

    }
}