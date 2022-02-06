#include "VM.h"

bool is_vm(std::string path)
{
    std::string ext = path.substr(path.length() - 3);
    return ext == std::string(".vm");
}

void CodeGenMain(std::string vm_path)
{
    try {
        if(!is_vm(vm_path))
        {
            return;
        }

        Parser  parser(vm_path);
        CodeWriter codewriter((vm_path + std::string(".asm")));

        while(parser.hasMoreCommands())
        {
            try{
                switch(parser.commandType()){
                    case Parser::C_ARITHEMIC:
                        codewriter.writeArithmetic(parser.arg1());
                        break;
                    case Parser::C_PUSH:
                        codewriter.writePushPop(Parser::C_PUSH, parser.arg1(), parser.arg2());
                        break;
                    case Parser::C_POP:
                        codewriter.writePushPop(Parser::C_POP, parser.arg1(), parser.arg2());
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
            parser.advance();
        }

    } catch (std::exception &e){

    }
}

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        std::cerr << "[error] invalid arguments num.\n";
    }

    if(std::filesystem::is_directory(argv[1]))
    {
        std::filesystem::directory_iterator iter(argv[1]), end;
        std::error_code err;
        for(; iter != end && !err; iter.increment(err))
        {
            const std::filesystem::directory_entry entry = *iter;
            CodeGenMain(entry.path().string());
        }
    }
    else
    {
        CodeGenMain(argv[1]);
    }



    return 0;
}