# 数値演算と数学関数
## 数値の入力について
### 整数
Paraphrase には int, long, big-int という
複数の整数型が存在します。
プログラム上や対話環境において
直接具体的な数値を入力する場合は、
それぞれの数値が格納できる最も小さな整数型として
取り扱われます。

int や long で取り扱える値の範囲は、
Paraphrase が実装されている
それぞれの環境に依存します。
具体的には Paraphrase が記述されている C++ 言語により、
これらの型の表現できる範囲が決まります。
おそらく、int よりも long の方がより多くの範囲を
表現可能な状況になっていると思います。
long に収容できないような大きな値は big-int という
多倍長整数型として取り扱われます。

以下は対話型環境において、
自動的に整数値の型が定まっている様子を示しています：

<[code-begin]>
&gt; clear 1234567890 show
      +----------------+
TOS--&gt;| int 1234567890 |
DS:------------------------
 ok.
&gt; 123456789012345 show
      +-----------------------+
TOS--&gt;| long 123456789012345L |
      |    int 1234567890     |
DS:-------------------------------
 ok.
&gt; 12345678901234567890 show
      +-------------------------------+
TOS--&gt;| bigInt 12345678901234567890LL |
      |     long 123456789012345L     |
      |        int 1234567890         |
DS:---------------------------------------
 ok.
<[code-end]>

なお、整数値の先頭に 0x を付ければ
16 進数表記として解釈されます：

<[code-begin]>
&gt; 0x12345 show
      +-----------+
TOS--&gt;| int 74565 |
DS:-------------------
 ok.
&gt; 0x1234567890 show
      +-------------------+
TOS--&gt;| long 78187493520L |
      |     int 74565     |
DS:---------------------------
 ok.
&gt; 0x12345678901234567890 show
      +----------------------------------+
TOS--&gt;| bigInt 85968058271978839505040LL |
      |        long 78187493520L         |
      |            int 74565             |
DS:------------------------------------------
 ok.
<[code-end]>

### 浮動小数点
整数は格納可能な最小限の型に格納されましたが、
浮動小数点の場合はできるだけ正確に表現できる型に
格納されます。

型を定める具体的な方法ですが、
まず double 型として取り扱い、
その後、float 型でも表現可能か否かを確認します。
float 型でも double 型の場合と同じ値が格納できるのであれば、
float 型とし、さもなければ double 型とします。

float 型並びに double 型の具体的な数値表現については、
Paraphrase の浮動小数点値は C++ の float 型ならびに
double 型をそのまま利用しているため、
実行環境におけるこれらの型と同じとなります。

整数値の場合は、値が大きくなるにつれ
int から long、big-int と型が変化していきましたが、
浮動小数点値の場合は必ずしもそうはなりません。

<[code-begin]>
&gt; clear 0.7 show
      +-----------------+
TOS--&gt;| double 0.700000 |
DS:-------------------------
 ok.
&gt; 0.5 show
      +-----------------+
TOS--&gt;| float 0.500000f |
      | double 0.700000 |
DS:-------------------------
 ok.
<[code-end]>

整数値の場合と同様に 数値の先頭に 0x を付けると
16 進数として取り扱われます。

<[code-begin]>
&gt; clear 0x10000.0 show
      +---------------------+
TOS--&gt;| float 65536.000000f |
DS:-----------------------------
 ok.
&gt; 0x0.8 show
      +---------------------+
TOS--&gt;|   float 0.500000f   |
      | float 65536.000000f |
DS:-----------------------------
 ok.
&gt; 0x0.123456789 show
      +---------------------+
TOS--&gt;|   double 0.071111   |
      |   float 0.500000f   |
      | float 65536.000000f |
DS:-----------------------------
 ok.
<[code-end]>

## 数値型の指定の必要性
上に示しているように、具体的な数値を扱う場合においては、
できるだけ数値に関する情報を失わないように
Paraphrase の側にて処理を行っています。

