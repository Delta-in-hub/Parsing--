#include "./LL1.hpp"
#include <bits/stdc++.h>
using namespace std;
#define debug(x) std::cout << #x << " is " << (x) << '\n'

signed main(void)
{
    clock_t s = clock();
    Parser::LL1 p("grammar/2.txt");
    cout << clock() - s << endl;
    assert(p.parse("name+name*name+num-num*num/num"));
    cout << clock() - s << endl;
    cout << "Successed" << endl;
    return 0;
}
