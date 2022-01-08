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
            try{
                switch(parser.commandType()){
                    case Parser::C_ARITHEMIC:
                        break;
                    case Parser::C_PUSH:
                        break;
                    case Parser::C_POP:
                        break;
                    case Parser::C_LABEL:
                        break;
                    case Parser::C_GOTO:
                        break;
                    case Parser::C_IF:
                        break;
                    case Parser::C_FUNCTION:
                        break;
                    case Parser::C_RETURN:
                        break;
                    case Parser::C_CALL:
                        break;
                    default:
                        break;
                }
            } catch (std::exception &e){

            }
        }

    } catch (std::exception &e){

    }

    return 0;
}