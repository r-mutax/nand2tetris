#include "JackCompiler.h"

JackAnalyzer::JackAnalyzer(const std::string path)
{
    if(std::filesystem::is_directory(path))
    {
        
    }
    else
    {
        JackTokenizer jt(path);

        jt.printDataHead("tokens");
        while(jt.hasMoreTokens())
        {
            switch (jt.tokenType())
            {
                case JackTokenizer::KEYWORD:
                {
                    std::string buf = jt.keyword();
                    jt.printDataLine("keyword", buf);
                    break;
                }
                case JackTokenizer::SYMBOL:
                {
                    std::string buf = jt.symbol();
                    jt.printDataLine("symbol", buf);
                    break;
                }
                case JackTokenizer::IDENTIFIER:
                {
                    jt.printDataLine("identifier", jt.identifier());
                    break;
                }
                case JackTokenizer::INT_CONST:
                {
                    int32_t buf = jt.intVal();
                    jt.printDataLine("integerConstant", std::to_string(buf));
                    break;
                }
                case JackTokenizer::STRING_CONST:
                {
                    jt.printDataLine("stringConstant", jt.stringVal());
                    break;
                }
                default:
                    break;
            }

            jt.advance();
        }

        jt.printDataTail("tokens");
    }
}
