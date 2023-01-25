
/**
 * @file main.cpp
 * @author Mangesh Dalvi ( 112001010@smail.iitpkd.ac.in )
 * @brief
 * @date 11-12-2022
 * @copyright Copyright (c) 2022
 */

#include <bits/stdc++.h>
using namespace std;

#define int long long

class state
{
public:
    static int counter;
    int number;
    set<char> char_set;
    map<char, set<state *>> delta;
    set<state *> startStates;
    set<state *> finalStates;
    state()
    {
        this->counter++;
        this->number = counter;
    }
    void SetEdge(char c, state *state_pointer)
    {
        char_set.insert(c);
        this->delta[c].insert(state_pointer);
    }
    void addFinal(state *s)
    {
        this->finalStates.insert(s);
    }
    void addStart(state *s)
    {
        this->startStates.insert(s);
    }
};

class NFA
{

public:
    set<state *> start_states;
    set<state *> end_states;
    set<char> sigma;
    map<int, state *> numberToState;

    set<int> getStates(int stateNumber, NFA &nfa, char c)
    {
        set<int> s;
        if (nfa.numberToState[stateNumber]->delta.find(c) == nfa.numberToState[stateNumber]->delta.end())
        {
            return set<int>{};
        }
        for (auto to : nfa.numberToState[stateNumber]->delta[c])
        {
            s.insert(to->number);
        }
        return s;
    }

    state *init(string reg_expression)
    {
        auto start = this->init(reg_expression, 0);
        set<state *> finalnumbers;
        for (auto each : start->finalStates)
        {
            for (auto e : each->delta['#'])
            {
                finalnumbers.insert(e);
            }
        }
        for (auto num : finalnumbers)
        {
            start->finalStates.insert(num);
        }
        return start;
    }

    state *init(string reg_expression, int dontCare)
    {
        vector<string> temp = parse(reg_expression);

        state *state1, *state2;

        state *toReturn;

        if (temp[0] == "concat")
        {
            state1 = init(temp[1]);
            state2 = init(temp[2]);

            for (auto ele : state1->finalStates)
            {
                ele->SetEdge('#', state2);
            }
            state1->finalStates.clear();
            for (auto ele : state2->finalStates)
            {
                state1->finalStates.insert(ele);
            }
            state2->finalStates.clear();
            toReturn = state1;
        }
        else if (temp[0] == "union")
        {
            state1 = init(temp[1]);
            state2 = init(temp[2]);
            toReturn = new state;
            numberToState[state::counter] = toReturn;
            toReturn->SetEdge('#', state1);
            toReturn->SetEdge('#', state2);

            for (auto ele : state1->finalStates)
            {
                toReturn->finalStates.insert(ele);
            }

            for (auto ele : state2->finalStates)
            {
                toReturn->finalStates.insert(ele);
            }
            state1->finalStates.clear();
            state2->finalStates.clear();
        }

        else if (temp[0] == "star")
        {
            state2 = init(temp[1]);
            state1 = new state;
            numberToState[state::counter] = state1;
            state1->SetEdge('#', state2);

            for (auto ele : state2->finalStates)
            {
                ele->SetEdge('#', state2);
            }

            for (auto ele : state2->finalStates)
            {
                state1->finalStates.insert(ele);
            }

            state2->finalStates.clear();
            toReturn = state1;
        }
        else
        {
            state1 = new state;
            numberToState[state::counter] = state1;
            state2 = new state;
            numberToState[state::counter] = state2;
            state1->addFinal(state2);
            state1->SetEdge(temp[1][0], state2);
            this->sigma.insert(temp[1][0]);
            toReturn = state1;
        }
        return toReturn;
    }

    vector<string> parse(string s)
    {
        vector<string> t;
        int k = s.find('(');
        string sf = s.substr(0, k);

        t.push_back(sf);
        s.pop_back();
        string s1 = "";
        for (int i = k + 1; i < s.size(); i++)
        {
            s1 += s[i];
        }

        if (sf == "star" || sf == "symbol")
        {
            t.push_back(s1);
            return t;
        }

        int op = 0, loc = -1;
        for (int i = 0; i < s1.size(); i++)
        {
            if (s1[i] == '(')
            {
                op++;
            }
            else if (s1[i] == ')')
            {
                op--;
            }
            else if (s1[i] == ',' && op == 0)
            {
                assert(loc == -1);
                loc = i;
            }
        }
        assert(loc != -1);
        assert(op == 0);

        string cur = "";
        for (int i = 0; i < loc; i++)
            cur += s1[i];
        t.push_back(cur);

        cur = "";
        for (int i = loc + 1; i < s1.size(); i++)
            cur += s1[i];
        t.push_back(cur);
        return t;
    }
};

