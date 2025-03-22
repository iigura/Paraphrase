# リスト
複数の値を格納できる、いわゆるコンテナと呼ばれる値について、
配列、連想配列と説明を行ってきました。
この章では残るもうひとつのコンテナ型であるリストについて
説明を行います。

## リストの作成
### 空リストの作成
最も単純なリストは何も要素を含まない空リストではないでしょうか。
まずはスタックに空のリストを積むワード '()' を紹介します。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref empty_list &lparen;&rparen;]>
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> --- R <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
<[code-begin]>
&gt; () show
      +----------+
TOS--&gt;| list ( ) |
DS:------------------
 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
エラーは発生しません。
<[/ddt]>
<[dl-table-hr-end]>

### ワード '(' と ')' によるリストの作成
空のリストではなく、
要素を含むリストも勿論作成できます。
そしてこのようなリストの作成方法には様々なものがあります。
もっとも直感的だと筆者が感じている方法は、
ワード '(' と ')' を用いる方法です。

<[code-begin]>
( 11 22 33 )
<[code-end]>
と Paraphrase の言語処理系に入力すれば、
最初の要素が 11 という整数値で、
2 番目の要素が 22 という整数値、
3 番目の要素が 33 という整数値から成る
要素数 3 のリストが生成されます。
もちろん、インタプリタモードで上記のプログラム片を入力すれば
スタックに当該のリストが積まれますし、
コンパイルモードであれば、実行に際して当該のリストが
スタックに積まれるようなコードがコンパイルされることとなります。

#### 即時実行レベル 2
ワード '(' および ')' は、即時実行レベル 2 のワードです。
これは '(' で始まり ')' で終わる間に入力された文字列を、
文字列として処理するため、このような実行レベルに設定されています。
<[code-begin]>
( apple banana cherry durian )
<[code-end]>
として要素数 4 のリストを構築する場合、
apple や banana といった文字列はシンボルとして取り扱われます。

同様に
<[code-begin]>
( dup swap ; )
<[code-end]>
といったリストを構築する場合においても、
標準実行レベル（内部的には実行レベル 0）のワードである
dup や swap はもちろんのこと、
即時実行レベル 1 であるワード ; 、つまりコンパイル中であっても
実行されるべきワード ; においても、リストの要素として
シンボルとして取り扱われなければなりません。

実際に上記のリストを構築してワード show にて確認してみると、
次のような結果が得られます。
<[code-begin]>
&gt; ( dup swap ; ) show
      +---------------------+
TOS--&gt;| list ( dup swap ; ) |
DS:-----------------------------
 ok.
<[code-end]>

ちなみにリストの要素として dup や swap といったワードを、
ワードとして（つまりワードを表す値として）格納したい場合は、
後ほど説明するワード &lcurly;&lcurly; と &rcurly;&rcurly;, を用いて
次のように記述する必要があります。
下の例はワード dup を要素に持つリストを構築する例です：
<[code-begin]>
&gt; ( {{ 'dup' &gt;word }}, ) show
      +----------------------------------+
TOS--&gt;| list ( 0x600003a708f0[std:dup] ) |
DS:------------------------------------------
 ok.
<[code-end]>
このように記述すれば、ワードを要素に持つリストの構築も可能です。
dup は標準実行モードのワードですが、即時実行レベル 1 のワードである ; でも同様です。

話を戻しましょう。
このように、たとえ要素として即時実行属性レベル 1 のワード名が
入力されても、それらは単なるシンボルとして取り扱われなければなりません。
そのためリスト構築中は実行レベルの閾値を 2 に引き上げ、
実行レベル 1 であるワード ; においても、
実行されることなく単なるシンボルとして取り扱われるようになっています。

そのため、当該リストの構築を終了するワード ')' においては
必然的に即時実行レベル 2 のワードである必要があります
（そうでなければ当該リストの構築を終了させる処理が起動できません）。

また、リストはリストの要素になることが可能です。
<[code-begin]>
( Hirame ( Aka-ebi arrived-today! ) Maguro )
<[code-end]>
等といったリストの構築にもワード '(' は対応しなくてはなりません。
そのため、ワード '(' も実行レベル 2 のワードとして設定されています。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref begin_list &lparen;]> <[ref end_list &rparen;]>
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>即時実行レベル 2<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
&lparen; : RS --- &omega; &theta; <[br]>
&rparen; : RS &omega; &theta; ---
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
<[code-begin]>
&gt; ( xevi 4 ) .
 ( xevi 4 ) ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
