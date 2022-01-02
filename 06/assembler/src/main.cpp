#include "assembler.hpp"
#include <stdio.h>

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        fprintf(stderr, "[error] invalid arguments num.\n");
        return -1;
    }
    
    return 0;    
}