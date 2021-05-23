#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#define TOFILE
using namespace std;

char buf[100000];
char* nc = buf;
namespace Lexical
{

//{"[A-Za-z_][A-Za-z0-9_]*", "IDENFR"},
char* isIdentify()
{
    char* r = nc;
    if (isalpha(*r) or *r == '_')
    {
        while (isalnum(*r) or *r == '_')
            r++;
    }
    return r;
}
//{"[1-9][\\d]*", "INTCON"},
char* isInt()
{
    char* r = nc;
    while (isdigit(*r))
        r++;
    return r;
}

//{"'\\\\?.'", "CHARCON"},
char* isChar()
{
    char* r = nc;
    if (*r == '\'' and *(r + 2) == '\'')
    {
        return r + 3;
    }
    return r;
}

//{"\".*?\"", "STRCON"},
char* isStr()
{
    char* r = nc;
    if (*r == '\"')
    {
        r++;
        while (*r != '\"')
            r++;
    }

    // "fda"
    return r;
}

vector<pair<string, string>> keyword{
    {"const", "CONSTTK"},
    {"int", "INTTK"},
    {"char", "CHARTK"},
    {"void", "VOIDTK"},
    {"main", "MAINTK"},
    {"if", "IFTK"},
    {"else", "ELSETK"},
    {"do", "DOTK"},
    {"while", "WHILETK"},
    {"for", "FORTK"},
    {"scanf", "SCANFTK"},
    {"printf", "PRINTFTK"},
    {"return", "RETURNTK"},
};

vector<pair<string, string>> identify{
    {"+", "PLUS"},
    {"-", "MINU"},
    {"*", "MULT"},
    {"/", "DIV"},
    {"<", "LSS"},
    {"<=", "LEQ"},
    {">", "GRE"},
    {">=", "GEQ"},
    {"==", "EQL"},
    {"!=", "NEQ"},
    {"=", "ASSIGN"},
    {";", "SEMICN"},
    {",", "COMMA"},
    {"(", "LPARENT"},
    {")", "RPARENT"},
    {"[", "LBRACK"},
    {"]", "RBRACK"},
    {"{", "LBRACE"},
    {"}", "RBRACE"},
};

ofstream os;
vector<string> tokenbuf;

string nextToken();
void init()
{
    using namespace Lexical;
    auto pcmp = [](const pair<string, string>& a, const pair<string, string>& b) -> bool {
        if (a.first.size() != b.first.size())
            return a.first.size() < b.first.size();
        return a < b;
    };

    sort(keyword.rbegin(), keyword.rend(), pcmp);
    sort(identify.rbegin(), identify.rend(), pcmp);

    ifstream infile("testfile.txt");
    char* input = buf;
    string tmp;
    os.open("output.txt");

#ifdef TOFILE
    cout.rdbuf(os.rdbuf());
#endif
    while (getline(infile, tmp))
    {
        strcpy(input, tmp.data());
        input += tmp.size();
    }
    string s = nextToken();
    while (not s.empty())
    {
        tokenbuf.push_back(move(s));
        s = nextToken();
    }
}
vector<string> outarr;
string nextToken()
{
    while (*nc != 0 and isspace(*nc))
    {
        nc++;
    }
    if (*nc == 0)
        return "";
    char* r = isIdentify();
    if (r > nc)
    { //nc - r
        bool iskeyword = false;
        for (auto&& i : keyword)
        {
            if (string(nc, r) == i.first)
            {
                iskeyword = true;
                outarr.push_back(i.second + ' ');
                // cout << i.second << ' ';
                while (nc != r)
                    outarr.back().push_back(*nc++);
                // cout << *nc++;
                // cout << endl;
                outarr.back().push_back('\n');
                return i.second;
            }
        }
        if (not iskeyword)
        {
            outarr.push_back("IDENFR ");
            // cout << "IDENFR" << ' ';
            while (nc != r)
                outarr.back().push_back(*nc++);
            // cout << *nc++;
            outarr.back().push_back('\n');
            // cout << endl;
        }
        return "IDENFR";
    }
    r = isInt();
    if (r > nc)
    {
        outarr.push_back("INTCON ");
        // cout << "INTCON" << ' ';
        while (nc != r)
            outarr.back().push_back(*nc++);
        // cout << *nc++;
        // cout << endl;
        outarr.back().push_back('\n');
        return "INTCON";
    }
    r = isChar();
    if (r > nc)
    {
        nc++;
        outarr.push_back("CHARCON ");
        // cout << "CHARCON" << ' ';
        while (nc + 1 != r)
            outarr.back().push_back(*nc++);
        // cout << *nc++;
        // cout << endl;
        outarr.back().push_back('\n');
        nc = r;
        return "CHARCON";
    }
    r = isStr();
    if (r > nc)
    {
        nc++;
        outarr.push_back("STRCON ");
        // cout << "STRCON" << ' ';
        while (nc != r)
            outarr.back().push_back(*nc++);
        // cout << *nc++;
        // cout << endl;
        outarr.back().push_back(10);
        nc++;
        return "STRCON";
    }
    for (auto&& i : identify)
    {
        if (strncmp(nc, i.first.data(), i.first.size()) == 0)
        {
            outarr.push_back(i.second + ' ' + i.first + "\n");
            // cout << i.second << ' ';
            // cout << i.first << endl;
            nc += i.first.size();
            return i.second;
        }
    }
    outarr.push_back("");
    return "";
}
}

