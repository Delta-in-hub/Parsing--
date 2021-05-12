# Parser--
Simple Header Only Parser of LL1 /
## Grammar form
```
;left recursion     Any character behind ; will be ignored.
; <NonTerminal> ::= symbols are separated by blank. !!!Remember this!!!
; Those symbols that never appear on the left of ::= will be recognized as <Terminal>
Expr ::= Expr + Term | Expr - Term | Term  ;You can also write comments like this.
Term ::= Term * Factor | Term / Factor | Factor
Factor ::= ( Expr ) | num | name
;E ::= E b | a
```
## LL1
```c++
    constructGrammar(filePath);
    findAllTerminal();
    removeLeftRecursion();
    getFirst();
    getFollow();
    constructTable();
```