#include "assembler.hpp"

SymbolTable::SymbolTable(){
    
    // register Predifined symbol
    m_table["SP"] = 0x0000;
    m_table["LCL"] = 0x0001;
    m_table["ARG"] = 0x0002;
    m_table["THIS"] = 0x0003;
    m_table["THAT"] = 0x004;
    m_table["SCREEN"] = 0x4000;
    m_table["KBD"] = 0x6000;

    m_table["R0"] = 0x0000;
    m_table["R1"] = 0x0001;
    m_table["R2"] = 0x0002;
    m_table["R3"] = 0x0003;
    m_table["R4"] = 0x0004;
    m_table["R5"] = 0x0005;
    m_table["R6"] = 0x0006;
    m_table["R7"] = 0x0007;
    m_table["R8"] = 0x0008;
    m_table["R9"] = 0x0009;
    m_table["R10"] = 0x000A;
    m_table["R11"] = 0x000B;
    m_table["R12"] = 0x000C;
    m_table["R13"] = 0x000D;
    m_table["R14"] = 0x000E;
    m_table["R15"] = 0x000F;    
}

bool SymbolTable::contains(std::string symbol){

    return m_table.count(symbol) == 1;
}

int64_t SymbolTable::getAddress(std::string symbol){
    return m_table[symbol];
}

void SymbolTable::addEntry(std::string symbol, int64_t address){
    m_table[symbol] = address;
}

void SymbolTable::addVariable(std::string symbol){
    static long var_address = 16;
    m_table[symbol] = var_address++;
}