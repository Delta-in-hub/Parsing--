# Parser--
Simple Header Only Parser of LL1 /
## Grammar form
```
;left recursion     Any character behind ; will be ignored.
; <NonTerminal> ::= symbols are separated by blank. !!!Remember this!!!
; Those symbols that never appear on the left of ::= will be recognized as <Terminal>
; write grammars of one <NonTerminal> in one line.
; use | to separate porductions of one <NonTerminal>
Expr ::= Expr + Term | Expr - Term | Term  ;You can also write comments like this.
Term ::= Term * Factor | Term / Factor | Factor
Factor ::= ( Expr ) | num | name
```
## LL1
```
    constructGrammar(filePath);
    findAllTerminal();
    removeLeftRecursion();
    getFirst();
    getFollow();
    constructTable();
```