この章では四則演算等の計算を行うワードを紹介していきますが、
これらのワードでは計算結果により数値の型を変えるという
ことはありません。
つまり、int 型の整数の最大値…これは正の整数ですが、
これに 1 を加えたら負の数になってしまいます。
<[code-begin]>
&gt; clear 2147483647 1 + show
      +-----------------+
TOS--&gt;| int -2147483648 |
DS:-------------------------
 ok.
<[code-end]>

このような状況を防ぐため、
数値の末尾に型に関する情報を記すことで
型指定できるようになっています。

整数値の場合、特に何も指定しなければ、
既に示しているようにその値を表現し得る
最小の型として取り扱われます。
int 型として表現可能な値であれば int 型となります。

しかし、数値の末尾に <[ref type_small_l l]>
または <[ref type_large_L L（型指定）]> を記すと、
int 型に格納可能な整数値であっても
long 型として取り扱われます。
<[code-begin]>
&gt; clear 1L show
      +---------+
TOS--&gt;| long 1L |
DS:-----------------
 ok.
<[code-end]>

そのため、先程示した 2147483647 + 1 を計算させる場合などでも、
2147483647 という値を long 型の整数値として指定すれば
2147483647 + 1 の結果として正しく 2147483648 という値を
得ることができます。
<[code-begin]>
&gt; clear 2147483647L 1 + show
      +------------------+
TOS--&gt;| long 2147483648L |
DS:--------------------------
 ok.
<[code-end]>

同様にして、数値の最後に <[ref small_ll ll]>
または <[ref LL]> を付けると
それらの整数値は big-int 型となります。
big-int 型であることを明示することにより、
10 番目のメルセンヌ素数である 2&circ;89-1 も
正確に計算できます。
<[code-begin]>
&gt; clear 2LL 89 pow 1 - .
 618970019642690137449562111LL ok.
<[code-end]>

これは big-int 型でないと格納できない整数値ですので、
2L 89 pow 1 - や、2 89 pow 1 - では
正しく計算できない状況となります：
<[code-begin]>
&gt; clear 2L 89 pow 1 - .
 9223372036854775806L ok.
&gt; clear 2 89 pow 1 - .
 2147483646 ok.
<[code-end]>

浮動小数点を取り扱う場合は、
特に float や double を意識する必要は少ないかもしれませんが、
末尾に <[ref type_f f（型指定）]> や
<[ref type_d d（型指定）]> を付けると、
それぞれ float ならびに double 型となります。
<[code-begin]>
&gt; clear 0.5f show
      +-----------------+
TOS--&gt;| float 0.500000f |
DS:-------------------------
 ok.
&gt; 0.5d show
      +-----------------+
TOS--&gt;| double 0.500000 |
      | float 0.500000f |
DS:-------------------------
 ok.
<[code-end]>

### 浮動小数点型の型指定時の注意（16 進数を取り扱う場合）
浮動小数点値の型指定で注意しなければならないのは、
16 進数を取り扱う場合の時です。
というのも 16 進数では f や d を含む A から F までが
各桁を表す「数字」として使用されるからです。

そのため、0x0.123456789ABCDEf は 16 進数で表された
浮動小数点値 0x0.123456789ABCDE を float で表すという
意味ではなく、0x0.123456789ABCDEF という浮動小数点値を
表します。
この数値について double 型で表現する必要がある場合は
当然ですが double 型の数値として取り扱われます。
<[code-begin]>
&gt; clear 0x0.123456789ABCDEf show
      +-----------------+
TOS--&gt;| double 0.071111 |
DS:-------------------------
 ok.
<[code-end]>

もし、0x0.123456789ABCDE を強制的に float 型の
数値としたい場合には、後ほど説明する型変換ワード
&gt;float を用いるようにして下さい。
<[code-begin]>
&gt; clear 0x0.123456789ABCDE &gt;float show
      +-----------------+
TOS--&gt;| float 0.071111f |
DS:-------------------------
 ok.
<[code-end]>

同様の注意は double 型を指定する d を用いる場合も同様です。
例えば 0x0.1d は 0x0.1D という 16 進数表記の
浮動小数点値として解釈されますので、
0x0.1 という値を double 型として取り扱いたい場合も
double 型へ変換するワード（これも後述します） &gt;double
を利用するようにして下さい。
<[code-begin]>
&gt; clear 0x0.1d show
      +-----------------+
