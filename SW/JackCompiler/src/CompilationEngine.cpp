#include "JackCompiler.h"
#if 0

CompilationEngine::CompilationEngine(const std::string path)
{
    jt = new JackTokenizer(path);
    m_ofs.open(change_extension(path, "xml"));
    lebel = 0;
}

CompilationEngine::~CompilationEngine()
{
    delete jt;
}


// exec compile file.
void CompilationEngine::compile(void)
{
    compileClass();
}

void CompilationEngine::compileClass()
{
    printDataHead("class");
    lebel++;

    lebel--;
    printDataTail("class");
}
#endif
