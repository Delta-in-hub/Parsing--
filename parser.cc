// #include "./LL1.hpp"
#include "./LR1.hpp"
#include <bits/stdc++.h>
using namespace std;
#define debug(x) std::cout << #x << " is " << (x) << '\n'

signed main(void)
{
    clock_t s = clock();
    Parser::LR1 p("grammar/3.txt");
    cout << clock() - s << endl;
    assert(p.parse("(a)-(b)"));
    cout << clock() - s << endl;
    cout << "Successed" << endl;
    return 0;
}
