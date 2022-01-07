#include "VM.h"

Parser::Parser(std::string vm_file_path)
{
    m_ifs.open(vm_file_path);
}

bool Parser::hasMoreCommands()
{
    return true;
}