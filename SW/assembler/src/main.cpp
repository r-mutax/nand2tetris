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
        SymbolTable symtable;

        // stage 1. create symbol table
        int64_t     ROMAddress = 0;
        while(parser.hasMoreCommands()){
            try{

                switch(parser.commandType()){
                    case Parser::A_COMMAND:
                    case Parser::C_COMMAND:
                        ROMAddress++;
                        break;
                    case Parser::L_COMMAND:
                        symtable.addEntry(parser.symbol(), ROMAddress);
                        break;    
                    default:
                        break;
                }
                parser.advance();
            } catch(std::out_of_range& e) {
                break;
            }
        }

        parser.reset();

        // stage 2. generate .hack file.
        while(parser.hasMoreCommands())
        {
            try{
                switch (parser.commandType()){
                    case Parser::A_COMMAND:
                        {
                            int64_t symbol;
                            if(symtable.contains(parser.symbol())){
                                symbol = symtable.getAddress(parser.symbol());
                            } else {
                                try{
                                    symbol = stoll(parser.symbol());
                                }
                                catch(std::invalid_argument& e){
                                    symtable.addVariable(parser.symbol());
                                    symbol = symtable.getAddress(parser.symbol());
                                }
                            }
                            hackio << "0" << code.symbol(std::to_string(symbol)) << "\r\n";
                        }                    
                        break;
                    case Parser::C_COMMAND:
                        {
                            std::string d = code.dest(parser.dest());
                            std::string c = code.comp(parser.comp());
                            std::string j = code.jump(parser.jump());

                            hackio << "111" << c << d << j << "\r\n";
                        }
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