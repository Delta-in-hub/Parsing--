#ifndef PARSER__
#define PARSER__
#include <algorithm>
#include <cstring>
#include <fstream>
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
    struct production
    {
        bool nullable = false;
        std::vector<std::vector<std::string>> symbol;
    };

    std::unordered_set<std::string> terminal;
    std::unordered_map<std::string, production> nonterminal;

    std::unordered_map<std::string, std::unordered_set<std::string>> firstSet;
    std::unordered_map<std::string, std::unordered_set<std::string>> followSet;
    //return the first nonterminal
    std::string constructGrammar(const std::string& s)
    {
        using namespace std;
        fstream fs(s);
        if (not fs.is_open())
            throw invalid_argument("File don't exist.");
        string str;
        string firstTerminal;
        bool isFirst = true;
        while (getline(fs, str))
        {
            if (str[0] == ';')
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
                        if (ter == "<null>")
                        {
                            nonterminal[nonter].nullable = true;
                            arr.pop_back();
                            --cnt;
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
                    if (ter == "<null>")
                    {
                        nonterminal[nonter].nullable = true;
                        arr.pop_back();
                        --cnt;
                    }
                    else
                    {
                        arr[cnt].push_back(move(ter));
                    }
                    ter.clear();
                }
                ++pos;
            }
            nonterminal[nonter].symbol = move(arr);
        }
        return firstTerminal;
    }
    void findAllTerminal()
    {
        for (auto&& i : nonterminal)
        {
            for (auto&& j : i.second.symbol)
            {
                for (auto&& k : j)
                {
                    if (nonterminal.find(k) == nonterminal.end())
                        terminal.emplace(k);
                }
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
                if (i.second.nullable)
                {
                    isChanged = firstSet[i.first].insert(NULLCHAR).second;
                }
                for (auto&& j : i.second.symbol)
                {
                    auto&& fron = j.front();
                    if (terminal.find(fron) != terminal.end())
                    {
                        isChanged = firstSet[i.first].insert(fron).second;
                    }
                    else
                    {
                        bool allNull = true;
                        for (auto&& k : j)
                        {
                            auto pos = nonterminal.find(k);
                            if (pos != nonterminal.end())
                            {
                                for (auto&& m : firstSet[pos->first])
                                {
                                    if (m != ENDCHAR)
                                    {
                                        isChanged = firstSet[i.first].insert(m).second;
                                    }
                                }
                                if (not pos->second.nullable)
                                {
                                    allNull = false;
                                    break;
                                }
                            }
                            else
                            {
                                allNull   = false;
                                isChanged = firstSet[i.first].insert(k).second;
                                break;
                            }
                        }
                        if (allNull)
                        {
                            isChanged = firstSet[i.first].insert(NULLCHAR).second;
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
                for (auto&& j : i.second.symbol)
                {
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
                                    isChanged = followSet[*k].insert(*next).second;
                                    allNull   = false;
                                    break;
                                }
                                else
                                {
                                    for (auto&& m : firstSet[*next])
                                    {
                                        if (m != NULLCHAR)
                                        {
                                            isChanged = followSet[*k].insert(m).second;
                                        }
                                    }
                                    if (not nonterminal[*next].nullable)
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
                                    isChanged = followSet[*k].insert(m).second;
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

  public:
    LL1(const std::string& filePath)
    {
        auto&& res = constructGrammar(filePath);
        nonterminal[STARTCHAR].symbol.push_back({move(res), ENDCHAR});
        findAllTerminal();
        getFirst();
        getFollow();
    }
    bool parse(const std::string& input)
    {
        using namespace std;
        char* buf = new char[input.size() + ENDCHAR.size() + 2];
        strcpy(buf, input.data());
        strcpy(buf + input.size(), ENDCHAR.data());
        char* nc   = buf;
        auto error = [&]() { string tmp("Error : " + string(nc,nc + input.size() + ENDCHAR.size()));delete[] buf;throw std::logic_error(tmp); };
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
                auto pos = nonterminal.find(top);
                if (pos != nonterminal.end())
                {
                    bool flag = false;
                    for (auto&& i : pos->second.symbol)
                    {
                        for (auto&& j : i)
                        {
                            if (terminal.find(j) != terminal.end())
                            {
                                if (strncmp(j.data(), nc, j.size()) == 0)
                                {
                                    for (auto k = i.rbegin(); k != i.rend(); ++k)
                                    {
                                        st.push(*k);
                                    }
                                    flag = true;
                                    goto _next;
                                }
                                else
                                    break;
                            }
                            else
                            {
                                for (auto&& m : firstSet[j])
                                {
                                    if (strncmp(m.data(), nc, m.size()) == 0)
                                    {
                                        for (auto k = i.rbegin(); k != i.rend(); ++k)
                                        {
                                            st.push(*k);
                                        }
                                        flag = true;
                                        goto _next;
                                    }
                                }
                                auto&& tmp = firstSet[j];
                                if (tmp.find(NULLCHAR) != tmp.end())
                                {
                                    continue;
                                }
                                else
                                    break;
                            }
                        }
                    }
                _next:
                    if (not flag)
                    {
                        flag = false;
                        if (firstSet[pos->first].find(NULLCHAR) != firstSet[pos->first].end())
                        {
                            for (auto&& m : followSet[pos->first])
                            {
                                if (strncmp(m.data(), nc, m.size()) == 0)
                                {
                                    flag = true;
                                    break;
                                }
                            }
                        }
                        if (not flag)
                            error();
                    }
                }
                else
                    error();
            }
        }
        delete[] buf;
        return false;
    }
};
} // namespace Parser

#endif