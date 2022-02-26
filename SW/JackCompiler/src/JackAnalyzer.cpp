#include "JackCompiler.h"

JackAnalyzer::JackAnalyzer(const std::string path)
{
    if(std::filesystem::is_directory(path))
    {
        std::filesystem::directory_iterator iter(path), end;
        std::error_code err;

        for(; iter != end && !err; iter.increment(err))
        {
            const std::filesystem::directory_entry entry = *iter;

            if(entry.path().extension() == ".jack"){
                genTokenToXML(entry.path().string());
            }
        }
    }
    else
    {
        genTokenToXML(path);
    }
}

void JackAnalyzer::genTokenToXML(const std::string path)
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
