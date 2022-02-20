#include "JackCompiler.h"

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        fprintf(stderr, "[error] invalid arguments num.");
    }

    try
    {
        JackAnalyzer ja(argv[1]);
    }
    catch (...)
    {

    }
    return 0;
}
