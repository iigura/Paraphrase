# スタックに関するワード
Paraphrase でのプログラミングにおいては
スタックが重要な役割を担います。
ここではスタックに関するワードの説明を行います。

## スタックの状況確認
### スタックの内容表示
最初に現在のスタックの状況を確認するためのワードについて
説明を行います。
show はデータスタック（DS）の現在の状況を、
show-rs は補強スタック（RS）の現在の状況を
それぞれ標準出力に出力します。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]>
<[gt]><[ref show]> , <[ref show-rs]><[/gt]>
<[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]><[ref stack-do-not-change ---]>（変化しません）<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例ではスタックを初期化した後、
文字列 "XEVIOUS" と 1983 と 3 という整数値を
スタックに積んだ後のスタックの様子を示しています。
<[code-begin]>
&gt; clear "XEVIOUS" 1983 3 show
      +---------+
TOS--&gt;|    3    |
      |  1983   |
      | XEVIOUS |
DS:-----------------
 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>エラーは発生しません。<[/ddt]>
<[dl-table-hr-end]>

### スタックの深さの確認
スタックにどれだけ値が積まれているのか、
スタックの深さを確認するワードも存在します。
ワード depth はスタックの深さをスタックに積み、
empty? はスタックが空であれば
（つまり深さが 0 であれば）true を、
さもなければ false をスックに積むワードです。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref depth depth]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]><[ddt]>--- I<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
<[code-begin]>
&gt; clear "Zakato" "Brag Zakato" "Garu Zakato" show
      +----------------------+
TOS--&gt;| string 'Garu Zakato' |
      | string 'Brag Zakato' |
      |   string 'Zakato'    |
DS:------------------------------
 ok.
&gt; depth .
 3 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>エラーは発生しません。<[/ddt]>
<[dl-table-hr-end]>

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref is_empty empty&quest;]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]><[ddt]>--- B<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
<[code-begin]>
&gt; clear empty? .
 true ok.
&gt; clear 'HALITO' show
      +-----------------+
TOS--&gt;| string 'HALITO' |
DS:-------------------------
 ok.
&gt; empty? .
 false ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>エラーは発生しません。<[/ddt]>
<[dl-table-hr-end]>