ワード '&lparen;' に関してはエラーは発生しません。
一方 '&rparen;' については、スタックが想定された状態でない場合、
エラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

### 内包表記的なリストの作成（動的リスト生成）
Python ではリストを定義する際に、
リストの要素をひとつひとつ明示的に記す代わりに
それらの要素の計算方法を示すことにより
リストを構築することもできます。
例えば [ i for i in range(10) ] 等といった記述方法です。
Python では、これらは「リスト内包表記」と呼ばれています。

Paraphase でも同様の事を実現するワードが用意されています。
次の例はワード '&lparen;&lt;' と '&gt;&rparen;' を用いて、
1 から 9 までの整数から成るリストを生成し、スタックに積み、
そのリストを表示する例です：
<[code-begin]>
&gt; (&lt; 1 10 for+ i next &gt;) .
 ( 1 2 3 4 5 6 7 8 9 ) ok.
<[code-end]>
まだ説明していない for+ や i、next といったワードが登場していますが、
1 10 for+ i next というプログラムは 1 から 10 未満の整数（つまり 9 まで）を
生成してスタックに積む処理を行います。

ワード '&lparen;&lt;' は実行された時のスタックの深さを記録しておき、
'&gt;&rparen;' では、記録されたスタックの深さよりも増えている部分の値を用いて
リストを生成します。
結果として、Python のリスト内包表記のような記述にてリストの生成が可能となります。
なお Paraphrase では、このようなリストの構築方法を「動的リスト生成」などとも呼びます。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref begin_dynamic_list &lparen;&lt;]> , <[ref end_dynamic_list &gt;&rparen;]>
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>即時実行レベル 2<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
&lparen;&lt; : SS --- &omega; &theta; I <[br]>
&gt;&rparen; : SS &omega; &theta; I --- <[br]>
なお TOS の I はワード '&lparen;&lt;' 実行時のデータスタックの深さを表す値です。
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
<[code-begin]>
&gt; ( dyn list (< 1 5 for+ i next >) gen ) .
( dyn list ( 1 2 3 4 ) gen ) ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
ワード '&lparen;&lt;' に関してはエラーは発生しません。
一方 '&gt;&rparen;' については、スタックが想定された状態でない場合、
エラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

### 単一要素からなるリストの作成
TOS の値を取り出し、その値のみから構成されるリストを生成するワードも存在します。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref en-list]>
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> X --- R <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
<[code-begin]>
&gt; "hello" en-list .
 ( "hello" ) ok.
