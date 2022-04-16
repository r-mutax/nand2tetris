#include "JackCompiler.h"

GenXMLFile::~GenXMLFile()
{
    if( m_ofs.is_open() )
        m_ofs.close();
}

void GenXMLFile::openXMLFile(const std::string& path)
{
    m_ofs.open(path);
}

// print xml node head with lebel.
void GenXMLFile::printDataHead(std::string node_name)
{
    for(int i = 0; i < lebel; i++)
    {
        m_ofs << "  ";
    }

    m_ofs << "<" << node_name << ">" << std::endl;

    lebel++;
}

// print xml node tail with lebel.
void GenXMLFile::printDataTail(std::string node_name)
{
    lebel--;
    for(int i = 0; i < lebel; i++)
    {
        m_ofs << "  ";
    }

    m_ofs << "</" << node_name << ">" << std::endl;
}

// print xml node in one line.
void GenXMLFile::printDataLine(std::string node_name, std::string value)
{
    value = std::regex_replace(value, std::regex("&"), "&amp;");
    value = std::regex_replace(value, std::regex("<"), "&lt;");
    value = std::regex_replace(value, std::regex(">"), "&gt;");

    for(int i = 0; i < lebel; i++)
    {
        m_ofs << "  ";
    }
    m_ofs << "<" << node_name << "> ";
    m_ofs << value;
    m_ofs << " </" << node_name << ">" << std::endl;;
}