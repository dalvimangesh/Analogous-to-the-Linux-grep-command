# Theory of Computation
Analogous to the Linux grep command

## Name : Mangesh Dalvi

## Roll No : 112001010

#### run command -> ``` make build ``` or ``` make ```

#### Input is taken from tests.txt file
For giving custom input modify tests.txt as follow : 

##### On first line write number of test cases

##### For each test 

    Regular Expression 
    then String

##### For given test cases : 
    test1 takes less than 2 second
    test2 takes less than 2 second
    test3 takes less than 2 second
    test4 takes almost 2 minutes
    test5 takes less than 20 second

NFA class : 
    ```state *init(string reg_expression)``` used to create regular expression to NFA
    ```vector<string> parse(string s)``` used for parsing

DFA class :
    constructor ```DFA(DFA &dfa1, DFA &dfa2)``` used to find DFA using product construction.
    constructor ```DFA(DFA &dfa``` used to find minimum of DFA
    ```void init(state *start, NFA &nfa)``` used to find DFA from NFA
    ```void MakeComplement()``` used to create complement of DFA

```bool dfacheck(set<int> st, string s, DFA &dfa)``` is used to check validity of the final DFA

Reference:
    for finding subsets
    https://www.geeksforgeeks.org/find-distinct-subsets-given-set/

#### Thanks