; // namespace Lexical

namespace Parse
{

string id("IDENFR");

unordered_map<string, string> keyword{
    {"const", "CONSTTK"},
    {"int", "INTTK"},
    {"char", "CHARTK"},
    {"void", "VOIDTK"},
    {"main", "MAINTK"},
    {"if", "IFTK"},
    {"else", "ELSETK"},
    {"do", "DOTK"},
    {"while", "WHILETK"},
    {"for", "FORTK"},
    {"scanf", "SCANFTK"},
    {"printf", "PRINTFTK"},
    {"return", "RETURNTK"},
};

unordered_map<string, string> delimiter{
    {"+", "PLUS"},
    {"-", "MINU"},
    {"*", "MULT"},
    {"/", "DIV"},
    {"<", "LSS"},
    {"<=", "LEQ"},
    {">", "GRE"},
    {">=", "GEQ"},
    {"==", "EQL"},
    {"!=", "NEQ"},
    {"=", "ASSIGN"},
    {";", "SEMICN"},
    {",", "COMMA"},
    {"(", "LPARENT"},
    {")", "RPARENT"},
    {"[", "LBRACK"},
    {"]", "RBRACK"},
    {"{", "LBRACE"},
    {"}", "RBRACE"},
};

unordered_map<string, bool> hasVar;

using namespace Lexical;

vector<pair<size_t, string>> parsearr;

//tokenbuf   outarr
string word;
//pre Word

string getId(size_t n)
{
    string t;
    auto pos = outarr[n].find(' ');
    return outarr[n].substr(pos + 1);
}

size_t tposi = 0;
//nextWord
void nw()
{
    if (tposi < tokenbuf.size())
        word = Lexical::tokenbuf[tposi];
    else
        word = "";
    tposi++;
    // cout << Lexical::outarr[oposi++];
}

//<类型标识符>      ::=  int | char
bool type()
{
    if (word == keyword["int"] or word == keyword["char"])
    {
        nw();
        return true;
    }
    return false;
}

//<加法运算符> ::= +｜-
bool addOp()
{
    if (word == delimiter["+"] or word == delimiter["-"])
    {
        nw();
        return true;
    }
    return false;
}

//<关系运算符>  ::=  <｜<=｜>｜>=｜!=｜==
bool relationOp()
{
    if (word == delimiter["<"] or word == delimiter["<="] or word == delimiter[">"] or word == delimiter[">="] or word == delimiter["!="] or word == delimiter["=="])
    {
        nw();
        return true;
    }
    return false;
}

//<乘法运算符>  ::= *｜/
bool mulOp()
{
    if (word == delimiter["*"] or word == delimiter["/"])
    {
        nw();
        return true;
    }
    return false;
}

//NO OUPUT ^
bool expression();
bool returnFunctionCall();
bool constDeclare();
bool statement();
bool parameterList();
bool compoundStatement();
bool condition();
bool statementList();

bool unsignedI()
{
    if (word == "INTCON")
    {
        nw();
        parsearr.push_back({tposi, "<无符号整数>"});
        return true;
    }
    return false;
}

bool integer()
{
    addOp();
    if (unsignedI())
    {
        parsearr.push_back({tposi, "<整数>"});
        return true;
    }
    return false;
}

//<无返回值函数定义>  ::= void<标识符>'('<参数表>')''{'<复合语句>'}'
bool voidFunctionDefine()
{
    if (word == keyword["void"])
    {
        nw();
        if (word == id)
        {
            nw();
            if (word == delimiter["("])
            {
                nw();
                if (parameterList())
                {
                    if (word == delimiter[")"])
                    {
                        nw();
                        if (word == delimiter["{"])
                        {
                            nw();
                            if (compoundStatement())
                            {
                                if (word == delimiter["}"])
                                {
                                    nw();
                                    parsearr.push_back({tposi, "<无返回值函数定义>"});
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

//<返回语句>   ::=  return['('<表达式>')']
bool returnStatement()
{
    if (word == keyword["return"])
    {
        nw();
        if (word == delimiter["("])
        {
            nw();
            if (expression())
            {
                if (word == delimiter[")"])
                {
                    nw();
                    parsearr.push_back({tposi, "<返回语句>"});
                    return true;
                }
            }
        }
        parsearr.push_back({tposi, "<返回语句>"});
        return true;
    }
    return false;
}

//<写语句>    ::= printf '(' <字符串>,<表达式> ')'| printf '('<字符串> ')'| printf '('<表达式>')'
bool printfStatement()
{
    if (word == keyword["printf"])
    {
        nw();
        if (word == delimiter["("])
        {
            nw();
            if (word == "STRCON")
            {
                nw();
                parsearr.push_back({tposi, "<字符串>"});
                if (word == delimiter[","])
                {
                    nw();
                    if (expression())
                    {
                        if (word == delimiter[")"])
                        {
                            nw();
                            parsearr.push_back({tposi, "<写语句>"});
                            return true;
                        }
                    }
                }
                else if (word == delimiter[")"])
                {
                    nw();
                    parsearr.push_back({tposi, "<写语句>"});
                    return true;
                }
            }
            else if (expression())
            {
                if (word == delimiter[")"])
                {
                    nw();
                    parsearr.push_back({tposi, "<写语句>"});

                    return true;
                }
            }
            else
                return false;
        }
    }
    return false;
}
//<读语句>    ::=  scanf '('<标识符>{,<标识符>}')'
bool scanfStatement()
{
    if (word == keyword["scanf"])
    {
        nw();
        if (word == delimiter["("])
        {
            nw();
            if (word == id)
            {
                nw();
                bool flag;
                do
                {
                    flag = false;
                    if (word == delimiter[","])
                    {
                        nw();
                        if (word == id)
                        {
                            nw();
                            flag = true;
                        }
                    }
                } while (flag);
                if (word == delimiter[")"])
                {
                    nw();
                    parsearr.push_back({tposi, "<读语句>"});
                    return true;
                }
            }
        }
    }
    return false;
}

//<值参数表>   ::= <表达式>{,<表达式>}｜<空>
bool valParameterList()
{
    if (expression())
    {
        bool flag;
        do
        {
            flag = false;
            if (word == delimiter[","])
            {
                nw();
                if (expression())
                    flag = true;
                else
                    return false;
            }
        } while (flag);
    }
    parsearr.push_back({tposi, "<值参数表>"});
    return true;
}

//<步长>::= <无符号整数>
bool stepLength()
{
    if (unsignedI())
    {
        parsearr.push_back({tposi, "<步长>"});
        return true;
    }
    return false;
}


/*
fun()
a
a[1]
()
1
'a'
*/
//<因子>    ::= <标识符>｜<标识符>'['<表达式>']'|
// '('<表达式>')'｜<整数>|<字符>｜<有返回值函数调用语句>
// (+i / 3)
bool factor()
{
    if (word == id and tokenbuf[tposi] == delimiter["("])
    {
        returnFunctionCall();
        parsearr.push_back({tposi, "<因子>"});
        return true;
    }
    else if (word == id)
    {
        nw();
        if (word == delimiter["["])
        {
            nw();
            if (expression())
            {
                if (word == delimiter["]"])
                {
                    nw();
                    parsearr.push_back({tposi, "<因子>"});

                    return true;
                }
                return false;
            }
        }
        parsearr.push_back({tposi, "<因子>"});

        return true;
    }
    else if (word == delimiter["("])
    {
        nw();
        if (expression())
        {
            if (word == delimiter[")"])
            {
                nw();
                parsearr.push_back({tposi, "<因子>"});
                return true;
            }
        }
    }
    else if (integer())
    {
        parsearr.push_back({tposi, "<因子>"});
        return true;
    }
    else if (word == "CHARCON")
    {
        nw();
        parsearr.push_back({tposi, "<因子>"});
        return true;
    }
    return false;
}

//<项>     ::= <因子>{<乘法运算符><因子>}
//             (+i / 3) * 3
bool item()
{
    if (factor())
    {
        if (mulOp())
        {
            if (factor())
            {
                parsearr.push_back({tposi, "<项>"});
                return true;
            }
            return false;
        }
        parsearr.push_back({tposi, "<项>"});
        return true;
    }
    return false;
}

//<表达式>    ::= ［＋｜－］<项>{<加法运算符><项>}   //[+ | -]只作用于第一个<项>
// i - (+i / 3) * 3
bool expression()
{
    int cnt = 0;
    addOp();
    if (item())
    {
        cnt++;
    }
    bool flag;
    do
    {
        flag = false;
        if (addOp() and item())
        {
            ++cnt;
            flag = true;
        }
    } while (flag);
    if (cnt > 0)
    {
        parsearr.push_back({tposi, "<表达式>"});
    }
    return cnt > 0;
}

//<循环语句>   ::=  while '('<条件>')'<语句> | do<语句>while '('<条件>')' | for'('<标识符>＝<表达式>;<条件>;<标识符>＝<标识符>(+|-)<步长>')'<语句>
bool loopStatement()
{
    if (word == keyword["while"])
    {
        nw();
        if (word == delimiter["("])
        {
            nw();
            if (condition())
            {
                if (word == delimiter[")"])
                {
                    nw();
                    if (statement())
                    {
                        parsearr.push_back({tposi, "<循环语句>"});
                        return true;
                    }
                }
            }
        }
    }
    else if (word == keyword["do"])
    {
        nw();
        if (statement())
        {
            if (word == keyword["while"])
            {
                nw();
                if (word == delimiter["("])
                {
                    nw();
                    if (condition())
                    {
                        if (word == delimiter[")"])
                        {
                            nw();
                            parsearr.push_back({tposi, "<循环语句>"});
                            return true;
                        }
                    }
                }
            }
        }
    }
    // for'('<标识符>＝<表达式>;<条件>;<标识符>＝<标识符>(+|-)<步长>')'<语句>
    else if (word == keyword["for"])
    {
        nw();
        if (word == delimiter["("])
        {
            nw();
            if (word == id)
            {
                nw();
                if (word == delimiter["="])
                {
                    nw();
                    if (expression())
                    {
                        if (word == delimiter[";"])
                        {
                            nw();
                            if (condition())
                            {
                                if (word == delimiter[";"])
                                {
                                    nw();
                                    if (word == id)
                                    {
                                        nw();
                                        if (word == delimiter["="])
                                        {
                                            nw();
                                            if (word == id)
                                            {
                                                nw();
                                                if (addOp())
                                                {
                                                    if (stepLength())
                                                    {
                                                        // for'('<标识符>＝<表达式>;<条件>;<标识符>＝<标识符>(+|-)<步长>')'<语句>
                                                        if (word == delimiter[")"])
                                                        {
                                                            nw();
                                                            if (statement())
                                                            {
                                                                parsearr.push_back({tposi, "<循环语句>"});
                                                                return true;
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}
//<无返回值函数调用语句> ::= <标识符>'('<值参数表>')'
bool voidFunctionCall()
{
    if (word == id)
    {
        nw();
        if (word == delimiter["("])
        {
            nw();
            if (valParameterList())
            {
                if (word == delimiter[")"])
                {
                    nw();
                    parsearr.push_back({tposi, "<无返回值函数调用语句>"});
                    return true;
                }
            }
        }
    }
    return false;
}

//<有返回值函数调用语句> ::= <标识符>'('<值参数表>')'
bool returnFunctionCall()
{
    if (word == id)
    {
        nw();
        if (word == delimiter["("])
        {
            nw();
            if (valParameterList())
            {
                if (word == delimiter[")"])
                {
                    nw();
                    parsearr.push_back({tposi, "<有返回值函数调用语句>"});
                    return true;
                }
            }
        }
    }
    return false;
}

//<赋值语句>   ::=  <标识符>＝<表达式>|<标识符>'['<表达式>']'=<表达式>
bool assignStatement()
{
    if (word == id)
    {
        nw();
        if (word == delimiter["="])
        {
            nw();
            if (expression())
            {
                parsearr.push_back({tposi, "<赋值语句>"});
                return true;
            }
        }
        else if (word == delimiter["["])
        {
            nw();
            if (expression())
            {
                if (word == delimiter["]"])
                {
                    nw();
                    if (word == delimiter["="])
                    {
                        nw();
                        if (expression())
                        {
                            parsearr.push_back({tposi, "<赋值语句>"});
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

//<条件>    ::=  <表达式><关系运算符><表达式> //整型表达式之间才能进行关系运算
//    ｜<表达式>    //表达式为整型，其值为0条件为假，值不为0时条件为真
bool condition()
{
    if (expression())
    {
        if (relationOp())
        {
            if (expression())
            {
                parsearr.push_back({tposi, "<条件>"});
                return true;
            }
            else
                return false;
        }
        parsearr.push_back({tposi, "<条件>"});
        return true;
    }
    return false;
}

// <条件语句>  ::= if '('<条件>')'<语句>［else<语句>］
bool ifStatement()
{
    if (word == keyword["if"])
    {
        nw();
        if (word == delimiter["("])
        {
            nw();
            if (condition())
            {
                if (word == delimiter[")"])
                {
                    nw();
                    if (statement())
                    {
                        if (word == keyword["else"])
                        {
                            nw();
                            if (statement())
                            {
                                parsearr.push_back({tposi, "<条件语句>"});
                                return true;
                            }
                            return false;
                        }
                        parsearr.push_back({tposi, "<条件语句>"});
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

//<语句>    ::= <条件语句>｜<循环语句>| '{'<语句列>'}'| <有返回值函数调用语句>;|<无返回值函数调用语句>;｜<赋值语句>;｜<读语句>;｜<写语句>;｜<空>;|<返回语句>;
bool statement()
{
    if (ifStatement())
    {
        parsearr.push_back({tposi, "<语句>"});
        return true;
    }
    if (loopStatement())
    {
        parsearr.push_back({tposi, "<语句>"});
        return true;
    }
    if (word == delimiter["{"])
    {
        nw();
        bool flag = false;
        if (statementList())
        {
            if (word == delimiter["}"])
            {
                nw();
                parsearr.push_back({tposi, "<语句>"});
                flag = true;
            }
        }
        return flag;
    }

    if (scanfStatement() or printfStatement() or returnStatement())
    {
        if (word == delimiter[";"])
        {
            nw();
            parsearr.push_back({tposi, "<语句>"});
            return true;
        }
        return false;
    }
    /*
    fun(a,b);
    fun(a,b);

    a=b;
    a[1]=b;

    */
    else if (word == id and tokenbuf[tposi] == delimiter["("])
    {
        if (hasVar[getId(tposi - 1)])
            returnFunctionCall();
        else
            voidFunctionCall();
        if (word == delimiter[";"])
        {
            nw();
            parsearr.push_back({tposi, "<语句>"});
            return true;
        }
        return false;
    }
    else if (word == id) //error
    {
        assignStatement();
        if (word == delimiter[";"])
        {
            nw();
            parsearr.push_back({tposi, "<语句>"});
            return true;
        }
        return false;
    }
    else if (word == delimiter[";"])
    {
        nw();
        parsearr.push_back({tposi, "<语句>"});
        return true;
    }
    return false;
}

// <语句列>   ::= ｛<语句>｝
bool statementList()
{
    int cnt = 0;
    bool flag;
    do
    {
        flag = false;
        if (statement())
        {
            flag = true;
            cnt++;
        }
    } while (flag);
    if (cnt > 0)
    {
        parsearr.push_back({tposi, "<语句列>"});
    }
    return cnt > 0;
}
bool varDeclare();
//<复合语句>   ::=  ［<常量说明>］［<变量说明>］<语句列>
bool compoundStatement()
{
    if (word == keyword["const"])
        constDeclare();
    if (word == keyword["int"] or word == keyword["char"])
        varDeclare();
    if (statementList())
    {
        parsearr.push_back({tposi, "<复合语句>"});
        return true;
    }
    return false;
}

//<参数表>    ::=  (<类型标识符><标识符>{,<类型标识符><标识符>}) | <空>
bool parameterList()
{
    if (type())
    {
        if (word == id)
        {
            nw();
            bool flag = false;
            do
            {
                flag = false;
                if (word == delimiter[","])
                {
                    nw();
                    if (type())
                    {
                        if (word == id)
                        {
                            nw();
                            flag = true;
                        }
                    }
                }
            } while (flag);
        }
    }
    parsearr.push_back({tposi, "<参数表>"});
    return true;
}

//<声明头部>   ::=  int<标识符> |char<标识符>
bool declareHeader()
{
    bool flag = false;
    if (word == keyword["int"])
    {
        nw();
        if (word == id)
        {
            hasVar[getId(tposi - 1)] = true;
            nw();
            flag = true;
        }
    }
    else if (word == keyword["char"])
    {
        nw();
        if (word == id)
        {
            hasVar[getId(tposi - 1)] = true;

            nw();
            flag = true;
        }
    }
    if (flag)
    {
        parsearr.push_back({tposi, "<声明头部>"});
        return true;
    }
    return false;
}

// <有返回值函数定义>  ::=  <声明头部>'('<参数表>')' '{'<复合语句>'}'
bool returnFunctionDefine()
{
    if (declareHeader())
    {
        if (word == delimiter["("])
        {
            nw();
            if (parameterList())
            {
                if (word == delimiter[")"])
                {
                    nw();
                    if (word == delimiter["{"])
                    {
                        nw();
                        if (compoundStatement()) // error
                        {
                            if (word == delimiter["}"])
                            {
                                nw();
                                parsearr.push_back({tposi, "<有返回值函数定义>"});
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}
//<变量定义>  ::= <类型标识符>(<标识符>|<标识符>'['<无符号整数>']')
// {,(<标识符>|<标识符>'['<无符号整数>']' )}
bool varDefine()
{
    bool rflag = false;
    if (type()) //error
    {
        if (word == id)
        {
            rflag = true;
            nw();
            if (word == delimiter["["])
            {
                nw();
                if (unsignedI())
                {
                    if (word == delimiter["]"])
                    {
                        nw();
                    }
                    else
                        rflag = false;
                }
                else
                    rflag = false;
            }
            // <类型标识符>(<标识符>|<标识符>'['<无符号整数>']')

            bool flag = false;
            do
            {
                flag = false;
                if (word == delimiter[","])
                {
                    nw();
                    if (word == id)
                    {
                        nw();
                        //{,'['<无符号整数>']' }
                        if (word == delimiter["["])
                        {
                            if (unsignedI())
                            {
                                nw();
                                if (word == delimiter["]"])
                                {
                                    nw();
                                    flag = true;
                                }
                                else
                                    rflag = false;
                            }
                            else
                                rflag = false;
                        }
                        else if (word == delimiter[","])
                        {
                            flag = true;
                        }
                    }
                    else
                        rflag = false;
                }
            } while (flag);
        }
    }
    if (rflag)
        parsearr.push_back({tposi, "<变量定义>"});
    return rflag;
}

// <变量说明>  ::= <变量定义>;{<变量定义>;}
bool varDeclare()
{
    int cnt = 0;
    bool flag;
    do
    {
        flag = false;
        //int arr,bff[10];
        //int arr[10];
        //int buf;
        if ((word == keyword["int"] or word == keyword["char"]) and (tokenbuf[tposi + 1] == delimiter[","] or tokenbuf[tposi + 1] == delimiter["["] or tokenbuf[tposi + 1] == delimiter[";"]))
        {
            varDefine();
            if (word == delimiter[";"])
            {
                nw();
                cnt++;
                flag = true;
            }
        }
    } while (flag);
    if (cnt > 0)
        parsearr.push_back({tposi, "<变量说明>"});
    return cnt > 0;
}
//<常量定义>   ::=   int<标识符>＝<整数>{,<标识符>＝<整数>}
// |    char<标识符>＝<字符>{,<标识符>＝<字符>}
bool constDefine()
{
    bool flag = false;
    if (word == keyword["int"])
    {
        nw();
        if (word == id)
        {
            nw();
            if (word == delimiter["="])
            {
                nw();
                if (integer())
                {
                    flag       = true;
                    bool flag1 = false;
                    do
                    {
                        flag1 = false;
                        if (word == delimiter[","])
                        {
                            nw();
                            if (word == id)
                            {
                                nw();
                                ;
                                if (word == delimiter["="])
                                {
                                    nw();
                                    if (integer())
                                    {
                                        flag1 = true;
                                    }
                                }
                            }
                        }
                    } while (flag1);
                    parsearr.push_back({tposi, "<常量定义>"});
                    return flag;
                }
            }
        }
    }
    else if (word == keyword["char"])
    {
        nw();
        if (word == id)
        {
            nw();
            if (word == delimiter["="])
            {
                nw();
                if (word == "CHARCON")
                {
                    nw();
                    flag       = true;
                    bool flag1 = false;
                    do
                    {
                        flag1 = false;
                        if (word == delimiter[","])
                        {
                            nw();
                            if (word == id)
                            {
                                nw();
                                if (word == delimiter["="])
                                {
                                    nw();
                                    if (word == "CHARCON")
                                    {
                                        nw();
                                        flag1 = true;
                                    }
                                }
                            }
                        }
                    } while (flag1);
                    parsearr.push_back({tposi, "<常量定义>"});
                    return flag;
                }
            }
        }
    }
    return false;
}

//<常量说明> ::=  const<常量定义>;{ const<常量定义>;}
bool constDeclare()
{
    int cnt   = 0;
    bool flag = false;
    do
    {
        flag = false;
        if (word == keyword["const"])
        {
            nw();
            if (constDefine())
            {
                if (word == delimiter[";"])
                {
                    nw();
                    // cout << "<常量说明>" << endl;
                    flag = true;
                    cnt++;
                }
            }
        }
    } while (flag);
    if (cnt > 0)
        parsearr.push_back({tposi, "<常量说明>"});
    return cnt > 0;
}

//<主函数>    ::= void main‘(’‘)’ ‘{’<复合语句>‘}’
bool mainFunction()
{
    if (word == keyword["void"])
    {
        nw();
        if (word == keyword["main"])
        {
            nw();
            if (word == delimiter["("])
            {
                nw();
                if (word == delimiter[")"])
                {
                    nw();
                    if (word == delimiter["{"])
                    {
                        nw();
                        if (compoundStatement())
                        {
                            if (word == delimiter["}"])
                            {
                                nw();
                                parsearr.push_back({tposi, "<主函数>"});
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

//<程序>    ::= ［<常量说明>］［<变量说明>］{<有返回值函数定义>|<无返回值函数定义>}<主函数>
bool program()
{
    nw();
    if (word == keyword["const"])
        constDeclare(); //Ok
    if (word == keyword["int"] or word == keyword["char"])
        varDeclare();
    bool flag;
    do
    {
        if (word == keyword["void"] and tokenbuf[tposi] != keyword["main"])
            flag = voidFunctionDefine() or flag;
        else if (word == keyword["int"] or word == keyword["char"])
            flag = returnFunctionDefine() or flag;
        else
            break;
    } while (flag);
    if (mainFunction())
    {
        parsearr.push_back({tposi, "<程序>"});
        return true;
    }
    else
        return false;
};
} // namespace Parse

signed main(void)
{
    using namespace Lexical;
    using namespace Parse;
    init();
    if (program())
        ;
    size_t j = 0;
    for (size_t i = 0; i < outarr.size(); i++)
    {
        while (i + 1 == parsearr[j].first)
        {
            cout << parsearr[j++].second << endl;
        }
        cout << outarr[i];
    }
    while (j != parsearr.size())
    {
        cout << parsearr[j++].second << endl;
    }

    return 0;
}