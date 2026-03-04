# 型変換等
Paraphrase ではスタックに積まれている値が
どのような値であるのかを示す、型に関する情報を持っています。
この章では int から float 等、
様々な型に関するワードを説明していきます。

## 型変換
### 数値に関するもの
TOS にある数値やアドレス、文字列を int や float 等に変換するワードとして、
&gt;int や &gt;float 等があります。
ワード名の先頭にある "&gt;" は "to" の意味で、
&gt;int は "to int" 等と呼びます。

なお、以下に示すワードを用いると文字列も数値に変換できますが、
後述するワード eval を用いても変換できます。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref to_int &gt;int]> , <[ref to_long &gt;long]> , <[ref to_float &gt;float]> ,
<[ref to_bigint &gt;big-int]> , <[ref to_double &gt;double]>
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
&gt;int   : &lcurly; N &verbar; S &verbar; &alpha; &rcurly; --- I <[br]>
&gt;long  : &lcurly; N &verbar; S &verbar; &alpha; &rcurly; --- L <[br]>
&gt;float : &lcurly; N &verbar; S &verbar; &alpha; &rcurly; --- F <[br]>
&gt;big-int : &lcurly; N &verbar; S &rcurly; --- J <[br]> 
&gt;double  : &lcurly; N &verbar; S &rcurly; --- D <[br]> 
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では TOS にある 1.2 なる値を整数に変換し、
その値（=1）を表示しています。 
<[code-begin]>
&gt; 1.2 &gt;int .
  1 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状況が想定されたものでない場合はエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

### 真偽値に関するもの
TOS にある値を bool 型に変換するワードは &gt;bool となります。
このワードは TOS の値が：
<[ol]>
<[li]>数値型の場合は 0 の場合は false に変換します。
そうでない場合は true に変換します。0 != と同値となります。<[/li]>
<[li]>アドレス型の場合は一旦整数型（int）に変換の上、処理されます。
&gt;int 0 != と同値となります。<[/li]>
<[li]>文字列型の場合は、空の文字列の時は false に、それ以外は true に変換されます。
"" != と同値です。<[/li]>
<[li]>無効な値（invalid）は false に変換されます。<[/li]>
<[li]>真偽値（bool）の場合は何も処理を行いません。<[/li]>
<[li]>上記以外の型の場合は真偽値に変化できないため、エラーとなります。<[/li]>
<[/ol]>

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref to_bool &gt;bool]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>&lcurly; N &verbar; B &verbar; S &verbar; &alpha; &verbar; O &rcurly; --- B<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では、1 という値を true に変換しています。
<[code-begin]>
&gt; 1 &gt;bool .
  true ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状況が想定されたものでない場合はエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

### 文字列等に関するもの
&gt;int 等では文字列を数値に変換できました。
ここでは様々な値から文字列に変換するワードを紹介します。

&gt;string では数値や真偽値などを文字列に変換します。
&gt;symbol ではシンボルに変換します。
少し変わったワードとしては &gt;hex というものがあります。
これは TOS にある整数を取り出し、その整数の 16 進数表現を
文字列としてスタックに積む、というワードです。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref to_string &gt;string]> , <[ref to_symbol &gt;symbol]> , <[ref to_hex &gt;hex]>
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
&gt;string : &beta; --- S <[br]>
&gt;string : &beta; --- T <[br]>
&gt;hex : K --- S
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では、10 進の整数値 65535 を 16 進数の文字列 FFFF に 変換して表示しています。 
<[code-begin]>
&gt; 65535 &gt;hex .
  FFFF ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状況が想定されたものでない場合はエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

### コンテナ型に関するもの
&gt;array というワードは TOS から値をひとつ取り出し、
それを配列に変換してスタックに積みます。

TOS に格納されていた値がリストであった場合、
そのリストの要素から成る配列を作成します。
リストの左端にある要素から順次、配列の 0 番目、1 番目の要素として
格納されていきます。

TOS が連想配列であった場合、
キーと値の組からなるタプルが生成される配列の要素となります。
どのような順序で配列に格納されるのかは決まっていません。

TOS が文字列の場合は、ホワイトスペース（空白、タブ、改行文字）で分割し、
これら分割された文字列から構成される配列をスタックに積みます。
なお、ホワイトスペースのみからなる文字列や空の文字列に対し本ワードを適用すると、
要素数 0 の配列をスタックに積みます。
ホワイトスペースを含まない文字列に本ワードを適用した場合は、
 当該文字列のみを含む要素数 1 の配列をスタックに積みます。 