TOS--&gt;| float 0.113281f |
DS:-------------------------
 ok.
&gt; 0x0.1 show
      +-----------------+
TOS--&gt;| float 0.062500f |
      | float 0.113281f |
DS:-------------------------
 ok.
&gt; 0x0.1 &gt;double show
      +-----------------+
TOS--&gt;| double 0.062500 |
      | float 0.062500f |
      | float 0.113281f |
DS:-------------------------
 ok.
<[code-end]>

数値と数値の型に関する説明が終わりましたので、
次からいよいよ計算や数学関数に関するワードを説明していきます。

## 四則演算
四則演算を行うワードは通常の算術記号と同じです。
参照型については命名規則通り @+ 等のワードと、
+@ 等、後ろに @ が付くものがあります。

命名規則通りのものは、
ワード実行前のスタック上の値をそのまま保存しますが、
+@ 等ではスタックのセカンドのみを残す状態となります。
<[code-begin]>
&gt; clear 1 2 +@ show
      +-------+
TOS--&gt;| int 3 |
      | int 1 |
DS:---------------
 ok.
<[code-end]>

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]>
<[gt]>
<[ref plus +]> , <[ref minus -]> ,
<[ref mul &ast;]> , <[ref div /]> ,  
<[ref ref_plus @+]> , <[ref ref_minus @-]> ,
<[ref ref_mul @&ast;]> , <[ref ref_div @/]> ,  
<[ref post_ref_plus +@]> , <[ref post_ref_minus -@]> ,
<[ref post_ref_mul &ast;@]> , <[ref post_ref_div /@]>
<[/gt]>
<[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
参照型以外は N N --- N となり、<[br]>
（標準的な）参照型は N1 N2 --- N1 N2 N3 となり、<[br]>
後ろに @ が付く参照型は N1 N2 --- N1 N3 となります。
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では 1+2 を計算しています。
<[code-begin]>
&gt; 1 2 + .
 3 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
想定されるスタックの状況となっていない場合はエラーとなります。
除算に関しては 0 で除算するとエラーが発生します。
<[/ddt]>
<[dl-table-hr-end]>

## 剰余
2 つの整数値の余りを計算するワードは &percnt; となります。
a mod b = c を計算する場合には
a b &percnt; としてワード &percnt; を呼び出せば、
スタック上にある a と b なる値を取り出して
a mod b の結果である c という値をスタックに積みます。

四則演算と同様に通常の参照型 @&percnt; ならびに
後ろに @ が付く参照型 &percnt;@ も用意されています。

スタックの変化は、ワードを呼び出す前に
スタックに積まれている数値型が整数型である点を除いては
四則演算を行うワードと同様です。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]>
<[gt]>
<[ref mod &percnt;]> , <[ref ref_mod @&percnt;]> ,
<[ref post_ref_mod &percnt;@]>
<[/gt]>
<[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
&percnt; : K1 K2 --- K3 <[br]>
@&percnt; : K1 K2 --- K1 K2 K3 <[br]>
&percnt;@ : K1 K2 --- K1 K3
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
17 mod 3 (=2) をそれぞれのワードで計算した例を
以下に示します。
<[code-begin]>
&gt; clear 17 3 % show
      +-------+
TOS--&gt;| int 2 |
DS:---------------
 ok.
&gt; clear 17 3 @% show
      +--------+
TOS--&gt;| int 2  |
      | int 3  |
      | int 17 |
DS:----------------
 ok.
&gt; clear 17 3 %@ show
      +--------+
TOS--&gt;| int 2  |
      | int 17 |
DS:----------------
 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状態が想定されているものと異なる場合は
エラーとなります。
またスタックトップが 0 である場合も
エラーとなります（0 で割った余りを求めることになるためです）。
<[/ddt]>
<[dl-table-hr-end]>

## ビット演算
ビット単位の演算を行うワードもあります。
2 項演算を行う場合は TOS とセカンドに格納されている
値を対象にビット演算を行います。
例えば、0x1234 の下位 4 ビットを取り出して表示する場合は
0x1234 0x000F &amp; . などとします。
ビットシフトはセカンドにある値を TOS の値の分だけ
シフトします。
次の例は右ビットシフトを行うワード &gt;&gt; を用いて
16 進数で一桁分（つまり 4 ビット分だけ）
右にシフトしている様子です。
to-hex-str というワードについては後述しますが、
TOS にある値を 16 進数で表示した時の文字列に
変換するワードです。
<[code-begin]>
&gt; 0x1234 4 &gt;&gt; to-hex-str .
  123 ok.
<[code-end]>

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]>
<[gt]>
<[ref bit_and &amp;]> , <[ref bit_or &verbar;]> ,
<[ref bit_xor &circ;]> ,
<[ref bit_not &sim;]> ,
<[ref bit-lshift &lt;&lt;]> ,
<[ref bit-rshift &gt;&gt;]> ,
<[ref ref_bit_and @&amp;]> , <[ref ref_bit_or @&verbar;]> ,
<[ref ref_bit_xor @&circ;]> ,
<[ref ref_bit_not @&sim;]> ,
<[ref ref_lshift @&lt;&lt;]> ,
<[ref ref_rshift @&gt;&gt;]> 
<[/gt]>
<[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
&amp; , &verbar; , &circ; , &lt;&lt;&lt; , &gt;&gt;&gt; は 
K K --- K <[br]>
&sim; は K --- K <[br]>
@&amp; , @&verbar; , @&circ; , @&lt;&lt;&lt; , @&gt;&gt;&gt; は 
K1 K2 --- K1 K2 K3 <[br]>
@&sim; は K1 --- K1 K2 
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では 0x1234 を左に 4 ビットシフトして、
その値を 16 進数に変換しています。
<[code-begin]>
&gt; 0x1234 4 @<< to-hex-str show
      +--------------+
TOS--&gt;| string 12340 |
      |    int 4     |
      |   int 4660   |
DS:----------------------
ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状態が想定されているものと異なる場合は
エラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

## 数学関数
### 数学定数
ワード pi では円周率をスタックに積みます。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref pi]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> --- D <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
<[code-begin]>
&gt; pi .
 3.141593 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