class DFA
{

public:
    set<char> char_set;
    set<int> startStates;
    set<set<int>> finalStates;
    map<int, set<int>> numberToState;
    map<set<int>, int> StateToNumber;

    map<set<int>, map<char, set<int>>> delta;

    int totalState;

    DFA()
    {
    }

    int parent[100001];
    int area[100001];
    void make_set()
    {
        for (int i = 0; i < 100001; i++)
        {
            parent[i] = i;
            area[i] = 1;
        }
    }
    int find_set(int root)
    {
        if (parent[root] == root)
            return root;
        return parent[root] = find_set(parent[root]);
    }
    void union_sets(int u, int v)
    {
        int p1 = find_set(u);
        int p2 = find_set(v);

        if (p1 == p2)
        {
            return;
        }

        int s1 = area[find_set(u)];
        int s2 = area[find_set(v)];

        if (s1 >= s2)
        {
            parent[p2] = p1;
            area[p1] += area[p2];
        }
        else
        {
            parent[p1] = p2;
            area[p2] += area[p1];
        }
    }

    DFA(DFA &dfa1, DFA &dfa2)
    {
        this->char_set = dfa1.char_set;
        int n = dfa1.delta.size();
        int m = dfa2.delta.size();

        for (int i = 1; i <= n; i++)
        {
            for (int j = 1; j <= m; j++)
            {

                for (auto c : this->char_set)
                {
                    int x = dfa1.StateToNumber[dfa1.delta[dfa1.numberToState[i]][c]];
                    int y = dfa2.StateToNumber[dfa2.delta[dfa2.numberToState[j]][c]];

                    set<int> from;

                    for (auto ele : dfa1.numberToState[i])
                    {
                        from.insert(ele);
                    }

                    for (auto ele : dfa2.numberToState[j])
                    {
                        from.insert(ele * (-1));
                    }

                    set<int> to;

                    for (auto ele : dfa1.numberToState[x])
                    {
                        to.insert(ele);
                    }

                    for (auto ele : dfa2.numberToState[y])
                    {
                        to.insert(ele * (-1));
                    }

                    this->delta[from][c] = to;
                }
            }
        }

        for (auto ele : dfa1.startStates)
        {
            this->startStates.insert(ele);
        }

        for (auto ele : dfa2.startStates)
        {
            this->startStates.insert(ele * (-1));
        }

        for (auto f1 : dfa1.finalStates)
        {

            for (auto f2 : dfa2.finalStates)
            {
                set<int> temp;
                for (auto ele : f1)
                {
                    temp.insert(ele);
                }
                for (auto ele : f2)
                {
                    temp.insert(ele * (-1));
                }
                this->finalStates.insert(temp);
            }
        }
    }

    DFA(DFA &dfa)
    {
        this->char_set = dfa.char_set;
        this->totalState = dfa.delta.size();
        int n = this->totalState;
        int sz = n;

        vector<vector<bool>> marked(this->totalState + 10, vector<bool>(this->totalState + 10, false));

        for (int i = 1; i <= n; i++)
        {
            for (int j = i + 1; j <= n; j++)
            {
                if ((dfa.finalStates.find(dfa.numberToState[i]) != dfa.finalStates.end()) and (dfa.finalStates.find(dfa.numberToState[j]) == dfa.finalStates.end()))
                {
                    marked[i][j] = true;
                }
                else if (dfa.finalStates.find(dfa.numberToState[j]) != dfa.finalStates.end() and (dfa.finalStates.find(dfa.numberToState[i]) == dfa.finalStates.end()))
                {
                    marked[i][j] = true;
                }
            }
        }

        while (true)
        {
            bool flag = true;
            for (int i = 1; i <= n; i++)
            {
                for (int j = i + 1; j <= n; j++)
                {
                    if (marked[i][j] or marked[j][i])
                        continue;
                    for (char c : dfa.char_set)
                    {
                        int x = dfa.StateToNumber[dfa.delta[dfa.numberToState[i]][c]];
                        int y = dfa.StateToNumber[dfa.delta[dfa.numberToState[j]][c]];
                        if (marked[x][y] or marked[y][x])
                        {
                            marked[i][j] = true;
                            flag = false;
                        }
                    }
                }
            }
            if (flag)
            {
                break;
            }
        }

        make_set();

        for (int i = 1; i <= n; i++)
        {
            for (int j = i + 1; j <= n; j++)
            {
                if (marked[i][j] == false)
                {
                    union_sets(i, j);
                }
            }
        }

        map<int, set<int>> clpsd;
        for (int i = 1; i <= n; i++)
        {
            clpsd[find_set(i)].insert(i);
        }

        int oldStart = dfa.StateToNumber[dfa.startStates];

        this->startStates = clpsd[find_set(oldStart)];

        set<int> oldFinals;
        for (auto s : dfa.finalStates)
        {
            oldFinals.insert(dfa.StateToNumber[s]);
        }

        for (int s : oldFinals)
        {
            this->finalStates.insert(clpsd[find_set(s)]);
        }

        for (auto pr : dfa.delta)
        {

            for (auto c : dfa.char_set)
            {
                auto y = clpsd[find_set(dfa.StateToNumber[pr.first])];
                auto x = clpsd[find_set(dfa.StateToNumber[dfa.delta[pr.first][c]])];
                this->delta[y][c] = x;
            }
        }

        int cnt = 1;

        for (auto pr : this->delta)
        {
            this->numberToState[cnt] = pr.first;
            this->StateToNumber[pr.first] = cnt;
            cnt++;
        }
    }

