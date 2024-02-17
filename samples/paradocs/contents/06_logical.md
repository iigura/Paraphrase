# 論理値と論理演算
## 論理値
真なる値を直接スタックに積むワードとして
true があります。
同様にワード false は偽なる値をスタックに積みます。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref true]> , <[ref false]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> --- B <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
<[code-begin]>
&gt; true .
  true ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
エラーは発生しません。
<[/ddt]>
<[dl-table-hr-end]>

## 論理演算
ワード &amp;&amp; および &verbar;&verbar; により、
TOS とセカンドより真偽値を取り出し、
論理積や論理和、排他的論理和を計算できます。
真偽値の反転 not については、TOS より値を一つ取り出し、
その値について反転演算を行います。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref simple_and &amp;&amp;]> ,
<[ref simple_or &verbar;&verbar;]> , <[ref xor]> , <[ref not]>
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
not 以外：B B --- B <[br]>
not : B --- B
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
<[code-begin]>
&gt; true false &amp;&amp; .
  false ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状態が想定されているものと異なる場合は
エラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

TOS とセカンドだけでなく、とある処理の結果について
それらの論理積や論理和を調べたい場合もあるかと思います。
例えばいくつかある処理全てが真である場合に限り、
特定の処理を行いたい場合などです。

このような処理を簡単に行うため、
名前が与えられていない一連の手続き
（これは無名ワードやラムダと名付けられており、
別途詳しく説明します）を
格納したリスト（こちらも別途詳しく説明します）に対し、
それらの処理の結果が全て真であるか（論理積の結果）、
またはどれか一つが真であるか（論理和の結果）を計算する
ワードとして and と or が用意されています。

and と or はリスト中の全ての無名ワードを実行しません。
いずれのワードもリストの先頭より無名ワードのを
順次実行していきますが、
and の場合は、どれか一つでも false の値であれば
全体の論理積の結果は false となることが確定しますので、
それ以降の無名ワードは実行されません。
or の場合も同様で、どれか一つでも true となれば、
それ以降の無名ワードは実行されません。

以下の説明では、使用例を掲載していますが、
無名ワードやリストの使用方法の説明をご覧になってから
再度、使用例を読んでみていただければ幸いです。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]> <[ref and]> , <[ref or]> <[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> R --- B <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
i次の例では、TOS の値（この例では 5）が
3 より大きく、かつ 10 未満であるかどうかを確認しています。
<[code-begin]>
&gt; 5 ( { dup 3 > } { 10 < } ) and .
  true ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状態が想定されているものと異なる場合は
エラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

