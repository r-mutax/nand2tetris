#include "JackCompiler.h"

void SymbolTable::init(){

    class_scope.clear();
    func_scope.clear(); 

    cur_scope = &class_scope;  
}

void SymbolTable::startSubroutine(){
    func_scope.clear();

    // change scope
    cur_scope = &func_scope;
}

void SymbolTable::define(std::string name, std::string type, SYMBOL_KIND kind)
{
    symbol_entity entity;
    entity.type = type;
    entity.kind = kind;
    entity.index = varCount(kind);

    (*cur_scope)[name] = entity;
}

int32_t SymbolTable::varCount(SYMBOL_KIND kind){
    int32_t ret = 0;
    for(auto&& it : *cur_scope){
        if(it.second.kind == kind) ret++;
    }

    return ret;
}

SymbolTable::symbol_entity  SymbolTable::getSymbolEntity(std::string name)
{
    if(func_scope.count(name) != 0){
        // find in func_scope
        return func_scope[name];
    }

    if(class_scope.count(name) != 0){
        // find in class_scope
        return class_scope[name];
    }

    // This project compile .jack file not include error,
    // then code flow should not reach here.
    symbol_entity dummy;
    return dummy;
}

SymbolTable::SYMBOL_KIND SymbolTable::kindOf(std::string name)
{
    symbol_entity ent = getSymbolEntity(name);

    return ent.kind;
}

std::string SymbolTable::typeOf(std::string name)
{
    symbol_entity ent = getSymbolEntity(name);

    return ent.type;
}

int32_t SymbolTable::indexOf(std::string name)
{
    symbol_entity ent = getSymbolEntity(name);

    return ent.index;
}

bool SymbolTable::is_defined(std::string name){
    return func_scope.count(name) == 1 || class_scope.count(name) == 1;
}