    void printSet(set<int> s)
    {
        cout << "{ ";
        for (int ele : s)
            cout << ele << " ";
        cout << "}  \n";
        return;
    }

    set<int> getStates(int stateNumber, NFA &nfa, char c)
    {
        if (stateNumber == 99999)
        {
            return {99999};
        }
        set<int> s;
        if (nfa.numberToState[stateNumber]->delta.find(c) == nfa.numberToState[stateNumber]->delta.end())
        {
            return set<int>{};
        }
        for (auto to : nfa.numberToState[stateNumber]->delta[c])
        {
            s.insert(to->number);
        }
        return s;
    }

    // https://www.geeksforgeeks.org/find-distinct-subsets-given-set/
    vector<vector<int>> findPowerSet(vector<int> nums)
    {
        // Size of array to set bit
        int bits = nums.size();

        // Total number of subsets = pow(2,
        // sizeof(arr))
        unsigned int pow_set_size = pow(2, bits);

        // Sort to avoid adding permutation of
        // the substring
        sort(nums.begin(), nums.end());
        vector<vector<int>> ans;

        // To store subset as a list to
        // avoid adding exact duplicates
        vector<string> list;

        // Counter 000..0 to 111..1
        for (int counter = 0; counter < pow_set_size;
             counter++)
        {
            vector<int> subset;
            string temp = "";

            // Check for the current bit in the counter
            for (int j = 0; j < bits; j++)
            {
                if (counter & (1 << j))
                {
                    subset.push_back(nums[j]);

                    // Add special character to separate
                    // integers
                    temp += to_string(nums[j]) + '$';
                }
            }

            if (find(list.begin(), list.end(), temp) == list.end())
            {
                ans.push_back(subset);
                list.push_back(temp);
            }
        }
        return ans;
    }

    void init(state *start, NFA &nfa)
    {

        this->char_set = nfa.sigma;
        int cnt = 1;
        int total = state::counter;
        vector<int> temp;

        for (int i = 1; i <= total; i++)
        {
            temp.push_back(i);
        }

        vector<vector<int>> allSubsets = findPowerSet(temp);

        set<int> finalStateNumbers;
        for (auto e : start->finalStates)
        {
            finalStateNumbers.insert(e->number);
        }

        set<int> dead;

        dead.insert(99999);

        for (auto c : this->char_set)
        {
            this->delta[dead][c] = dead;
        }
        allSubsets.push_back(vector<int>{99999});

        for (auto each : allSubsets)
        {
            // cout << "over\n";

            set<int> temp(each.begin(), each.end());

            if (each.size() == 0)
            {
                continue;
            }

            for (auto c : this->char_set)
            {
                set<int> curState;
                for (auto cur : each)
                {
                    if (finalStateNumbers.find(cur) != finalStateNumbers.end())
                    {
                        this->finalStates.insert(temp);
                    }

                    auto s = getStates(cur, nfa, '#');
                    for (auto ele : s)
                    {
                        curState.insert(ele);
                    }
                }
                for (auto s : temp)
                {
                    curState.insert(s);
                }

                set<int> cur1;
                for (auto cur : curState)
                {
                    auto s = getStates(cur, nfa, c);
                    for (auto ele : s)
                        cur1.insert(ele);
                }

                set<int> cur2;

                for (auto cur : cur1)
                {
                    auto s = getStates(cur, nfa, '#');
                    cur2.insert(cur);
                    for (auto ele : s)
                        cur2.insert(ele);
                }

                if (cur2.size() == 0)
                {
                    cur2 = dead;
                }
                this->delta[temp][c] = cur2;
            }
            this->numberToState[cnt] = temp;
            this->StateToNumber[temp] = cnt;
            cnt++;
        }
        this->startStates = this->getStates(start->number, nfa, '#');
        this->startStates.insert(start->number);
    }

    void MakeComplement()
    {
        set<set<int>> temp;
        for (auto pr : this->delta)
        {
            if (this->finalStates.find(pr.first) == this->finalStates.end())
            {
                temp.insert(pr.first);
            }
        }
        this->finalStates = temp;
    }
};

