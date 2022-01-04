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
        Code    code;
        HackFileIO  hackio(argv[1]);

        while(parser.hasMoreCommands())
        {
            try{
                switch (parser.commandType())
                {
                case Parser::A_COMMAND:
                    hackio << "0" << code.symbol(parser.symbol()) << "\r\n";
                    break;
                case Parser::C_COMMAND:
                    {
                        std::string d = code.dest(parser.dest());
                        std::string c = code.comp(parser.comp());
                        std::string j = code.jump(parser.jump());

                        hackio << "111" << c << d << j << "\r\n";
                    }
                    // std::cout << parser.getCommandsString() << std::endl;

                    // std::cout << "dest : " << code.dest(parser.dest()) << std::endl;
                    // std::cout << "comp : " << code.comp(parser.comp()) << std::endl;
                    // std::cout << "jump : " << code.jump(parser.jump()) << std::endl;
                    break;
                case Parser::L_COMMAND:
                    break;
                default:
                    break;
                }

                parser.advance();
            } catch(std::out_of_range& e) {
                break;
            }
        }
    } catch(std::invalid_argument& e){
        std::cerr << e.what();
        return -1;
    } catch(std::out_of_range& e){
        std::cerr << "[error] .asm file doesn't has Command.\n";
    }

    return 0;    
}