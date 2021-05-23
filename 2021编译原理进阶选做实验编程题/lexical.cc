#include <ciso646>
#include <cstring>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

char buf[100000];
size_t strLen;
char* nc = buf;

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

signed main(void)
{
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
    ofstream os("output.txt");
    while (getline(infile, tmp))
    {
        strcpy(input, tmp.data());
        input += tmp.size();
    }
    strLen = strlen(buf);
    while (true)
    {
        while (*nc != 0 and isspace(*nc))
        {
            nc++;
        }
        if (*nc == 0)
            break;
        char* r = isIdentify();
        if (r > nc)
        { //nc - r
            bool iskeyword = false;
            for (auto&& i : keyword)
            {
                if (string(nc, r) == i.first)
                {
                    iskeyword = true;
                    os << i.second << ' ';
                    while (nc != r)
                        os << *nc++;
                    os << endl;
                    break;
                }
            }
            if (not iskeyword)
            {
                os << "IDENFR" << ' ';
                while (nc != r)
                    os << *nc++;
                os << endl;
            }
            continue;
        }
        r = isInt();
        if (r > nc)
        {
            os << "INTCON" << ' ';
            while (nc != r)
                os << *nc++;
            os << endl;
            continue;
        }
        r = isChar();
        if (r > nc)
        {
            nc++;
            os << "CHARCON" << ' ';
            while (nc + 1 != r)
                os << *nc++;
            os << endl;
            nc = r;
            continue;
        }
        r = isStr();
        if (r > nc)
        {
            nc++;
            os << "STRCON" << ' ';
            while (nc != r)
                os << *nc++;
            os << endl;
            nc++;
            continue;
        }
        for (auto&& i : identify)
        {
            if (strncmp(nc, i.first.data(), i.first.size()) == 0)
            {
                os << i.second << ' ';
                os << i.first << endl;
                nc += i.first.size();
                break;
            }
        }
    }
    return 0;
}