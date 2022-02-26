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
    value = std::regex_replace(value, std::regex("&"), "&amp;");
    value = std::regex_replace(value, std::regex("<"), "&lt;");
    value = std::regex_replace(value, std::regex(">"), "&gt;");

    for(int i = 0; i < lebel; i++)
    {
        m_ofs << "\t";
    }
    m_ofs << "<" << node_name << "> ";
    m_ofs << value;
    m_ofs << " </" << node_name << ">" << std::endl;;
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

        if(buf.compare(idx, 2, "//") == 0){
            // ignore the rest of this line.
            return;
        }

        if(buf.compare(idx, 2, "/*") == 0){
            isInComment = true;
            idx += 2;
            continue;
        }

        if(isKeyWord(buf, idx, tk.str))
        {
            tk.type = KEYWORD;
            tokens.push(tk);
            idx += tk.str.length();
            tk.reset();
            continue;
        }

        if(isSymbol(buf, idx, tk.str))
        {
            tk.type = SYMBOL;
            tokens.push(tk);
            idx += tk.str.length();
            tk.reset();
            continue;
        }

        if(isdigit(buf[idx]))
        {
            std::string digitbuf;
            do {
                if(isdigit(buf[idx])){
                    digitbuf += buf[idx];
                    idx++;
                } else if(isalpha(buf[idx])){
                    // [error]
                    // identifier begin with number.
                } else {
                    break;
                }
            } while(1);
            tk.type = INT_CONST;
            tk.data = std::stol(digitbuf);
            tokens.push(tk);
            tk.reset();
            continue;
        }

        if(buf[idx] == '"')
        {
            // string Constant
            idx++;
            while(buf[idx] != '"')
            {
                tk.str += buf[idx];
                idx++;
            }

            tk.type = STRING_CONST;
            tokens.push(tk);
            tk.reset();
            idx++;
            continue;
        }

        if(isident1(buf[idx]))
        {
            do {
                tk.str += buf[idx];
                idx++;
            } while(isident1(buf[idx]) || isdigit(buf[idx]));

            tk.type = IDENTIFIER;
            tokens.push(tk);
            tk.reset();
            continue;
        }

        idx++;
    }
}

bool JackTokenizer::isKeyWord(std::string buf, int32_t pos,std::string& str)
{
    auto check = [buf, pos, &str](std::string keyword){
        if(buf.compare(pos, keyword.length(), keyword) == 0){
            str = keyword;
            return true;
        } else {
            return false;
        }
    };

    if (check("class")) return true;
    if (check("constructor")) return true;
    if (check("function")) return true;
    if (check("method")) return true;
    if (check("field")) return true;
    if (check("static")) return true;
    if (check("var")) return true;
    if (check("int")) return true;
    if (check("char")) return true;
    if (check("boolean")) return true;
    if (check("void")) return true;
    if (check("true")) return true;
    if (check("false")) return true;
    if (check("null")) return true;
    if (check("this")) return true;
    if (check("let")) return true;
    if (check("do")) return true;
    if (check("if")) return true;
    if (check("else")) return true;
    if (check("while")) return true;
    if (check("return")) return true;
    
    return false;
}

bool JackTokenizer::isSymbol(std::string buf, int32_t pos, std::string& str)
{
    if(std::string("{}()[].,;+-*/&|<>=~").find(buf[pos]) != std::string::npos)
    {
        str = buf[pos];
        return true;
    }
    return false;
}

bool JackTokenizer::isident1(char c)
{
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

std::string JackTokenizer::keyword()
{
    return tokens.front().str;
}

std::string JackTokenizer::symbol()
{
    return tokens.front().str;
}

int32_t JackTokenizer::intVal()
{
    return tokens.front().data;
}

std::string JackTokenizer::stringVal()
{
    return tokens.front().str;
}

std::string JackTokenizer::identifier()
{
    return tokens.front().str;
}