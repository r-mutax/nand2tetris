#include "assembler.hpp"

std::string Code::dest(std::string code)
{
    std::string ret;
    if(code == ""){
        ret = "000";
    } else if(code == "M"){
        ret = "001";
    } else if(code == "D"){
        ret = "010";
    } else if(code == "MD"){
        ret = "011";
    } else if(code == "A"){
        ret = "100";
    } else if(code == "AM"){
        ret = "101";
    } else if(code == "AD"){
        ret = "110";
    } else if(code == "AMD"){
        ret = "111";
    }

    return ret;
}

std::string Code::comp(std::string code)
{
    std::string a_flg;
    if(code.find('M') != std::string::npos){
        a_flg = "1";
        code[code.find('M')] = 'A';
    } else {
        a_flg = "0";
    }

    std::string ret;
    if(code == "0"){
        ret = "101010";
    } else if (code == "1"){
        ret = "111111";
    } else if (code == "-1"){
        ret = "111010";
    } else if (code == "D"){
        ret = "001100";
    } else if (code == "A"){
        ret = "110000";
    } else if (code == "!D"){
        ret = "001101";
    } else if (code == "!A"){
        ret = "110001";
    } else if (code == "-D"){
        ret = "001111";
    } else if (code == "-A"){
        ret = "110011";
    } else if (code == "D+1"){
        ret = "011111";
    } else if (code == "A+1"){
        ret = "110111";
    } else if (code == "D-1"){
        ret = "001110";
    } else if (code == "A-1"){
        ret = "110010";
    } else if (code == "D+A"){
        ret = "000010";
    } else if (code == "D-A"){
        ret = "010011";
    } else if (code == "A-D"){
        ret = "000111";
    } else if (code == "D&A"){
        ret = "000000";
    } else if (code == "D|A"){
        ret = "010101";
    }

    return a_flg + ret;
}

std::string Code::jump(std::string code){
    std::string ret;
    
    if(code == ""){
        ret = "000";
    } else if(code == "JGT"){
        ret = "001";
    } else if(code == "JEQ"){
        ret = "010";
    } else if(code == "JGE"){
        ret = "011";
    } else if(code == "JLT"){
        ret = "100";
    } else if(code == "JNE"){
        ret = "101";
    } else if(code == "JLE"){
        ret = "110";
    } else if(code == "JMP"){
        ret = "111";
    }

    return ret;
}