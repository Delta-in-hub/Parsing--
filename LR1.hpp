#ifndef PARSERLR1__
#define PARSERLR1__
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Parser
{
class LR1
{
  protected:
    const std::string STARTCHAR{"__<START>__"};
    const std::string ENDCHAR{"__<END>__"};
    const std::string NULLCHAR{"__<NULL>__"};

    std::vector<std::vector<std::string>> production;

    std::unordered_set<std::string> terminal;
    std::unordered_map<std::string, std::vector<size_t>> nonterminal;
    std::unordered_map<size_t, std::string> prodToNonter;

    std::unordered_map<std::string, std::unordered_set<std::string>> firstSet;

    struct Item
    {
        size_t productionPosi;
        size_t dotPosi;
        std::string next;
        bool operator<(const Item& t) const
        {
            return tie(productionPosi, dotPosi, next) < tie(t.productionPosi, t.dotPosi, t.next);
        }
    };
    struct ItemSet
    {
        std::set<Item> item;
        bool operator<(const ItemSet& t) const
        {
            return item < t.item;
        }
    };

    std::set<ItemSet> CC;

    //return the first nonterminal
    std::string constructGrammar(const std::string& s)
    {
        using namespace std;

        static const string NullStr("<null>");

        fstream fs(s);
        if (not fs.is_open())
            throw invalid_argument("File don't exist.");
        string str;
        string firstTerminal;
        bool isFirst = true;
        while (getline(fs, str))
        {
            if (str.size() == 0 or str[0] == ';')
                continue;
            //E ::= T E' ;comments
            string nonter;
            // size_t pos = 0;
            auto pos = str.find("::=");
            if (pos == string::npos)
            {
                throw invalid_argument("production grammar error.\n");
            }
            for (size_t i = 0; i < pos; i++)
            {
                if (not isspace(str[i]))
                    nonter.push_back(str[i]);
            }
            if (isFirst)
            {
                isFirst       = false;
                firstTerminal = nonter;
            }
            pos += 3;
            while (isspace(str[pos]))
                pos++;
            string ter;
            vector<vector<string>> arr;
            arr.push_back({});
            size_t cnt = 0;
            while (pos != str.size() + 1)
            {
                if (pos == str.size())
                {
                    if (not ter.empty())
                    {
                        if (ter == NullStr)
                        {
                            arr[cnt].push_back(NULLCHAR);
                        }
                        else
                        {
                            arr[cnt].push_back(move(ter));
                        }
                        ter.clear();
                    }
                    break;
                }
                if (str[pos] == ';')
                {
                    break;
                }
                if (str[pos] != ' ' and str[pos] != '|')
                {
                    ter.push_back(str[pos]);
                }
                else if (str[pos] == '|')
                {
                    arr.push_back({});
                    ++cnt;
                }
                else if (not ter.empty())
                {
                    if (ter == NullStr)
                    {
                        arr[cnt].push_back(NULLCHAR);
                    }
                    else
                    {
                        arr[cnt].push_back(move(ter));
                    }
                    ter.clear();
                }
                ++pos;
            }
            auto&& tmp = nonterminal[nonter];
            for (auto&& i : arr)
            {
                tmp.push_back(production.size());
                prodToNonter[production.size()] = nonter;
                production.push_back(move(i));
                // nonterminal[nonter].symbol = move(arr);
            }
        }
        return firstTerminal;
    }
    void findAllTerminal()
    {
        for (auto&& i : production)
        {
            for (auto&& j : i)
            {
                if (nonterminal.find(j) == nonterminal.end())
                    terminal.emplace(j);
            }
        }
    }
    void getFirst()
    {
        using namespace std;

        bool isChanged = false;
        while (true)
        {
            isChanged = false;
            for (auto&& i : nonterminal)
            {
                for (auto&& _j : i.second)
                {
                    auto&& j    = production[_j];
                    auto&& fron = j.front();
                    if (terminal.find(fron) != terminal.end())
                    {
                        isChanged = (firstSet[i.first].insert(fron).second or isChanged);
                    }
                    else
                    {
                        bool allNull = true;
                        for (auto&& k : j)
                        {
                            auto pos = nonterminal.find(k);
                            if (pos != nonterminal.end())
                            {
                                bool isn = false;
                                for (auto&& m : firstSet[pos->first])
                                {
                                    if (m != NULLCHAR)
                                    {
                                        isChanged = (firstSet[i.first].insert(m).second or isChanged);
                                    }
                                    else
                                        isn = true;
                                }
                                if (not isn)
                                {
                                    allNull = false;
                                    break;
                                }
                            }
                            else
                            {
                                allNull   = false;
                                isChanged = (firstSet[i.first].insert(k).second or isChanged);
                                break;
                            }
                        }
                        if (allNull)
                        {
                            isChanged = (firstSet[i.first].insert(NULLCHAR).second or isChanged);
                        }
                    }
                }
            }
            if (not isChanged)
                break;
        }
    }

    static std::vector<std::string> tmpFirst;
    bool first(const std::vector<std::string>& t, size_t s)
    {
        tmpFirst.clear();
        for (size_t i = s; i < t.size(); i++)
        {
            auto&& now = t[i];
            if (terminal.find(now) != terminal.end())
            {
                tmpFirst.push_back(now);
                return false;
            }
            else
            {
                auto&& s    = firstSet[now];
                bool isnull = false;
                for (auto&& i : s)
                {
                    if (i != NULLCHAR)
                        tmpFirst.push_back(i);
                    else
                        isnull = true;
                }
                if (not isnull)
                    return false;
            }
        }
        return true;
    }

    void closure(ItemSet& a)
    {
        using namespace std;
        queue<Item> q;
        for (auto&& i : a.item)
        {
            q.push(i);
        }
        while (not q.empty())
        {
            auto&& now = q.front();
            if (now.dotPosi < production[now.productionPosi].size())
            {
                auto pos = nonterminal.find(production[now.productionPosi][now.dotPosi]);
                if (pos != nonterminal.end())
                {
                    if (first(production[now.productionPosi], now.dotPosi + 1))
                    {
                        tmpFirst.push_back(now.next);
                    }
                    for (auto&& j : pos->second)
                    {
                        for (auto&& i : tmpFirst)
                        {
                            auto flag = a.item.insert({j, 0, i});
                            if (flag.second)
                            {
                                q.push(*(flag.first));
                            }
                        }
                    }
                }
            }
            q.pop();
        }
    }

    ItemSet goto_(const ItemSet& s, const std::string& x)
    {
        ItemSet n;
        for (auto&& i : s.item)
        {
            if (i.dotPosi < production[i.productionPosi].size() and production[i.productionPosi][i.dotPosi] == x)
            {
                n.item.insert({i.productionPosi, i.dotPosi + 1, i.next});
            }
        }
        closure(n);
        return n;
    }

    struct hashpair
    {
        size_t operator()(const std::pair<const ItemSet*, std::string>& t) const
        {
            return std::hash<const ItemSet*>()(t.first) xor std::hash<std::string>()(t.second);
        }
    };
    std::unordered_map<std::pair<const ItemSet*, std::string>, std::tuple<char, std::string, const ItemSet*, size_t>, hashpair> Action;

    std::unordered_map<std::pair<const ItemSet*, std::string>, const ItemSet*, hashpair> Goto;

    std::unordered_map<const ItemSet*, std::vector<std::pair<const ItemSet*, std::string>>> path;

    const ItemSet* StartState;
    void constructCC()
    {
        using namespace std;
        auto error = []() {
            throw std::logic_error("NOT LR1 GRAMMAR!\n");
        };
        ItemSet tmp;
        tmp.item.insert({production.size() - 1, 0, ENDCHAR});
        closure(tmp);
        CC.emplace(move(tmp));
        queue<decltype(CC.begin())> q;
        q.push(CC.begin());
        StartState = &(*(CC.begin()));
        while (not q.empty())
        {
            auto&& now = q.front();
            unordered_set<string> behinddot;
            for (auto&& i : now->item)
            {
                if (i.dotPosi == production[i.productionPosi].size())
                {
                    auto n = production[i.productionPosi].size();
                    if (production[i.productionPosi].size() == 1 and production[i.productionPosi].front() == NULLCHAR)
                    {
                        n = 0;
                    }
                    if (not Action.insert({{&(*now), i.next}, {'r', prodToNonter[i.productionPosi], nullptr, n}}).second)
                        error();
                }
                else // if (production[i.productionPosi][i.dotPosi] != NULLCHAR)
                {
                    behinddot.insert(production[i.productionPosi][i.dotPosi]);
                }
            }
            for (auto&& x : behinddot)
            {
                auto tmp = goto_(*now, x);
                if (tmp.item.empty())
                    continue;
                auto pos = CC.emplace(move(tmp));
                if (pos.second)
                {
                    q.push(pos.first);
                }
                auto ntpos = nonterminal.find(x);
                if (ntpos != nonterminal.end())
                {
                    if (not Goto.insert({{&(*now), x}, &(*pos.first)}).second)
                        error();
                }
                else
                {
                    if (not Action.insert({{&(*now), x}, {'s', "", &(*pos.first), 0}}).second)
                    {
                        error();
                    }
                }
            }
            q.pop();
        }
    }

  public:
    LR1(const std::string& filePath)
    {
        using namespace std;
        auto&& res = constructGrammar(filePath);
        nonterminal[STARTCHAR].push_back(production.size());
        prodToNonter[production.size()] = STARTCHAR;
        production.push_back({move(res)});
        // terminal.insert(ENDCHAR);
        // terminal.insert(NULLCHAR);
        findAllTerminal();
        getFirst();
        constructCC();
        // size_t n = 0;
        // for (auto&& i : CC)
        // {
        //     n += i.item.size();
        // }
        // cout << n << endl;
        // cout << CC.size() << endl;
        // cout << Action.size() << endl;
        // cout << Goto.size() << endl;
    }

    bool parse(const std::string& input)
    {
        using namespace std;
        char* buf = new char[input.size() + ENDCHAR.size() + 10];
        strcpy(buf, input.data());
        buf[input.size()] = ' ';
        strcpy(buf + input.size() + 1, ENDCHAR.data());

        char* nc      = buf;
        auto nextWord = [&]() -> string {
            string res;
            if (*nc == 0)
                throw std::range_error("");
            while (isspace(*nc))
                nc++;
            while (not isspace(*nc) and *nc != 0)
                res.push_back(*nc++);
            return res;
        };
        auto error = [&]() {
            cout << buf << endl;
            char* l = buf;
            while (++l < nc)
                cout << ' ';
            cout << "^^";
            cout << endl;
            delete[] buf;
            throw std::logic_error("INPUT ERROR!");
        };
        stack<const ItemSet*> state;
        state.push(StartState);
        stack<string> symbol;
        auto word = nextWord();
        while (true)
        {
            auto ns  = state.top();
            auto pos = Action.find({ns, word});
            if (pos == Action.end())
            {
                pos = Action.find({ns, NULLCHAR});
                if (pos == Action.end())
                    error();
                else
                {
                    state.push(get<2>(pos->second));
                }
                continue;
            }
            auto&& tmp = pos->second;
            if (get<0>(tmp) == 's')
            {
                symbol.push(word);
                state.push(get<2>(tmp));
                word = nextWord();
            }
            else
            {
                if (get<1>(tmp) == STARTCHAR and *nc == 0)
                {
                    break;
                }
                else
                {
                    auto n = get<3>(tmp);
                    if (n == 0)
                        state.pop();
                    while (n--)
                    {
                        state.pop();
                        symbol.pop();
                    }
                    symbol.push(get<1>(tmp));
                    state.push(Goto[{state.top(), get<1>(tmp)}]);
                }
            }
        }
        return true;
    }
};

std::vector<std::string> LR1::tmpFirst{};

} // namespace Parser

#endif