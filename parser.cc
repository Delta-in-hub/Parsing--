#include "./parser--.hpp"
#include <bits/stdc++.h>
using namespace std;
#define debug(x) std::cout << #x << " is " << (x) << '\n'

signed main(void)
{
    Parser::LL1 p("grammar/2.txt");
    assert(p.parse("name+name*name"));
    cout << "Successed" << endl;
    return 0;
}
