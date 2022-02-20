#include "JackCompiler.h"

JackTokenizer::JackTokenizer(const std::string path)
{
    m_ifs.open(path);
    m_ofs.open(path + ".xml");
}

// print xml node head with lebel.
void JackTokenizer::printDataHead(std::string node_name)
{
    for(int i = 0; i < lebel; i++)
    {
        m_ofs << "\t";
    }

    m_ofs << "<" << node_name << ">" << std::endl;
}

// print xml node tail with lebel.
void JackTokenizer::printDataTail(std::string node_name)
{
    for(int i = 0; i < lebel; i++)
    {
        m_ofs << "\t";
    }

    m_ofs << "</" << node_name << ">" << std::endl;
}

// print xml node in one line.
void JackTokenizer::printDataLine(std::string node_name, std::string value)
{
    for(int i = 0; i < lebel; i++)
    {
        m_ofs << "\t";
    }
    m_ofs << "<" << node_name << ">";
    m_ofs << value;
    m_ofs << "</" << node_name << ">" << std::endl;;
}

bool JackTokenizer::hasMoreTokens()
{
    return false;
}

JackTokenizer::TK_TYPE JackTokenizer::tokenType()
{
    return JackTokenizer::INT_CONST;
}

void JackTokenizer::advance()
{

}