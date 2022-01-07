#include "VM.h"

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        std::cerr << "[error] invalid arguments num.\n";
    }

    try {
        Parser  parser(argv[1]);

        while(parser.hasMoreCommands())
        {
            
        }

    } catch (std::exception &e){

    }

    return 0;
}