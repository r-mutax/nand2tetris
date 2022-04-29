#include "JackCompiler.h"

JackTokenizer::JackTokenizer(void)
{
}

JackTokenizer::JackTokenizer(const std::string path)
{
    Open(path);
}

void JackTokenizer::Open(const std::string path)
{
    m_ifs.open(path);

    size_t  dot_pos = path.rfind(".");
    std::string xml_path = path.substr(0, dot_pos);

    //m_xml.openXMLFile(xml_path + "T.xml");

    readTokens();
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
            linebuf += " ";
            tokenizeLine(linebuf);
        } else {
            hasToken = false;
            break;
        }
    }
}

void JackTokenizer::advance()
{
    tokens.pop_front();

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
            tokens.push_back(tk);
            idx += tk.str.length();
            tk.reset();
            continue;
        }

        if(isSymbol(buf, idx, tk.str))
        {
            tk.type = SYMBOL;
            tokens.push_back(tk);
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
            tokens.push_back(tk);
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
            tokens.push_back(tk);
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
            tokens.push_back(tk);
            tk.reset();
            continue;
        }

        idx++;
    }
}

bool JackTokenizer::isKeyWord(std::string buf, int32_t pos,std::string& str)
{
    auto check = [buf, pos, &str](std::string keyword){
        int32_t len = keyword.length();
        if(buf.compare(pos, len, keyword) == 0){   
            char c = buf[pos + len];         
            if(!(('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || c == '_')){
                str = keyword;
                return true;
            } else {
                return false;
            }
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

void JackTokenizer::stash_front_token()
{
    stash_token = tokens.front();
    tokens.pop_front();
}

void JackTokenizer::stash_pop_front_token()
{
    tokens.push_front(stash_token);
}

// get next_tokentype.
JackTokenizer::TK_TYPE JackTokenizer::next_tokenType()
{
    stash_front_token();

    TK_TYPE tk_type = tokens.front().type;

    stash_pop_front_token();

    return tk_type;
}

std::string JackTokenizer::next_keyword()
{
    std::string ret_keyword;

    stash_front_token();

    ret_keyword = tokens.front().str;

    stash_pop_front_token();

    return ret_keyword;
}

std::string JackTokenizer::next_symbol()
{
    std::string ret_symbol;

    stash_front_token();

    ret_symbol = tokens.front().str;

    stash_pop_front_token();

    return ret_symbol;
}

int32_t JackTokenizer::next_intVal()
{
    int32_t retval = 0;
    stash_front_token();

    retval = tokens.front().data;

    stash_pop_front_token();

    return retval;
}

std::string JackTokenizer::next_stringVal()
{
    std::string ret_stringVal;

    stash_front_token();

    ret_stringVal = tokens.front().str;

    stash_pop_front_token();

    return ret_stringVal;
}

std::string JackTokenizer::next_identifier()
{
    std::string ret_identifier;

    stash_front_token();

    ret_identifier = tokens.front().str;

    stash_pop_front_token();

    return ret_identifier;
}

bool JackTokenizer::expect_token(TK_TYPE type, std::string str)
{
    if(type != tokens.front().type
        || str != tokens.front().str)
    {
        return false;    
    }

    return true;
}

bool JackTokenizer::expect_token(TK_TYPE type)
{
    if(type != tokens.front().type)
    {
        return false;    
    }
    
    return true;
}
