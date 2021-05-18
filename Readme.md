# Parser--
Simple Header Only Parser of LL1 / LR1
## Grammar form
```
;left recursion     Any character behind ; will be ignored.
; <NonTerminal> ::= symbols are separated by blank. !!!Remember this!!!
; Those symbols that never appear on the left of ::= will be recognized as <Terminal>
; write grammars of one <NonTerminal> in one line.
; use | to separate porductions of one <NonTerminal>
Expr ::= Expr + Term | Expr - Term | Term  ;You can also write comments like this.
Term ::= Term * Factor | Term / Factor | Factor
Factor ::= ( Expr ) | num | name | <null>  ; <null> for epsilon
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


## LR1
```

```
输入的TOKEN用空格分隔
###  如何处理空产生式(epsilon)
正常生成Action表和Goto表

在判断时,若Action[state,word]不存在,则去找Action[state,epsilon]
若找到了,则移进到下个对应状态
对epsilon规约时,状态栈弹出一个,符号栈不弹.