int state::counter = 0;

state *start, *start1;

map<pair<state *, string>, bool> visited;

bool result = false;

bool check(state *st, string s)
{

    bool res = false;

    if (visited.find({st, s}) == visited.end())
    {
        if (start->finalStates.find(st) != start->finalStates.end() and (s.size() == 0))
        {
            result = true;
            return true;
        }

        visited[{st, s}] = true;

        if (s.size() == 0)
        {
            return false;
        }

        for (auto e : st->delta[s[0]])
        {
            res = res | check(e, s.substr(1));
        }

        for (auto e : st->delta['#'])
        {
            res = res | check(e, s);
        }
    }

    return res;
}

map<pair<set<int>, string>, bool> visted1;

bool dfaResult = false;

void printSet(set<int> s)
{
    cout << "{ ";
    for (int ele : s)
        cout << ele << " ";
    cout << "}  \n";
    return;
}

bool dfacheck(set<int> st, string s, DFA &dfa)
{

    if ((dfa.finalStates.find(st) != dfa.finalStates.end()) and (s.size() == 0))
    {
        return false;
    }

    if (s.size() == 0 or st.size() == 0)
    {
        return true;
    }

    if (dfa.delta[st].find(s[0]) == dfa.delta[st].end())
    {
        return true;
    }

    return dfacheck(dfa.delta[st][s[0]], s.substr(1), dfa);
}

void print(NFA &obj)
{

    cout << "\n \n I am printing NFA \n \n";
    state *cur = start;
    for (int i = 1; i <= state::counter; i++)
    {

        cout << "New -> " << i << endl;

        state *s = obj.numberToState[i];

        for (auto ele : s->delta)
        {
            cout << ele.first << " ";
            for (auto e : ele.second)
            {
                cout << e->number << " ";
            }
            cout << endl;
        }
    }
}

void print(DFA &dfa)
{

    cout << "\n \n I am printing DFA \n \n";

    for (auto ele : dfa.delta)
    {

        cout << "\n new state \n";

        printSet(ele.first);

        cout << endl;

        for (auto e : ele.second)
        {

            cout << e.first << " ";
            printSet(e.second);
        }

        cout << endl;
    }
}

string makeString(string &e)
{

    int n = e.size();

    string a = "concat(";

    string ans = a;

    int cnt = 0;

    for (char c : e)
    {
        ans += "symbol(";
        ans.push_back(c);
        ans.push_back(')');
        if (cnt < n - 2)
        {
            ans.push_back(',');
            ans += a;
        }
        else if (cnt == n - 1)
        {
            ans.push_back(')');
        }
        else
        {
            ans.push_back(',');
        }
        cnt++;
    }

    for (int i = 0; i < n - 2; i++)
    {
        ans.push_back(')');
    }

    return ans;
}

signed main()
{

    string line;
    string RegularExpr, String;
    getline(cin, line);

    int testCases = stoi(line);

    cout << "Number of Test Cases :  " << testCases << endl;

    int cnt = 1;

    while (testCases--)
    {

        cout << "Test Case :  " << cnt << "\n\n";

        state::counter = 0;
        start = NULL;
        start1 = NULL;
        visted1.clear();
        visited.clear();
        dfaResult = false;
        result = false;

        getline(cin, RegularExpr);
        getline(cin, String);

        cout << "Given regular expression :   " << RegularExpr << endl;
        cout << "Input string             :   " << String << endl;

        string s;

        // Stage 2

        s = makeString(String);

        state::counter = 0;

        NFA nfaOfString;

        start = nfaOfString.init(s);

        DFA dfaOfString;

        dfaOfString.init(start, nfaOfString);

        state::counter = 0;

        NFA nfaOfRegExpr;

        start1 = nfaOfRegExpr.init(RegularExpr);

        DFA dfaOfRegExpr;

        dfaOfRegExpr.init(start1, nfaOfRegExpr);

        cout << "Checking ... \n";

        // bool res = dfacheck(dfaOfRegExpr.startStates, String, dfaOfRegExpr);

        dfaOfRegExpr.MakeComplement();

        DFA dfa1(dfaOfRegExpr);
        DFA dfa2(dfaOfString);

        // Stage 3

        DFA dfaFinal(dfa1, dfa2);

        // Stage 4

        // cout << dfacheck(dfaFinal.startStates, String, dfaFinal) << endl;

        bool res = dfacheck(dfaFinal.startStates, String, dfaFinal);

        if (res)
        {
            cout << "\n\nYes\n\n";
        }
        else
        {
            cout << "\n\nNo\n\n";
        }

        cnt++;
    }
    return 0;
}