TOS が配列の場合は何もしません。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref to_array &gt;array]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> &lcurly; R &verbar; H &verbar; A &verbar; S &rcurly; --- A <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では、文字列 "  HALITO   MAHALITO LAHALITO  " から、
"HALITO", "MAHALITO", "LAHALITO" という 3 つの文字列から構成される配列を生成しています。 
<[code-begin]>
&gt; "  HALITO   MAHALITO LAHALITO  " &gt;array .
 array:(size=3 data=0x7fa236e16d98)
[0]=HALITO
[1]=MAHALITO
[2]=LAHALITO
 ok.
<[code-end]>
なお、上に表示されている 16 進数は実行される環境により変化します。 
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状況が想定されたものでない場合はエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

TOS の値をリストに変換するワードとして en-list がありました。
en-list を配列に適用すると、出来上がるリストの要素は 1 つであり、
その要素は配列である、という状況となりました。

一方、例えば TOS に要素数 3 の配列が格納されている場合、
それらの各要素から成る要素数 3 のリストを生成したい場合もあります。
&gt;list はそのような動作を行うワードとなります。

&gt;list では、TOS にある値を取り出し、リストに変換した後スタックに積みます。
配列など集合型の値場合は、それらの要素からなるリストに変換します。
int など集合型でない値の場合は、その値ひとつからなるリストに変換します
（非集合型の値の場合はワード en-list と同様の処理となります）。 

配列に対して &gt;list を使用すると、
配列の 0 番目から順次左詰めのリストが生成されます。
連想配列に対してはキーと値の組からなるリストが生成されます。
この場合、リストの要素としてどのような順序となるかは未定義です。
リストに対しては、変換前と同じリストが生成されてスタックに積まれます。 

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref to_list &gt;list]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> X --- R <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では、&gt;array にて配列に変換したリストを、
本ワードを用いてリストに戻しています
（リストに対する &gt;array の逆演算が本ワードであることを示す例です）：
<[code-begin]>
&gt; ( ADO CAP LUTE ZAKO ) &gt;array &gt;list .cr
  ( ADO CAP LUTE ZAKO ) ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状況が想定されたものでない場合はエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

連想配列への変換 &gt;assoc については、特殊な形式のリストのみが対応となっています。
このワードは、主に連想配列の初期化用途を想定して整備しています。
&gt;assoc は TOS に格納されている全てタプルからなるリスト
（これをタブルリストなどと呼ぶことにします）を取り出し、
連想配列に変換しスタックに積みます。
新たにスタックに積まれる連想配列の要素については、
タブルリストの要素を ( K V ) とするときインデックス K に対し V なる値を持つものとします。
詳細については使用例を見て下さい。 

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref to_assoc &gt;assoc]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> R --- H <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では、タプルリストを連想配列に変換し、その内容を表示しています： 
<[code-begin]>
&gt; ( ( long-range-zapper 10 )
&gt;&gt;  ( octpus ( 70 100 ) )
&gt;&gt;  ( spinner 150 )
&gt;&gt; ) &gt;assoc dump
assoc(
	key=spinner, value=150
	key=octpus, value=( 70 100 )
	key=long-range-zapper, value=10
)
 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状況が想定されたものでない場合はエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

### その他の型変換
&gt;word は、
TOS にある文字列と同名のワードを探し、
そのワードへのポインタをスタックに積みます。
なお、Paraphrase ではスタックにワードを積むことができますが、
その実態はワードへのポインタをスタックに積むことに他なりません。

&gt;word は文字列からワードへの変換とも解釈できますので、
このようなワード名になっています。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref to_word &gt;word]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> S --- W <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では "+" という文字列を用いて数値の加算を行っています。 
<[code-begin]>
&gt; 2 3 "+" &gt;word exec .
  5 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状況が想定されたものでない場合はエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

型変換に関するワードとして最後に紹介するのは、
整数値をアドレス型に変換する &gt;addres です。
現状の Paraphrase ではあまり利用する機会は少ないのかもしれませんが、
このようなワードも存在しています。
TOS に格納されている値が big-int の場合は、
少し注意が必要です。
&gt;address が対応できる値は long で表現される値の最大値以下という制限があります。
それよりお大きな値はアドレス型に変換できません。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref to_address &gt;address]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> K --- &alpha; <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
使用例はありません。
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状況が想定されたものでない場合はエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

