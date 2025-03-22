# 配列
Paraphrase のプログラミングにおいては、
これまでに説明した型の値が
スタック上によく積まれます。
この章以降しばらくは、これらの値を複数格納する
配列やリスト、連想配列について順次説明を行っていきます。

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

## 配列の要素数ならびにインデックス値の確認
配列要素への代入や、配列要素の値の取得には
それら要素を指定するインデックス値を用います。
配列の場合、
生成時に指定した要素数から変化することはありませんが、
@valid-index&quest; を用いると動的に指定するインデックス値が
有効であるか否かを確認できます。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
  <[ddt]><[gt]>
    <[ref ref_is_valid-index @valid-index&quest;]> 
  <[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
A I --- A I B または R I --- R I B
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では要素数 5 の配列に対し、
3 が有効なインデックス値であることを確認し、
-1 が無効なインデックス値であることを確認します。
<[code-begin]>
&gt; 5 new-array
&nbsp; ok.
&gt; 3 @valid-index? .
&nbsp; true ok.
&gt; -1 @valid-index? .
&nbsp; false ok.
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

また、TOS に格納されている配列の要素数を調べるワードとして
size や @size があります。
これらのワードは配列だけではなくリストや連想配列、
文字列やシンボルにも対応しています。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
  <[ddt]><[gt]>
    <[ref size]> , <[ref ref_size  @size]> 
  <[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
size : &lcurly;
        A &verbar; R &verbar; H &verbar; S &verbar; T
       &rcurly;
        --- I <[br]>
@size: &lcurly;
        A &verbar; R &verbar; H &verbar; S &verbar; T 
       &rcurly;
        --- &lcurly;
                A &verbar; R &verbar; H &verbar; S &verbar; T
            &rcurly; I <[br]>
※ @size の実行後のスタックの状態は、
@size の実行前に TOS に存在していいた値がそのままセカンドの
値として残ります。
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では要素数 5 の配列に対し、
その配列の要素数（ここでは 5）を size ワードを用いて取得し、
表示しています。
<[code-begin]>
&gt; 5 new-array
  ok.
&gt; size .
  5 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状態が想定されているものと異なる場合は
エラーが発生します。
<[/ddt]>
<[dl-table-hr-end]>

## 配列要素への代入
配列内の各要素への値の代入は set または @set にて行われます。
なお、これらのワードは配列のみならず、
リストや連想配列にも使用することができます。

配列の i 番目の要素に x という値を代入する場合は、
TOS に代入先の配列を積んでおき、i x set とします。
例えば、新しく12 の要素からなる配列を生成し、
その 2 番目の要素（Paraphrase では
配列のインデックス値は 0 オリジンであるため、
インデックスの値としては 1 となります）に
28 を代入する場合は次のようなコードとなります：
<[code-begin]>
&gt; 12 new-array 1 28 set
<[code-end]>
配列に対する set ワードのスタックの変化は A I X --- であり、
@set は A I X --- A となります。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
  <[ddt]><[gt]>
    <[ref set]> , <[ref ref_set @set]> 
  <[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
set  : A I X --- , R I X --- , H Xk Xv --- <[br]>
@set : A I X --- A , R I X --- R , H Xk Xv --- H <[br]>
整数値または Xk にてインデックス値を指定します。
X または Xv は代入する値を意味します。
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では要素数 5 個の配列を生成し、
その配列の 0 番目の要素に 123 という値を代入し、
1 番目の要素に "hello" という文字列を代入し、
最後に配列の値を表示しています。
<[code-begin]>
&gt; 5 new-array
&nbsp; ok.
&gt; 0 123 @set
&nbsp; ok.
&gt; 1 "hello" @set
&nbsp; ok.
&gt; .
  array (size=5 data=0x7fc4e4d0d7b8)
  [0]= 123 [1]= hello
  [2]= (invalid-value) [3]= (invalid-value)
  [4]= (invalid-value)
  ok.
<[code-end]>
なお表示の関係上画面への出力が複数行になっていますが、
環境によっては 1 行で表示される場合もあります。
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
セカンドで指定したインデックス値が存在しない場合は
エラーとなります。
また、スタックの状態が想定されているものと異なる場合は
エラーが発生します。
<[/ddt]>
<[dl-table-hr-end]>

## 配列要素の値の取得
配列に格納されている要素の値を得るためには
get または @get を用います。
配列の場合は get ワードの場合、
A I --- X というスタックの状態となります。
セカンドに格納されている配列に対し、
TOS にある整数値をインデックス値とみなし、
そのインデックス値に対応する配列の値をスタックに積みます。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
  <[ddt]><[gt]>
    <[ref get]> , <[ref ref_get @get]> 
  <[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
get  : A I --- X , R I --- X , H Xk --- Xv <[br]>
@get : A I --- A X , R I --- R X , H Xk --- H Xv <[br]>
整数値または Xk にてインデックス値を指定します。
X または Xv は得られた値を意味します。
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では要素数 5 個の配列を生成し、
その配列の 0 番目の要素に 123 という値を代入し、
1 番目の要素に "hello" という文字列を代入し、
最後に 1 番目の要素の値（つまり "hello" という文字列）を
表示しています。
<[code-begin]>
&gt; 5 new-array
&nbsp; ok.
&gt; 0 123 @set
&nbsp; ok.
&gt; 1 "hello" @set
&nbsp; ok.
&gt; 1 get .
&nbsp; hello  ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
セカンドで指定したインデックス値が存在しない場合は
エラーとなります。
また、スタックの状態が想定されているものと異なる場合は
エラーが発生します。
<[/ddt]>
<[dl-table-hr-end]>

get や @get は配列に格納されている任意の要素を
取り出すワードですが、最後の要素の取り出しに特化した
last および @last というワードも存在します。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
  <[ddt]><[gt]>
    <[ref last]> , <[ref ref_last @last]> 
  <[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
last  : A --- X , R --- X <[br]>
@last : A --- A X , R --- R X 
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
get の例と同様に、要素数 5 個の配列を生成し、
その配列の最後の要素である 4 番目の要素に 999 という値を代入し、
last ワードによりその値を取得し表示しています。
<[code-begin]>
&gt; 5 new-array
&nbsp; ok.
&gt; 4 999 @set
&nbsp; ok.
&gt; last .
&nbsp; 999 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
スタックの状態が想定されているものでない場合はエラーとなります。
<[ddt]>
<[/ddt]>
<[dl-table-hr-end]>

