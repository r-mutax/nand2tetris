#include "JackCompiler.h"

JackAnalyzer::JackAnalyzer(const std::string path)
{
    if(std::filesystem::is_directory(path))
    {
        
    }
    else
    {
        JackTokenizer jt(path);

        jt.printDataHead("token");
        while(jt.hasMoreTokens())
        {
            switch (jt.tokenType())
            {
                case JackTokenizer::KEYWORD:
                    {
                        std::string buf = jt.keyword();
                        jt.printDataLine("keyword", buf);
                    }
                    break;
                case JackTokenizer::SYMBOL:
                    break;
                case JackTokenizer::IDENTIFIER:
                    break;
                case JackTokenizer::INT_CONST:
                    break;
                case JackTokenizer::STRING_CONST:
                    break;                
                default:
                    break;
            }

            jt.advance();
        }

        jt.printDataTail("token");
    }
}