## 型の確認
型に関するワードとしては、TOS にある値がどのような型なのかを
調べるものもあります。

TOS にある値がどのような型であるのかを調べるには
type-name というワードにて確認できます。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref type-name]> , <[ref ref_type-name @type-name]>
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
type-name: X --- S <[br]>
@type-name: X --- XS
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では、数値 1.23 の型を表示しています。 
<[code-begin]>
&gt; 1.23 type-name .
  double ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックが想定された状態にない場合はエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

具体的な型に対しては、例えば int 型であるのか否か等については、
int&quest; など、型名に疑問符を付けたワードにより確認できます。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref is_bool bool&quest;]> , 
<[ref is_int int&quest;]> , <[ref is_long long&quest;]> , <[ref is_bigInt big-int&quest;]> , 
<[ref is_float float&quest;]> , <[ref is_double double&quest;]> , 
<[ref is_string string&quest;]> ,
<[/gt]><[br]><[gt]>
<[ref is_word word&quest;]> , <[ref ref_is_word @word&quest;]> , 
<[ref is_array array&quest;]> , <[ref ref_is_array @array&quest;]> , 
<[ref is_list list&quest;]> , <[ref ref_is_list @list&quest;]> , 
<[/gt]><[br]><[gt]>
<[ref is_assoc assoc&quest;]> , <[ref ref_is_assoc @assoc&quest;]> 
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> X --- B <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では TOS にある値が int であるのか等を調べています。
<[code-begin]>
&gt; 123 int&quest; .
  true ok.
&gt; 123 string&quest; .
  false ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックが空である場合はエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

## 型変換の確認
TOS より値を取り出し、その値がとある型に変換できるかどうかを調べるワードも存在しています。
例えば &gt;int&quest; 等です。
このワードは TOS より値を取り出し、その値が int に変換できるか否かを調べます。
リストなど、そもそも整数値に変換できない値の場合は false をスタックに積みます。
また、TOS に格納されている値が数値であっても、
その値の大きさが int で表現できる範囲に収まらなければ、
その場合も false をスタックに積みます。
もちろん、整数に変換でき、かつ int の範囲で表現できる値であれば true を積みます。 
以下にこれら型変換が可能か否かを調べるワードを記します：

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref is_to_int &gt;int&quest;]> , 
<[ref is_to_long &gt;long&quest;]> , 
<[ref is_to_float &gt;float&quest;]> , 
<[ref is_to_double &gt;double&quest;]>
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> X --- B <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では、8 番目のメルセンヌ素数 2147483647 が int に変換できることを確認しています。
<[code-begin]>
&gt; 2LL 31 pow 1 - &gt;int&quest; .
  true ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状況が想定されたものでない場合はエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

## その他の型変換
Paraphrase の値は、値の実体と値の型を表す情報により実現されています。
例えば、123 という整数値は、123 というメモリ上の整数値の表現と、
そのメモリ上の値を整数として解釈するための識別情報（＝型情報）から
構成されています。

この型を表す識別情報の実態は C/C++ における long 型の値です。
この型に関する識別の値を取り出すワードが &gt;type です。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref to_type &gt;type]> , <[ref ref_to_type @&gt;type]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
&gt;type : X --- L<[br]>
@&gt;type : X --- X L
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では、整数を示す型情報を取り出して表示しています。
なお、この例では整数を表す型情報の値は 0L となっていますが、
Paraphrase のバージョンによっては、この値が異なる場合もありますので、
注意して下さい
（ `int-type-value : 1 &gt;type ; などとして int を表す型の値を取り出すワードを作っておいても良いかもしれません）。
<[code-begin]>
&gt; 123 >type .
  0L ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックが空の場合はエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

型情報の値を取り出す場合は、多くの場合、C 言語におけるキャストのようなことを
行いたいのではないかと想像しています。
セカンドに積まれた値を TOS に積まれた型情報の値でキャストするワードとして
type&gt; が用意されています。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref from_type type&gt;]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> X L--- X<[br]> <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では、16.0f という浮動小数点値をビットパターンそのままで整数値として解釈しなおしています。
<[code-begin]>
&gt; 16.0f 1 >type type> .
 1098907648 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックが想定された状態でない場合はエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

