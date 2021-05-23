#include "./LL1.hpp"
#include "./LR1.hpp"
#include <bits/stdc++.h>
using namespace std;
#define debug(x) std::cout << #x << " is " << (x) << '\n'

const char* nc;

char isConstNum(double* n)
{
    double num  = 0;
    double f    = 1.0 / 10;
    int p       = 0;
    bool isnegi = false;
    char state  = -1;
    bool flag   = false;
    while (1)
    {
        switch (state)
        {
        case -1:
            if (isdigit(*nc))
            {
                state = 0;
            }
            else
                flag = true;
            break;
        case 0:
            if (isdigit(*nc))
            {
                while (isdigit(*nc))
                {
                    num = num * 10 + *nc - '0';
                    ++nc;
                }
                state = 0;
            }
            else if (*nc == '.')
            {
                state = 1;
                ++nc;
            }
            else if (*nc == 'e')
            {
                state = 3;
                ++nc;
            }
            else
                flag = true;
            break;
        case 1:
            if (isdigit(*nc))
                state = 2;
            else
                flag = true;
            break;
        case 2:
            if (isdigit(*nc))
            {
                while (isdigit(*nc))
                {
                    num += (*nc - '0') * f;
                    f /= 10;
                    ++nc;
                }
            }
            else if (*nc == 'e')
            {
                state = 3;
                ++nc;
            }
            else
                flag = true;
            break;
        case 3:
            if (*nc == '+')
            {
                state = 4;
                ++nc;
            }
            else if (*nc == '-')
            {
                isnegi = true;
                state  = 4;
                ++nc;
            }
            else if (isdigit(*nc))
            {
                state = 6;
            }
            else
                flag = true;
            break;
        case 4:
            if (isdigit(*nc))
                state = 5;
            else
                flag = true;
            break;

        case 5:
            if (isdigit(*nc))
            {
                while (isdigit(*nc))
                {
                    p = p * 10 + *nc - '0';
                    ++nc;
                }
            }
            else
                flag = true;
            break;
        default:
            flag = true;
            break;
        }
        if (flag)
            break;
    }
    p = (isnegi == true) ? -p : p;
    num *= pow(10, p);
    *n = num;
    return state;
}

signed main(void)
{
    Parser::LR1 p("grammar/2.txt");
    Parser::LL1 p2("grammar/2.txt");

    string str("xx+y10*(33*a+0.7)-b/523e-2");
    // cin >> str;
    nc = str.data();
    string token;
    double tmp;
    while (*nc)
    {
        while (isspace(*nc))
            nc++;
        if (*nc == 0)
            break;
        switch (*nc)
        {
        case '+':
        case '-':
        case '*':
        case '/':
        case '(':
        case ')':
            token.push_back(*nc);
            token.push_back(' ');
            nc++;
            break;
        default:
            if (isdigit(*nc))
            {
                switch (isConstNum(&tmp))
                {
                case 0:
                case 2:
                case 5:
                    token += "Num ";
                    break;
                default:
                    cout << "输入数字有误!" << endl;
                    return 1;
                    break;
                }
            }
            else if (*nc == '_' or isalpha(*nc))
            {
                while (*nc == '_' or isalnum(*nc))
                {
                    nc++;
                }
                token += "Id ";
            }
            break;
        }
    }
    // cout << token << endl;
    assert(p.parse(token));
    assert(p2.parse(token));
    cout << "Success!" << endl;
    return 0;
}
