#include "./parser--.hpp"
#include <bits/stdc++.h>
using namespace std;
#define debug(x) std::cout << #x << " is " << (x) << '\n'

signed main(void)
{
    Parser::LL1 p("grammar/1.txt");
    assert(p.parse("(i+i*i+(i*i))"));
    return 0;
}
