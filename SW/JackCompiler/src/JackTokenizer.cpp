#include "JackCompiler.h"

JackTokenizer::JackTokenizer(const std::string path)
{
    m_ifs.open(path);
    m_ofs.open(path + ".xml");

    readTokens();
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
    return hasToken;
}

JackTokenizer::TK_TYPE JackTokenizer::tokenType()
{
    auto a = tokens.front();
    return tokens.front().type;
}

void JackTokenizer::readTokens()
{
    while(tokens.size() == 0)
    {
        std::string linebuf;
        if(std::getline(m_ifs, linebuf))
        {
            tokenizeLine(linebuf);
        } else {
            hasToken = false;
            break;
        }
    }
}

void JackTokenizer::advance()
{
    tokens.pop();

    // when tokens pool is empty, read next tokens from file.
    if(tokens.size() == 0){
        readTokens();
    }
}

void JackTokenizer::tokenizeLine(std::string buf)
{
    Token   tk;
    int idx = 0;
    while(idx < buf.length())
    {
        if(isInComment){
            if(buf.compare(idx, 2, "*/") == 0){
                isInComment = false;
                idx += 2;
            } else {
                idx++;
            }
            continue;
        }

        if(isspace(buf[idx])){
            idx++;
            continue;
        }
        else if(buf.compare(idx, 2, "//") == 0){
            // ignore the rest of this line.
            return;
        }
        else if(buf.compare(idx, 2, "/*") == 0){
            isInComment = true;
            idx += 2;
        }
        else if(isKeyWord(buf, idx, tk.str))
        {
            tk.type = KEYWORD;
            tokens.push(tk);
            idx += tk.str.length();
            tk.reset();
        }

        idx++;
    }
}

bool JackTokenizer::isKeyWord(std::string buf, int32_t pos,std::string& str)
{
    bool retval = false;
    if(buf.compare(pos, 5, "class") == 0){
        str = "class";
        retval = true;
    }
    return retval;
}

std::string JackTokenizer::keyword()
{
    return tokens.front().str;
}