#include <bits/stdc++.h>

using namespace std;
constexpr size_t MAXINPUTLEN = 1e2;
constexpr char terminal[]    = "+-*/()";
char buf[MAXINPUTLEN + 100];
char* nc;

struct tuples
{
    string op;
    string arg1, arg2;
    string res;
};

string getTempAddress()
{
    static int cnt = 0;
    return "_TA_" + to_string(cnt++) + "_";
}

vector<tuples> arr;

void error()
{
    cout << "错误";
    exit(1);
}

//0 for Integer ,2/5 for Float ,others for Error
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

string Factor();
string Term();
string Exp();

//<Factor> ::= [a-zA-Z_][a-zA-Z_0-9]*
// [0-9] |( <Exp> ) | - <Factor>
string Factor()
{
    if (isdigit(*nc))
    {
        double t;
        switch (isConstNum(&t))
        {
        case 0:
            return to_string((int)(t));
        case 2:
        case 5:
            return to_string(t);
            break;
        default:
            error();
            break;
        }
    }
    else if (isalpha(*nc) or *nc == '_')
    {
        char* l = nc;
        ++nc;
        while (isalpha(*nc) or *nc == '_' or isdigit(*nc))
            nc++;
        return string(l, nc);
    }
    else if (*nc == '-')
    {
        nc++;
        string op1 = Factor();
        string tmp = getTempAddress();
        arr.push_back({"-", op1, "<NULL>", tmp});
        return tmp;
    }
    else if (*nc == '(')
    {
        nc++;
        string op1 = Exp();
        if (*nc == ')')
        {
            ++nc;
            return op1;
        }
        else
            error();
    }
    else
        error();
    return "";
}

// <Term> ::= <Factor> (*<Factor> | / <Factor>)*
string Term()
{
    string op3;
    string op1 = Factor();
    if (*nc == 0)
        return "";
    while (*nc == '*' or *nc == '/')
    {
        string op;
        op.push_back(*nc);
        nc++;
        string op2 = Factor();
        op3        = getTempAddress();
        arr.push_back({op, op1, op2, op3});
        op1 = op3;
    }
    return op1;
}

// <Exp> ::= <Term> (+ <Term> | - <Term>)*
string Exp()
{
    string op3;
    string op1 = Term();
    if (*nc == 0)
        return "";
    while (*nc == '+' or *nc == '-')
    {
        string opt;
        opt.push_back(*nc);
        nc++;
        string op2 = Term();
        op3        = getTempAddress();
        arr.push_back({opt, op1, op2, op3});
        op1 = op3;
    }
    return op3;
}

signed main(void)
{
    cin >> buf;
    nc = buf;
    cout << Exp() << endl;
    cout << "正确" << endl;
    for (auto&& i : arr)
    {
        printf("(%s,%s,%s,%s)", i.op.data(), i.arg1.data(), i.arg2.data(), i.res.data());
        putchar(10);
    }
    return 0;
}
/*

<Exp> ::= <Term> (+ <Term> | - <Term>)*
<Term> ::= <Factor> (*<Factor> | / <Factor>)*

<Factor> ::= [a-zA-Z_][a-zA-Z_0-9]*
[0-9] |( <Exp> ) | - <Factor>

//!

<Exp> ::= <Exp> + <Term> |
<Exp> - <Term> |
<Term>
<Term> ::= <Term> * <Factor> |
<Term> / <Factor> |
<Factor>

*/