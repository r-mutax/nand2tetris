#include <string>
#include <filesystem>
#include <fstream>
#include <queue>

// utility library
std::string change_extension(std::string path,std::string ext);

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
        void printDataHead(std::string node_name);
        void printDataTail(std::string node_name);
        void printDataLine(std::string node_name, std::string value);

        JackTokenizer(const std::string path);
        bool hasMoreTokens(void);
        TK_TYPE tokenType(void);
        void advance(void);
        std::string keyword();
        std::string symbol();
    private:
        typedef struct _Token
        {
            JackTokenizer::TK_TYPE      type;   // token type
            std::string                 str;    // string (ex. identifer, symbol, string_const)
            long                        data;   // integer value(ex. int_const)
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
        std::ofstream   m_ofs;
        std::queue<Token>  tokens;
        int lebel = 0;
        bool isInComment = false;
        bool hasToken = true;
        void tokenizeLine(std::string buf);
        bool isKeyWord(std::string buf, int32_t pos, std::string& str);
        bool isSymbol(std::string buf, int32_t pos, std::string& str);
        void readTokens();
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
};