&gt; `hello en-list .
 ( hello ) ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックが空の場合はエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

### 2 つの要素からなるリストの作成
TOS およびセカンドから値を取り出し、それらの要素からなるリストを生成するワード tuple も存在します。
リストに格納される順序は、セカンドが第 1 要素となり TOS が第 2 要素となります。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref tuple]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> X X --- R <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
<[code-begin]>
&gt; 11 22 tuple .
 ( 11 22 ) ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックに 2 つの要素が存在しない場合はエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

## リストの要素数
TOS にあるリストの要素数を調べるには、
文字列のところで紹介したワード size を利用できます。

要素数 0 のリストを <[ref 空のリスト]> と呼ぶことにします。
TOS にあるリストが空のリストであるか否かは次に示すワードで調査できます。
empty-list&quest; は TOS よりリストを取り出し、
そのリストが空のリストであった場合は true を、
さもなければ false をスタックに積みます。
not-empty-list&quest; は逆に TOS より取り出したリストが
空のリストであった場合には false を、
空のリストでなかった場合には true をスタックに積みます。

@empty-list&quest; および @not-empty-list&quest; は、
それぞれ empty-list&quest; 、not-empty-list&quest; の
参照型です。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref is_empty-list empty-list&quest;]> , <[ref is_not-empty-list not-empty-list&quest;]> , 
<[ref ref_is_empty-list @empty-list&quest;]> , <[ref ref_is_not-empty-list @not-empty-list&quest;]>
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
empty-list&quest; , not-empty-list&quest; : R --- B <[br]>
@empty-list&quest; , @not-empty-list&quest; : R --- R B <[br]>
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
<[code-begin]>
&gt; ( 11 22 33 ) empty-list&quest; .
 false ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状況が想定されたものでない場合はエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

## 要素の追加と削除
リストへの要素の追加は append および push-back にて行うことができます。
それぞれリストの末尾に要素を追加するワードですが、
想定されるスタックの状況が異なります。
append は swap push-back と同値です。
同様に push-back は swap append と同値となります。
それぞれに参照型が存在します。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref append]> , <[ref push-back]> , 
<[ref ref_append @append]> , <[ref ref_push-back @push-back]>
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
append : R X --- <[br]>
push-back : X R --- <[br]>
@append : R X --- R <[br]>
@push-back : X R --- R
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
<[code-begin]>
&gt; ( 11 22 33 ) 44 @append .
 ( 11 22 33 44 ) ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状況が想定されたものでない場合はエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

## リストの合成
ワード concat を用いるとセカンドと TOS よりそれぞれリストを取り出し、
これら 2 つのリストを繋げたリストを作成し、それをスタックに積みます。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]> <[ref concat]> <[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> R R --- R <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
<[code-begin]>
&gt; ( 11 22 33 ) ( 44 55 ) concat .
 ( 11 22 33 44 55 ) ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状況が想定されたものでない場合はエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

ワード zip は TOS およびセカンドからリスト L1 および L2 を取り出し、
L1 の要素と L2 の要素から構成される 2 つ組のリスト（タプル）を生成し、
それらから構成されるリストをスタックに積みます。

リスト L1 と L2 の要素数は同一でないとエラーとなります。
2 つの空リストに対し、本ワードを使用すると空リストをスタックに積みます。 
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref zip]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> R R --- R <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
<[code-begin]>
&gt; ( ZOP AH SHEO ) ( 0 1 2 ) zip .
 ( ( ZOP 0 ) ( AH 1 ) ( SHEO 2 ) ) ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状況が想定されたものでない場合はエラーとなります。
TOS とセカンドに積まれているリストの要素数が異なる場合もエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

product を用いると TOS とセカンドに格納されているリストの直積が得られます。
どちらのリストが空の場合、product で得られるリストも空となります。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref product]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> R R --- R <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
<[code-begin]>
&gt; ( A B ) ( 1 2 3 ) product .
 ( ( A 1 ) ( A 2 ) ( A 3 ) ( B 1 ) ( B 2 ) ( B 3 ) ) ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状況が想定されたものでない場合はエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

## リストの分割
リストの合成とは逆に、リストを分割するワードも存在しています。

split は分割する要素の場所を指定して対象のリストを分割します。
slice は対象のリストから部分リストを取り出すワードです。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref split]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> R I --- R1 R2 <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
split はセカンドにあるリスト R と TOS にある整数値 I をスタックから取り出し、
TOS にある整数値 I を用いて R 分割し、分割されたリスト R1 と R2 をスタックに積みます。
R1 は 0 から I-1 番目までの要素からなるリストとなり、
R2 の最初の要素は、元のリスト R の I 番目の要素以降のものとなります。
なお、リストの要素の数え方は先頭が 0 番目となる 0 オリジンです。

TOS の値が 0 の場合は、R1 は空のリストとなり、
R2 は R と等しいリストとなります。
<[code-begin]>
 &gt; clear ( 1 2 3 4 5 ) 2 split show
      +----------------+
TOS--&gt;| list ( 3 4 5 ) |
      |  list ( 1 2 )  |
DS:------------------------
 ok.
 &gt; clear ( 1 2 3 4 5 ) 0 split show
      +--------------------+
TOS--&gt;| list ( 1 2 3 4 5 ) |
      |      list ( )      |
DS:----------------------------
 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状況が想定されたものでない場合はエラーとなります。
また TOS の値が負の場合もエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref slice]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> R I1 I2 --- R1 R2 <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>

slice はスタックより整数値 I2 と I1 およびリスト R を取り出し、
R の I1 番目の要素から I2 個の要素を取り出し R1 とします。
残った要素を R2 とし、R1 と R2 をスタックに積みます。
なおこのワードでも、リストの要素の数え方は先頭が 0 番目となる 0 オリジンです。
<[code-begin]>
 &gt; clear  ( 1 2 3 4 5 6 7 ) 2 3 slice show
      +------------------+
TOS--&gt;|  list ( 3 4 5 )  |
      | list ( 1 2 6 7 ) |
DS:--------------------------
 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状況が想定されたものでない場合はエラーとなります。
また I1 や I2  の値が負の場合もエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

## リスト内の要素取得
リストに保存されている要素を取り出すために様々なワードが用意されています。
expand はリストに格納されている要素を全て取り出し、スタックに順次積んでゆくワードです。
@expand は expand の参照形です。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref expand]> , <[ref ref_expand @expand]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
expand: R --- リストの中身に応じた値が何個か積まれます <[br]>
@expand: R --- R リストの中身に応じた値が何個か積まれます <[br]>
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
<[code-begin]>
 &gt; clear ( 11 22 ( 33 44 55 ) 66 ) expand show
      +-------------------+
TOS--&gt;|      int 66       |
      | list ( 33 44 55 ) |
      |      int 22       |
      |      int 11       |
DS:---------------------------
 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状況が想定されたものでない場合はエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

リストの先頭のみを取り出すワードや、
2 番目の要素をリストとして取り出すワードがあります。
それぞれ car と cdr というワードです。
これらのワードは Lisp からの輸入です。
名前の由来については Lisp の歴史を調べてみて下さい。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref car]> , <[ref ref_car @car]> ,
<[ref cdr]> , <[ref ref_cdr @cdr]>
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
car : R --- X <[br]>
@car : R --- R X <[br]>
cdr : R --- R <[br]>
@cdr : R --- R R
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
car はリストの先頭の要素を取り出し、
cdr はリストの先頭の要素を取り除いたリストをスタックに積みます。
なお、要素を 1 つしか含まないリストに対し cdr を実行した場合、
空のリストがスタックに積まれます。
<[code-begin]>
 &gt; ( 11 22 33 ) car .
 11 ok.
 &gt; ( apple banana cherry ) cdr .
 ( banana cherry ) ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状況が想定されたものでない場合はエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

car と似たような動作をするワードに pop-front というものがあります。
car は元のリストの先頭の要素をスタックに積むのに対し、
pop-front は対象のリストから先頭の要素を取り出した後、
その取り出した値をスタックに積みます。

そのため、dup した場合や局所変数に格納されているリストを対象にする場合は、
複製元のリストや局所変数に格納されているリストに影響を与えますので注意して下さい
（このような影響を避けたい場合は clone などを活用して下さい）。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref pop-front]> , <[ref ref_pop-front @pop-front]>
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
pop-front : R --- X <[br]>
@pop-front : R --- R X
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例は pop-front と car の違いを示すものです。
複製の元となったリストへの影響に注意して下さい。
<[code-begin]>
 &gt; clear ( 11 22 33 ) dup pop-front show
      +----------------+
TOS--&gt;|     int 11     |
      | list ( 22 33 ) |
DS:------------------------
 ok.
 &gt; clear ( 11 22 33 ) dup car show
      +-------------------+
TOS--&gt;|      int 11       |
      | list ( 11 22 33 ) |
DS:---------------------------
 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状況が想定されたものでない場合はエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

## リスト内の要素の確認
スタック上のリストに、とある要素が含まれているか否かを調べるワードも存在します。
この節では、これらのワードについて説明していきます。

has? と in? というワードは、対象のリスト R に要素 X が
含まれているか否かを確認します。
含まれている場合は true をスタックに積み、
さもなければ false をスタックに積みます。

has? と in? は前提とするスタックの状態が異なるだけです。
対象とするリストを R とし、
対象の要素を X とすると、
has? は X R --- B となり、
in? は R X --- B となります。
つまり、has? は swap in? と同値であり、
同様に in? は swap has? と同値です。

なお、それぞれのワードには参照型が用意されています。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref hasp has&quest;]> , <[ref ref_hasp @has&quest;]> ,
<[ref inp in&quest;]> , <[ref ref_inp @in&quest;]>
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
has&quest; : X R --- B <[br]>
@has&quest; : X R --- X R B <[br]>
in&quest; : R X --- B <[br]>
@in&quest; : R X --- R X B
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では、リスト ( 3 4 ) が TOS にあるリスト中に含まれているか否かを調べています。 
<[code-begin]>
&gt; ( 3 4 ) ( 1 2 ( 3 4 ) 5 ) has? .
  true ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状況が想定されたものでない場合はエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

## リスト内の各要素に関するその他のワード
配列に関するワードで既に説明しているもので、
リストにも適用できるワードも存在します。

例えば <[href set]> や <[href get]> およびその参照系や、
<[href last]>、<[href size]> といったワードがあります。

## リスト内の要素の並べ替え

