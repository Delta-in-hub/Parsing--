#include "./LL1.hpp"
#include "./LR1.hpp"
#include <bits/stdc++.h>
using namespace std;
#define debug(x) std::cout << #x << " is " << (x) << '\n'

signed main(void)
{

    Parser::LR1 p("grammar/2.txt");
    Parser::LL1 p2("grammar/2.txt");
    assert(p.parse("1 + 2 - 3 * ( a * b / x + y ) * ( 0 / 2 )"));
    assert(p2.parse("1 + 2 - 3 * ( a * b / x + y ) * ( 0 / 2 )"));
    cout << "Success!" << endl;
    return 0;
}
