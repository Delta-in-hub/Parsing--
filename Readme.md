# Parser--
Simple Header Only Parser of LL1 / LR1
## Grammar form
```
; Any character behind ; will be ignored.
; <NonTerminal> ::= symbols are separated by blank. !!!Remember this!!!
; Those symbols that never appear on the left of ::= will be recognized as <Terminal>
; write grammars of one <NonTerminal> in one line.
; use | to separate porductions of one <NonTerminal>
; Start symbol must be the first one of all <Nonterminal>'s production.
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

使用了列表初始化初始化构造tuple
故编译器不支持 [N4387](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4387) 会报错
或者都换成`make_tuple()`
```

```
输入的TOKEN用空格分隔
###  如何处理空产生式(epsilon)
正常生成Action表和Goto表

在判断时,若Action[state,word]不存在,则去找Action[state,epsilon]
若找到了,则移进到下个对应状态
对epsilon规约时,状态栈弹出一个,符号栈不弹.


### 简单验证
https://cyberzhg.github.io/toolbox/lr1
```
Expr -> Expr + Term | Expr - Term | Term
Term -> Term * Factor | Term / Factor | Factor
Factor -> ( Expr ) | Num | Id
Num ->  0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
Id ->  a | b | x | y
```


我的LR1
```
Expr ::= Expr + Term | Expr - Term | Term
Term ::= Term * Factor | Term / Factor | Factor
Factor ::= ( Expr ) | Num | Id
Num ::= 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
Id ::= a | b | x | y
```

可以看到他共生成60个项集,我也生成了60个项集
它生成的Goto表有43个,我生成的也有43个

项共有1417个,太多了,不数了