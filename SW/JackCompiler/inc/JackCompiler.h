#pragma region

#include <string>
#include <filesystem>
#include <fstream>
#include <queue>
#include <regex>


// utility library
std::string change_extension(std::string path,std::string ext);

class GenXMLFile 
{
    private:
        std::ofstream   m_ofs;
        int32_t lebel = 0;
    public:
        GenXMLFile(){};
        ~GenXMLFile();
        void openXMLFile(const std::string& path);
        void printDataHead(std::string node_name);
        void printDataTail(std::string node_name);
        void printDataLine(std::string node_name, std::string value);
};

class JackTokenizer
{
    public:
        enum TK_TYPE {
            KEYWORD = 0
            , SYMBOL
            , IDENTIFIER
            , INT_CONST
            , STRING_CONST
            , NO_TYPE 
        };
        void printDataHead(std::string node_name) { m_xml.printDataHead(node_name); };
        void printDataTail(std::string node_name) { m_xml.printDataTail(node_name); };
        void printDataLine(std::string node_name, std::string value) { m_xml.printDataLine(node_name, value); };

        JackTokenizer(const std::string path);
        bool hasMoreTokens(void);
        TK_TYPE tokenType(void);
        void advance(void);
        std::string keyword();
        std::string symbol();
        int32_t intVal();
        std::string stringVal();
        std::string identifier();
    private:
        typedef struct _Token
        {
            JackTokenizer::TK_TYPE      type;   // token type
            std::string                 str;    // string (ex. identifer, symbol, string_const)
            int32_t                     data;   // integer value(ex. int_const)
            void reset(){
                type = JackTokenizer::TK_TYPE::NO_TYPE;
                str = "";
                data = 0;
            };

            _Token()
            {
                reset();
            };
        } Token;

        std::ifstream   m_ifs;
        GenXMLFile      m_xml;
        std::queue<Token>  tokens;
        int lebel = 0;
        bool isInComment = false;
        bool hasToken = true;
        void tokenizeLine(std::string buf);
        bool isKeyWord(std::string buf, int32_t pos, std::string& str);
        bool isSymbol(std::string buf, int32_t pos, std::string& str);
        void readTokens();
        bool isident1(char c);
};

// class CompilationEngine
// {
//     private:
//         JackTokenizer*      jt;
//         std::ofstream       m_ofs;
//         int                 lebel;
//         void compileClass();
//         void compileClassVarDec();



//     public:
//         CompilationEngine(const std::string path);
//         ~CompilationEngine(void);
//         void compile(void);
// };

class JackAnalyzer
{
    private:
    public:
        JackAnalyzer(const std::string path);
        void genTokenToXML(const std::string path);
};
