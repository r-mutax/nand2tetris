#include "VM.h"

CodeWriter codewriter;

bool is_vm(std::string path)
{
    int64_t pos = path.rfind(".");
    if(pos == std::string::npos){
        return false;
    }

    std::string ext = path.substr(pos + 1);
    return ext == std::string("vm");
}

std::string change_extension(std::string path,std::string ext)
{
    int64_t pos = path.rfind(".");
    if(pos == std::string::npos){
        return "";
    }

    return path.substr(0, pos) + ext;
}

void CodeGenMain(std::string vm_path)
{
    try {
        if(!is_vm(vm_path))
        {
            return;
        }

        Parser  parser(vm_path);

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
                        codewriter.writeLabel(parser.arg1());
                        break;
                    case Parser::C_GOTO:
                        break;
                    case Parser::C_IF:
                        codewriter.writeIf(parser.arg1());
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

        codewriter.setFileName(argv[1] + std::string(".asm"));
        for(; iter != end && !err; iter.increment(err))
        {
            const std::filesystem::directory_entry entry = *iter;
            CodeGenMain(entry.path().string());
        }
    }
    else
    {
        codewriter.setFileName(change_extension(argv[1], ".asm"));
        CodeGenMain(argv[1]);
    }

    return 0;
}