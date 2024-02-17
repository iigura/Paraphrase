# 配列
Paraphrase のプログラミングにおいては、
これまでに説明した型の値が
スタック上によく積まれます。
この章からは、これらの値を複数格納する
配列やリスト等について順次説明を行っていきます。

ここでは配列について説明を行います。

## 配列の作成
配列の作成は new-array または new-array-with ワードにて
行います。
new-array では TOS にある整数値取り出し、
その数の要素を持つ配列を生成しスタックに積みます。
生成された配列の個々の要素の初期値は未定です。
new-array-with では、セカンドと TOS より値を取り出し、
セカンドにある整数値を配列の要素数とし、
TOS にある値を配列の各要素の初期値とします。
その後、生成した配列をスタックに積みます。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref new-array]> , <[ref new-array-with]> 
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
new-array : I --- A <[br]>
new-array-with : I X --- A
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では 3 個の要素からなり、
0.0 という値で初期化された配列を生成しています。
<[code-begin]>
&gt; 3 0.0 new-array-with .
  array (size=3 data=0x7fafc6e005e8)
  [0]= 0.000000 [1]= 0.000000 [2]= 0.000000 ok.
<[code-end]>
なお表示の関係上画面への出力が複数行になっていますが、
環境によっては 1 行で表示される場合もあります。
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状態が想定されているものと異なる場合は
エラーが発生します。
<[/ddt]>
<[dl-table-hr-end]>