エラーは発生しません。
<[/ddt]>
<[dl-table-hr-end]>

### べき乗と平方根
a の b 乗は a b pow として計算できます。
次の例では 2 の 3 乗を計算して表示しています。
<[code-begin]>
&gt; 2 3 pow .
 8 ok.
<[code-end]>
現在のバージョンでは、セカンドにある値は
任意の数値型に対応していますが、
TOS の値は big-int 型以外の数値型でないとエラーとなります。
つまり、とある数値の big-int 乗には対応していませんので、
注意して下さい。

よく利用する 2 乗については square という
ワードが用意されています。
このワードは TOS より値を一つとりだして
その値の 2 乗をスタックに積みます。

また、0.5 乗である平方根についても
専用のワードが用意されており、
こちらは sqrt という名前になっています。
sqrt には参照型も用意されており @sqrt というワードも
存在します。

べき乗が施される側の値、
つまり a b pow における a の値が自然対数の底となる場合は、
それに特化したワード exp が用意されています。
このワードは、TOS より取り出し
その値を用いて e （自然対数の底）の TOS 乗を計算し、
スタックにプッシュします。

e の π 乗から π を減じた値は
19.999099979... と 20 に近い値となることが
知られています（e&circ;π-π≒20）。
興味を持たれた方は「ゲルフォントの定数」というキーワードで
検索してみて下さい。
実際に Paraphrase でも計算してみると
かなり近い値が計算できていることが確認できます。
<[code-begin]>
&gt; pi exp pi - .
 19.999100 ok.
<[code-end]>