## スタックにあるデータの操作
### スタックを空にするワード
スタックの内容を空にするワードも重要です。
特に対話型環境においては、
ちょっとした処理を試す場合、
冒頭の処理として「まずはスタックを空にして…」という
書き方をする場合が多いです。
clear はデータスタックを空にし、
clear-rs は補強スタックを空にするワードです。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]>
<[gt]><[ref clear clear]> , <[ref clear-rs clear-rs]><[/gt]>
<[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> X1 X2 ... Xn --- （空になります）<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例ではスタックに文字列 "Zapper" を積んだ後、
ワード clear にてスタックが空になっていることを確認しています。 
<[code-begin]>
 &gt; "Zapper"
 ok.
 &gt; show
      +-----------------+
TOS--&gt;| string 'Zapper' |
DS:-------------------------
 ok.
 &gt; clear
 ok.
 &gt; show
DS:-----------------------
    (DS is empty)
 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>エラーは発生しません。<[/ddt]>
<[dl-table-hr-end]>

## 値の複製
Paraphrase でのプログラミングにおいては、
スタック上の値の複製や破棄といった操作が良く使われます。
ここではそれらの操作に必要なワードについて説明します。

スタックの一番上にある値（TOS）を複製するワードには
dup というものがあります。
Forth 由来のワードで、
その名はおそらく duplicate から来ているのでは無いかと
推測しています。
このワードによるスタックの変化は a --- a a となります。

スタックの上から 2 つの値を複製するワードもあります。
これは 2dup で a b --- a b a b となります。

dup と 2dup の動作の様子は以下のとおりです。
これらのワードについてはスタックに複製する値が
存在しなければエラーとなります。
いずれのワードも標準実行モードです。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]>
<[gt]><[ref dup dup]> , <[ref 2dup 2dup]><[/gt]>
<[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
    dup は a --- a a 、2dup は a b --- a b a b となります。
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
<[code-begin]>
&gt; clear 'MAPPY' dup show
      +----------------+
TOS--&gt;| string 'MAPPY' |
      | string 'MAPPY' |
DS:------------------------
 ok.
&gt; clear 'PAC' 'MAN' show
      +--------------+
TOS--&gt;| string 'MAN' |
      | string 'PAC' |
DS:----------------------
 ok.
&gt; 2dup show
      +--------------+
TOS--&gt;| string 'MAN' |
      | string 'PAC' |
      | string 'MAN' |
      | string 'PAC' |
DS:----------------------
 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックに複製元となる値が存在しない場合は
エラーとなります。
つまり dup ならばスタックの深さが 1 未満であるとき、
2dup では 2 未満であるときにエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

### 浅いコピーと深いコピー
Paraphrase では配列やリストをスタックに積むことができます。
これらに対し dup を行うと浅いコピーが実行されます。
C 言語の文脈で言えば、配列やリストなど
Paraphrase のコンテナはポインタとしてスタックに格納され、
dup 等ではそのポインタを複製しているにすぎません。
これを Paraphrase では（でも？）浅いコピーと呼んでいます。

そのため、dup で複製したリストなどを操作すると、
スタックの 2 番目以降に残ったリストにも影響を及ぼします。
このような状況を望む場合もあるかと思いますが、
その逆にこのような状況となっては困る場合も存在します。
これはつまり、配列やリスト等、各種コンテナに関して、
各種操作に関して互いに影響を及ぼさない独立した
複製を作成する必要がある、ということです。
このような複製を作ることを Paraphrase では（でも？）
深いコピーと呼んでいます。

このような深いコピーを行うワードとして、
clone と full-clone があります。
どちらもスタックの変化は a --- a a です。
clone と full-clone の違いですが、
clone は入れ物のコンテナの方を新規に作り、
そこに浅いコピーとしての要素を追加します。
full-clone の方は、
コンテナもコンテナに格納されている各要素についても
深いコピーを行います。
そのため、full-clone は完全なる深いコピーですが、
clone の方は完全とは言い難い深いコピーという状況です。

コピーしたリストの要素を追加削減しても、
コピー元のリストに影響がなければそれで十分であるならば、
clone の使用をおすすめします
（clone の方が full-clone よりも高速です）。

一方、例えばコピー元のリストにはリストが含まれている
場合を考えます。
そのリスト中のリストに対し、要素を追加削除する場合等において
コピー元に存在するリスト中のリストの要素数などについても
変化してもらっては困る、といった場合などは
full-clone を使うようにして下さい。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]>
<[gt]>
<[ref clone]> , <[ref full-clone]>
<[/gt]>
<[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> a --- a a <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では、full-clone により要素単位で異なる実体を持つように
複製している様子を示しています。
clone で複製したものは、
要素の複製は同じ実体を指すように複製しているので、
同じ値が表示されています。
<[code-begin]>
&gt; ( () ) full-clone 0 get 123 append drop . .
  ( ( 123 ) ) ( ( ) ) ok.
&gt; ( () ) clone 0 get 123 append drop . .
  ( ( 123 ) ) ( ( 123 ) ) ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
いずれのワードもスタックに複製元となる値が存在しない場合は
エラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

## 値の破棄
スタックに積まれた値を破棄するワードもあります。
ただスタックから値を取り出し、
その値を何かに使うことなくそのままそのワードが
終了する、というワードです。

これらのワードには drop , 2drop , 3drop といったものが
あります。
drop は a --- というスタックの変化を伴うワードです。
2drop は drop を 2 回連続して行う、という意味のワードです。
スタックの変化は a b --- となります。
3drop は a b c --- という状況となります。
補強スタックに対するワードもあり、
これらは drop-rs などのワード名が与えられています。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]>
<[gt]>
<[ref drop]> , <[ref 2drop]> , <[ref 3drop]> ,
<[ref drop-rs]> , <[ref 2drop-rs]>
<[/gt]>
<[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
drop: a --- <[br]>
2drop: a b --- <[br]>
3drop: a b c --- <[br]>
drop-rs: RS: a --- <[br]>
2drop-rs: RS: a b --- <[br]>
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
<[code-begin]>
&gt; clear 'BOZA' 'LOGRAM' show
      +-----------------+
TOS--&gt;| string 'LOGRAM' |
      |  string 'BOZA'  |
DS:-------------------------
 ok.
&gt; drop show
      +---------------+
TOS--&gt;| string 'BOZA' |
DS:-----------------------
 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
いずれのワードもスタックに破棄の対象となる値が
存在しない場合はエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

## スタック間の値の移動
Paraphrase には複数のスタックが存在しますが、
主にデータスタックと補強スタック間で
値をやりとりするワードも存在しています。

データスタックから値をひとつ取り出し、
その値を補強スタック（RS）に積むワード
&gt;r （to-R と読みます）や、
補強スタックから値をひとつ取り出し
データスタックに積む r&gt; （from-R と読みます）が
存在します。
また、それぞれのワードに参照型も存在します。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]>
<[gt]>
<[ref to_r &gt;r]> , <[ref from_r r&gt;]> , 
<[ref ref_to_r @&gt;r]> , <[ref ref_from_r @r&gt;]>
<[/gt]>
<[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
&gt;r : DS: a --- , RS: --- a <[br]>
r&gt; : RS: a --- , DS: --- a <[br]>
@&gt;r : DS: a --- a , RS: --- a <[br]>
@r&gt; : RS: a --- a , DS: --- a 
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]> 使用例はありません。 <[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
いずれのワードもスタックに移動対象となる値が
存在しない場合はエラーとなります。
つまり、&gt;r および @&gt;r の場合は DS が空の場合、
@r&gt; および r&gt; の場合は RS が空の場合は
エラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

## スタック上の値の場所替え
Paraphrase では局所変数も利用できるため、
必ずしもスタック上の値の場所を変更する必要は
無いのかもしれません。
しかし、場合によっては局所変数を使うよりも
ここで説明するワードを用いて、
スタック上の値の位置を変更した方が
簡潔に記述できる場合もあります。

スタック上の値の場所を入れ替えるワードについて
少し詳しく説明すると、例えば swap というワードは
a b --- b a というスタックの変化をもたらします。
スタック上の値の位置が変更されるだけで、
それ以外は何の効果も持ちません。

<[code-begin]>
&gt; 'HALITO' 'TILTOWAITO' show
      +---------------------+
TOS--&gt;| string 'TILTOWAITO' |
      |   string 'HALITO'   |
DS:-----------------------------
 ok.
&gt; swap show
      +---------------------+
TOS--&gt;|   string 'HALITO'   |
      | string 'TILTOWAITO' |
DS:-----------------------------
 ok.
<[code-end]>

Paraphrase には Forth 程ではありませんが、
swap のようにスタック上の値の位置を変更するワードが
いくつか存在します。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]>
<[gt]>
<[ref swap]> , <[ref swap-rs]> , <[ref over]> , 
<[ref rot]> , <[ref inv-rot]> , 
<[ref pick]> , <[ref replace]> 
<[/gt]>
<[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
swap : a b --- b a <[br]>
swap-rs : RS: a b --- b a <[br]>
over : a b --- a b a <[br]>
rot : a b c --- b c a <[br]>
inv-rot : a b c --- c a b <[br]>
pick : Xn ... X2 X1 X0 I --- Xn ... X2 X1 X0 Xk <[br]>
replace : Xn ... X2 X1 X0 a I --- Xn ... Xk+1 a Xk-1 ... X2 X1 X0 <[br]>
<[br]>
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
pick と replace については動作が複雑なので
ここで補足説明を行います
（他のワードについてはスタックがどのように
変化するのかを見れば理解できるかと思います）。

pick については、TOS の int 値を k とすると、
TOS の int 値を含まずにスタックの上から k 番目の値を
スタックに積みます。
0 pick は dup と、1 pick は over と同じ動作となります。

次の例では DUMAPIC という文字列を 0 番目の値とし、
続いて KATINO , MOGREF といった文字列が
それぞれ 1 番目、2 番目という値となります。
TOS で指定しているのは 2 番目の値なので、
結局、MOGREF という文字列が TOS にコピーされています。
<[code-begin]>
&gt; "HALITO" "MOGREF" "KATINO" "DUMAPIC" 2 pick show
      +------------------+
TOS--&gt;| string 'MOGREF'  |
      | string 'DUMAPIC' |
      | string 'KATINO'  |
      | string 'MOGREF'  |
      | string 'HALITO'  |
DS:--------------------------
 ok.
<[code-end]>

replae では、TOS にある整数値およびセカンドにある任意の値を
取り出した後のスタックに対し操作を行います。
TOS にあった整数値の値を k とし、
セカンドにあった値を a とするとき、
スタックの上から k 番目の値を a に置き換えます。

次の例では、既にスタックに積まれている値の上から 2 番目の値を
999 に置き換えています。
replae はスタック上の 2 つの値を消費するワードですので、
それらを除くスタックに対して、
つまり、下の例では 3 2 1 0 というスタックの状況に対し、
999 2 という引数にて処理を行う、と考えると分かりやすい
かと思います
（つまり 2 というのは 3 2 1 0 999 2 という
スタックの状況に対して 2 番目の値という意味ではなく、
あくまでも 999 と 2 という値を積む直前の
スタックに対して 2 番目の値、という意味として
捉えて下さい）。
<[code-begin]>
&gt; 3 2 1 0 show
      +-------+
TOS--&gt;| int 0 |
      | int 1 |
      | int 2 |
      | int 3 |
DS:---------------
 ok.
&gt; 999 2 replace show
      +---------+
TOS--&gt;|  int 0  |
      |  int 1  |
      | int 999 |
      |  int 3  |
DS:-----------------
 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
いずれのワードもスタックに対象となる値が
存在しない場合はエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>


