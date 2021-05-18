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
    std::unordered_map<std::string, std::unordered_set<std::string>> followSet;

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
                // if (i.second.nullable)
                // {
                //     isChanged = firstSet[i.first].insert(NULLCHAR).second;
                // }
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
    void getFollow()
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
                    auto&& j = production[_j];
                    for (auto k = begin(j); k != end(j); ++k)
                    {
                        auto pos = nonterminal.find(*k);
                        if (pos != end(nonterminal))
                        {
                            auto next = ++k;
                            --k;
                            bool allNull = true;
                            //A->aB(c)+  first((c)+)
                            for (; next != end(j); ++next)
                            {
                                if (terminal.find(*next) != terminal.end())
                                {
                                    isChanged = followSet[*k].insert(*next).second or isChanged;
                                    allNull   = false;
                                    break;
                                }
                                else
                                {
                                    bool isn = false;
                                    for (auto&& m : firstSet[*next])
                                    {
                                        if (m != NULLCHAR)
                                        {
                                            isChanged = followSet[*k].insert(m).second or isChanged;
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
                            }

                            if (allNull) //A->aB
                            {
                                for (auto&& m : followSet[i.first])
                                {
                                    isChanged = followSet[*k].insert(m).second or isChanged;
                                }
                            }
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
    std::unordered_map<std::pair<const ItemSet*, std::string>, std::tuple<char, std::string, const ItemSet*>, hashpair> Action;
    std::unordered_map<std::pair<const ItemSet*, std::string>, const ItemSet*, hashpair> Goto;

    std::unordered_map<const ItemSet*, std::vector<std::pair<const ItemSet*, std::string>>> path;
    void constructCC()
    {
        using namespace std;
        auto error = []() {
            throw std::logic_error("NOT LR1 GRAMMAR!\n");
        };
        ItemSet tmp;
        tmp.item.insert({0, 0, ENDCHAR});
        closure(tmp);
        CC.emplace(move(tmp));
        queue<decltype(CC.begin())> q;
        q.push(CC.begin());
        while (not q.empty())
        {
            auto&& now = q.front();
            unordered_set<string> behinddot;
            for (auto&& i : now->item)
            {
                if (i.dotPosi == production[i.productionPosi].size())
                {
                    if (not Action.insert({{&(*now), i.next}, {'r', prodToNonter[i.productionPosi], nullptr}}).second)
                        error();
                }
                else
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
                    if (not Action.insert({{&(*now), x}, {'s', "", &(*pos.first)}}).second)
                    {
                        error();
                    }
                }
            }
            q.pop();
        }
    }

    // void constructTable()
    // {
    //     for (auto&& i : CC)
    //     {
    //         for (auto&& j : i.item)
    //         {
    //             if (j.dotPosi == production[j.productionPosi].size())
    //             {
    //                 if (j.next == ENDCHAR)
    //                     // Action[{&i, j.next}] = {'a', "ACCEPT"};
    //                 else
    //                     // Action[{&i, j.next}] = {'r', prodToNonter[j.productionPosi]};
    //             }
    //             else
    //             {
    //                 auto&& now = production[j.productionPosi][j.dotPosi];
    //                 if(terminal.find(now) != terminal.end())
    //                 {
    //                     auto cj = goto_(i, now);
    //                     // Action[{&i, now}] = {'s',};
    //                 }
    //             }
    //         }
    //     }
    // }

  public:
    LR1(const std::string& filePath)
    {
        using namespace std;
        auto&& res = constructGrammar(filePath);
        // nonterminal[STARTCHAR].push_back(production.size());
        // prodToNonter[production.size()] = STARTCHAR;
        // production.push_back({move(res), ENDCHAR});
        terminal.insert(ENDCHAR);
        terminal.insert(NULLCHAR);
        findAllTerminal();
        getFirst();
        getFollow();
        // production.back().pop_back();
        constructCC();
        cout << "Action\t" << Action.size() << endl;
        for (auto&& i : Action)
        {
            cout << i.first.first->item.size() << ' '
                 << i.first.second << ' ' << get<0>(i.second) << ' ' << get<1>(i.second)
                 << endl;
        }
        cout << "GOTO\t" << Goto.size() << endl;
        for (auto&& i : Goto)
        {
            cout << i.first.first->item.size() << ' '
                 << i.first.second << ' ' << i.second->item.size() << endl;
        }
    }

    bool parse(const std::string& input)
    {
        using namespace std;
        return true;
    }
};

std::vector<std::string> LR1::tmpFirst{};

} // namespace Parser

#endif