べき乗に関するこれらのワードをまとめますと、
次のようになります：
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref pow]> , <[ref square]> ,
<[ref sqrt]> , <[ref ref_sqrt @sqrt]> , <[ref exp]>
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
pow : N M --- N <[br]>
square : N --- N <[br]>
sqrt および exp : N --- E <[br]>
@sqrt : N --- N E
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では 2 の平方根を計算し、表示しています。
<[code-begin]>
&gt; 2 sqrt .
  1.414214 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状態が想定されているものと異なる場合は
エラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

### 対数
TOS にある対数は log や log10 で計算できます。
それぞれ TOS から値を取り出し、
自然対数の底に対するものと、
10 を底とする対数を計算し、スタックに積みます。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref log]> , <[ref log10]> 
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> N --- E <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では 100 の log10 を計算し、表示しています。
<[code-begin]>
&gt; 100 log10 .
  2.000000 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状態が想定されているものと異なる場合は
エラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

### 三角関数
ワード sin , cos , tan では
TOS より値を取り出し、
この値を角度を表すものとして扱い、
その角度対する正弦、余弦、正接を求め、
スタックに積みます。
角度の単位はラジアンです。

asin , acos , atan は TOS より値を取り出し、
その値に対する 逆正接、逆余弦、逆正接を求め
スタックに積みます。
asin など、先頭の a は arc の意味です。
asin = arc sin と読みます。
これらのワードで得られる値の単位はラジアンです。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref sin]> , <[ref cos]> , <[ref tan]> ,
<[ref asin]> , <[ref acos]> , <[ref atan]>
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> N --- E <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では 1 のアークタンジェントを計算し、
それを 4 倍してπの近似値を求めています。
<[code-begin]>
&gt; 1 atan 4 * .
  3.141593 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状態が想定されているものと異なる場合は
エラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

ラジアンと度を変換するワードも用意されています。
ラジアンから度への変換は rad-to-deg です。
度からラジアンへの変換は deg-to-rad で行えます。
いずれのワードも TOS より値を一つ取り出し、
その値をラジアンまたは度とみなして
度やラジアンに変換した値をスタックに積みます。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref rad-to-deg]> , <[ref deg-to-rad]>
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> N --- E <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例ではラジアンで示されたπという値を、
180 度に変換しています。
<[code-begin]>
&gt; pi rad-to-deg .
 180.000000 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状態が想定されているものと異なる場合は
エラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

### 乱数
乱数を発生させるワードは rand とrandom があります。
いずれのワードも発生させた乱数をスタックに積みます。
rand は整数値の乱数を発生させるワードで、
発生する乱数は 0 からワード rand-max で示される範囲に
収まるものとなります（rand-max については後述します）。
random は 0.0 から 1.0 までの float 値の乱数を発生させる
ワードです。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref rand]> , <[ref random]>
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
rand : --- I <[br]>
random : --- F
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では、モンテカルロ法を用いて
円周率の近似値を計算しています。
このプログラムの概略ですが、
for+ 〜 next にて 10,000 回のループを行い、
そのループ中にて 2 つの乱数を発生させ、
それを 2 次元座標の位置と見なし、
その点が半径 1 の円（厳密に言えば第 1 象限にある円の一部）の
内部に存在する比率を求めています。
random は 0 から 1 までに分布する一様乱数なので、
発生させた点が 1/4 円の内部に存在する確率は π/4 となります。
比率を計算した後に 4 倍してπの近似値を表示しています。
<[code-begin]>
&nbsp;&gt; 0.0 0 10000 for+
&gt;&gt; &nbsp;&nbsp; random dup * random dup * + sqrt
&gt;&gt; &nbsp;&nbsp; 1 < if 1 + then
&gt;&gt; next 10000 / 4 * .
 3.154000f ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
エラーは発生しません。
<[/ddt]>
<[dl-table-hr-end]>

先程 rand の説明のときに出てきた rand-max について
説明します。
このワードは rand により生成される乱数の最大値を
スタックに積むワードです。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref rand-max]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> --- I <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では rand と rand-max を用いて
0 から 1 までの乱数を生成しています。
<[code-begin]>
&gt; 0 5 for+ rand rand-max &gt;float / .cr next
 0.417486f
 0.686773f
 0.588977f
 0.930436f
 0.846167f
 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
