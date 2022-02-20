#include <string>
#include <filesystem>
#include <fstream>

// utility library
std::string change_extension(std::string path,std::string ext);

class JackTokenizer
{
    private:
        std::ifstream   m_ifs;
        std::ofstream   m_ofs;
        int lebel = 0;
    public:
        enum TK_TYPE {
            KEYWORD = 0
            , SYMBOL
            , IDENTIFIER
            , INT_CONST
            , STRING_CONST
        };
        void printDataHead(std::string node_name);
        void printDataTail(std::string node_name);
        void printDataLine(std::string node_name, std::string value);

        JackTokenizer(const std::string path);
        bool hasMoreTokens(void);
        TK_TYPE tokenType(void);
        void advance(void);
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
