#include "assembler.hpp"


int main(int argc, char **argv)
{
    if(argc != 2)
    {
        std::cerr << "[error] invalid arguments num.\n";
        return -1;
    }
    
    try {
        Parser  parser(argv[1]);

        while(parser.hasMoreCommands())
        {
            try{
                std::cout << parser.getCommandsString() << std::endl;
                parser.advance();
            } catch(std::out_of_range& e) {
                break;
            }
        }
    } catch(std::invalid_argument& e){
        std::cerr << "[error] .asm file is not existed.\n";
        return -1;
    } catch(std::out_of_range& e){
        std::cerr << "[error] .asm file doesn't has Command.\n";
    }

    return 0;    
}