エラーは発生しません。
<[/ddt]>
<[dl-table-hr-end]>

rand は 0 から rand-max までの範囲の整数値の乱数を
発生するワードでしたが、0 以上でとある整数値未満の
整数値の乱数が必要となる場面もよくあります。
例えば 0 以上 100 未満の整数値の乱数が必要であれば
random 100 * &gt;int などとすれば良いのですが、
rand-to というワードを利用することもできます。

rand-to は TOS より整数値を取り出し、
0 以上で、かつ、その取り出した値よりも小さい
整数値の乱数を発生させるワードです。
rand と同様に発生させた乱数はスタックに積まれます。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref rand-to]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> I --- I <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では [0,10) の乱数を 10 個生成し表示しています。
<[code-begin]>
&gt; 1 10 for+ 10 rand-to . next
  0 1 7 4 5 2 0 6 6 9 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状態が想定されているものと異なる場合は
エラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

上記で説明した乱数は他のプログラミング言語と同様に
計算により生成される疑似乱数です。
生成される乱数は乱数の種（seed）により決定されます。
乱数の種に関するワードには set-random-seed と
randomize があります。

set-random-seed は TOS より整数値を取り出し、
その値を乱数の種値とします。
randomize は現在の時刻を利用して乱数の種値を設定します。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref set-random-seed]> , <[ref randomize]>
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
set-random-seed : --- I <[br]>
randomize : ---
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では set-random-seed で同じ値を種値とすると、
同じ乱数が得られる様子を示しています。
<[code-begin]>
&gt; 12345 set-random-seed rand .
 207482415 ok.
&gt; rand .cr rand .cr
 1790989824
 2035175616
 ok.
&gt; 12345 set-random-seed rand .
 207482415 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
set-random-seed については、
スタックの状態が想定されているものと異なる場合は
エラーとなります。
randomize ではエラーは発生しません。
<[/ddt]>
<[dl-table-hr-end]>

### 天井関数と床関数
TOS より数値を取り出し、その数値以上の最小の
整数をスタックに積むワードが ceil であり、
TOS に積まれていた数値以下の最大の整数をスタックに
積むワードが floor です。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref ceil]> , <[ref floor]>
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> N --- I <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では -1.2 に対する ceil と floor の値を計算し
その結果を表示しています。
<[code-begin]>
&gt; -1.2 ceil .
 -1 ok.
&gt; -1.2 floor .
 -2 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状態が想定されているものと異なる場合は
エラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

### 最大・最小
TOS およびセカンドから数値を取り出し、
それらの数値の最大値や最小値をスタックに積むワードが
max ならびに min です。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref max]> , <[ref min]>
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> N N --- N <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では TOS およびセカンドに存在する
3 と 5  という値に対し最大値を計算して表示しています。
<[code-begin]>
&gt; 3 5 max .
  5 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状態が想定されているものと異なる場合は
エラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

### その他の数学（的）ワード
TOS にある数値の絶対値を計算し、
スタックに積むワードは abs です。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref abs]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> N --- N <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例は TOS にある -5 という値の絶対値を計算して
表示しています。
<[code-begin]>
&gt; -5 abs .
  5 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状態が想定されているものと異なる場合は
エラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

TOS より数値を取り出し、
小数点第一位を四捨五入した値をスタックに積むワードとして
round が用意されています。
負の数についての挙動については、次の使用例を参考にして下さい。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref round]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> N --- N <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例は負の数に対して本ワードを実行した例です
<[code-begin]>
&gt; -1.4 round .
  -1.000000ok.
&gt; -1.5 round .
  -2.000000ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状態が想定されているものと異なる場合は
エラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

even? とその参照型である @even? は
TOS に格納されている整数値が偶数であるか否かを
判定するワードです。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref is_even even&quest;]> ,
<[ref ref_is_even @even&quest;]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
even&quest; : K --- B <[br]>
@even&quest; : K --- K B
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
<[code-begin]>
&gt; 123 even&quest; . 
  false ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状態が想定されているものと異なる場合は
エラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

