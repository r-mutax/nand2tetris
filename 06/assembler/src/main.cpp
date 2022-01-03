#include "assembler.hpp"


int main(int argc, char **argv)
{
    if(argc != 2)
    {
        fprintf(stderr, "[error] invalid arguments num.\n");
        return -1;
    }
    
    try {
        Parser  parser(argv[1]);

        while(parser.hasMoreCommands())
        {
            std::cout << parser.getCommandsString() << std::endl;
            parser.advance();
        }
    }
    catch(std::invalid_argument& e){
        fprintf(stderr, "[error] .asm file is not existed.\n");
        return -1;
    }

    return 0;    
}