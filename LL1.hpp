#ifndef PARSERLL1__
#define PARSERLL1__
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <stack>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Parser
{
class LL1
{
  protected:
    const std::string STARTCHAR{"__<START>__"};
    const std::string ENDCHAR{"__<END>__"};
    const std::string NULLCHAR{"__<NULL>__"};

    std::vector<std::vector<std::string>> production;

    std::unordered_set<std::string> terminal;
    std::unordered_map<std::string, std::vector<size_t>> nonterminal;

    std::unordered_map<std::string, std::unordered_set<std::string>> firstSet;
    std::unordered_map<std::string, std::unordered_set<std::string>> followSet;

    std::unordered_map<std::string, std::unordered_map<std::string, size_t>> table;
    // std::map<std::pair<std::string, std::string>, size_t> table;

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
    void constructTable()
    {
        auto error = [&](size_t a, size_t b) {
            for (auto&& i : nonterminal)
            {
                if (find(begin(i.second), end(i.second), a) != end(i.second))
                {
                    std::cout << i.first << " ::= ";
                }
            }

            for (auto&& i : production[a])
            {
                std::cout << i << ' ';
            }
            std::cout << std::endl;

            for (auto&& i : nonterminal)
            {
                if (find(begin(i.second), end(i.second), b) != end(i.second))
                {
                    std::cout << i.first << " ::= ";
                }
            }
            for (auto&& i : production[b])
            {
                std::cout << i << ' ';
            }
            std::cout << std::endl;

            throw std::logic_error("NOT LL1 GRAMMAR!\n");
        };
        for (auto&& i : nonterminal)
        {
            for (auto&& _j : i.second)
            {
                auto&& j     = production[_j];
                bool allNull = true;
                for (auto&& k : j)
                {
                    if (terminal.find(k) != terminal.end())
                    {
                        if (k != NULLCHAR)
                        {
                            auto res = table[i.first].insert({k, _j});
                            if (not res.second)
                            {
                                error(res.first->second, _j);
                            }
                            allNull = false;
                        }
                        break;
                    }
                    else
                    {
                        auto&& fir = firstSet[k];
                        bool flag  = false;
                        for (auto&& m : fir)
                        {
                            if (m != NULLCHAR)
                            {
                                auto res = table[i.first].insert({m, _j});
                                if (not res.second)
                                {
                                    error(res.first->second, _j);
                                }
                            }
                            else
                                flag = true;
                        }
                        if (not flag)
                        {
                            allNull = false;
                            break;
                        }
                    }
                }
                if (allNull)
                {
                    for (auto&& m : followSet[i.first])
                    {
                        auto res = table[i.first].insert({m, _j});
                        if (not res.second)
                        {
                            error(res.first->second, _j);
                        }
                    }
                }
            }
        }
    }
    void removeLeftRecursion()
    {
        using namespace std;
        vector<string> allNonterminal;
        allNonterminal.reserve(nonterminal.size());
        for (auto&& i : nonterminal)
        {
            allNonterminal.push_back(i.first);
        }

        for (auto i_ = allNonterminal.begin(); i_ != allNonterminal.end(); ++i_)
        {
            //Ai
            auto i = nonterminal.find(*i_);
            // cout << i->first << endl;
            vector<size_t> removeIndex, toInsert;
            for (auto j_ = allNonterminal.begin(); j_ != i_; ++j_)
            {
                //Aj
                auto j = nonterminal.find(*j_);

                //Ai -> ...
                for (size_t _i = 0; _i != i->second.size(); ++_i)
                {
                    // auto&& np = production[i->second[_i]];
                    // cout << production[i->second[_i]].front() << endl;

                    //Ai -> Aj ...
                    if (production[i->second[_i]].front() == *j_)
                    {

                        removeIndex.push_back(_i);

                        for (auto&& _j : j->second)
                        {
                            toInsert.push_back(production.size());
                            production.push_back(production[_j]);

                            for (auto k = production[i->second[_i]].begin() + 1; k < production[i->second[_i]].end(); ++k)
                            {
                                production.back().push_back(*k);
                            }
                        }
                    }
                }
            }
            size_t l2 = 0;
            for (auto&& j : removeIndex)
            {
                i->second[j] = toInsert[l2++];
            }
            while (l2 < toInsert.size())
            {
                i->second.push_back(toInsert[l2++]);
            }

            vector<size_t> reindex, nonindex;
            for (size_t _i = 0; _i != i->second.size(); ++_i)
            {
                auto&& np = production[i->second[_i]];
                if (np.front() == i->first)
                    reindex.push_back(_i);
                else
                    nonindex.push_back(_i);
            }

            if (not reindex.empty())
            {
                string newname = i->first + "_NEW_";

                nonterminal[newname].push_back(production.size());

                production.push_back({NULLCHAR});
                terminal.insert(NULLCHAR);

                for (auto&& j : reindex)
                {

                    auto&& np = production[i->second[j]];
                    //
                    vector<string> tmp;
                    copy(np.begin() + 1, np.end(), back_inserter(tmp));
                    tmp.push_back(newname);
                    //aA'

                    nonterminal[newname].push_back(production.size());
                    production.push_back(move(tmp));
                }
                vector<size_t> res;
                res.reserve(nonindex.size());
                for (auto&& j : nonindex)
                {
                    res.push_back(i->second[j]);
                    auto&& np = production[i->second[j]];
                    np.push_back(newname);
                }
                // i->second = move(nonindex);  bug !
                i->second = move(res);
            }
        }
    }

  public:
    LL1(const std::string& filePath)
    {
        auto&& res = constructGrammar(filePath);
        nonterminal[STARTCHAR].push_back(production.size());
        production.push_back({move(res), ENDCHAR});
        findAllTerminal();
        removeLeftRecursion();
        getFirst();
        getFollow();
        constructTable();
    }

    bool parse(const std::string& input)
    {
        using namespace std;
        char* buf = new char[input.size() + ENDCHAR.size() + 2];
        strcpy(buf, input.data());
        strcpy(buf + input.size(), ENDCHAR.data());
        char* nc   = buf;
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
        stack<string> st;
        // st.push(ENDCHAR);
        st.push(STARTCHAR);
        while (not st.empty())
        {
            auto top = st.top();
            st.pop();
            if (top == ENDCHAR)
            {
                if (strncmp(top.data(), nc, top.size()) == 0)
                {
                    return true;
                }
                else
                    error();
            }
            else if (terminal.find(top) != terminal.end())
            {
                if (strncmp(top.data(), nc, top.size()) == 0)
                {
                    nc += top.size();
                }
                else
                    error();
            }
            else
            {
                auto&& tmp = table[top];
                bool flag  = false;
                for (auto&& i : tmp)
                {
                    if (strncmp(i.first.data(), nc, i.first.size()) == 0)
                    {
                        if (not(production[i.second].size() == 1 and production[i.second].front() == NULLCHAR))
                        {
                            for (auto j = production[i.second].rbegin(); j != production[i.second].rend(); ++j)
                            {
                                st.push(*j);
                            }
                        }
                        flag = true;
                        break;
                    }
                }
                if (not flag)
                    error();
            }
        }
        delete[] buf;
        return false;
    }
};
} // namespace Parser

#endif