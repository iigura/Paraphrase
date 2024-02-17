# 文字列
## ヒアドキュメント
Paradocs では二重引用符「"」または引用符「'」で囲んだものは、
文字列（という値）として認識されます。
それ以外にも、例えば改行を含む文字列を取り扱う
「ヒアドキュメント」と呼ばれる文字列も
取り扱うことができます。

### デデントモードのヒアドキュメント
ヒアドキュメントについては、
ヒアドキュメントを生成するワードを活用することで
実現されます。
ワード &lt;&lt;&lt; はヒアドキュメントを
開始するワードですが、ワードですので
ワードと認識されるための区切りが必要となります。
これは例えば、&lt;&lt;&lt;this is here document&gt;&gt;&gt;
などとした場合、
"&lt;&lt;&lt;this" というワードとして認識される
状況となってしまうからです。
このような制約があるため、
Paraphrase においてヒアドキュメントを利用する場合には、
ヒアドキュメントを生成するワードをワードとして認識させるため
&lt;&lt;&lt; の直後に空白文字やタブ文字等が必要となります。

と、このように説明すると、Paradocs では
空白文字から始まるヒアドキュメントは
利用できないのか？という疑問を持たれるかもしれませんが、
そのようなことはありません。
こちらについては後ほど説明します。

話をワード &lt;&lt;&lt; と &gt;&gt;&gt; にて
生成されるヒアドキュメントに戻しましょう。
これらのワードで囲まれるヒアドキュメントは、
&lt;&lt;&lt; の直後ならびに改行直後の
空白文字ならびにタブ文字を削除したものとして
取り扱われます。
これをデデント(dedent）モードのヒアドキュメントと呼びます。

実際にインタプリタ para での動作は次のようになります。
<[code-begin]>
&nbsp;&gt; &lt;&lt;&lt;
&gt;&gt;     a
&gt;&gt;   b
&gt;&gt; c
&gt;&gt;&nbsp; &gt;&gt;&gt; show
      +--------------------+
TOS-->| string 'a\nb\nc\n' |
DS:----------------------------
 ok.
&nbsp;&gt; write
a
b
c
 ok
<[code-end]>
ご覧のようにヒアドキュメントとして入力した際には
それぞれの文字 a, b, c の行頭には空白文字がいくつか付いて
いました。
しかし、ワード &lt;&lt;&lt; と &gt;&gt;&gt; により
生成されるヒアドキュメントはデデントモードの
ヒアドキュメントですので、これらの空白（字下げ、インデント）が
全て除去されたものとして取り扱われています。
なお、除去されるのは先頭部分の空白のみですので、
それ以外の空白文字は保存されます。

ワード &gt;&gt;&gt; もワードとして認識されなければ
なりませんので、&gt;&gt;&gt; の直前には
空白文字やタブ文字等の区切りとして認識される文字が
必要となります。

### 近似モードのヒアドキュメント
デデントモードのヒアドキュメントでは、
行頭の空白文字が削除されてしまいます。
行頭の空白も含めてヒアドキュメントを構成するために
近似モードのヒアドキュメントのためのワードが
設けられています。

それが raw&lt;&lt;&lt; と &gt;&gt;&gt;raw です。
これらのワードもワードとして認識される必要があるので、
厳密な意味においては、
入力された文字列を完全にヒアドキュメントとして
利用できるものではありません
（故に近似モードなのです）。
しかし、実際の利用においては殆ど支障がないのでは
ないかと思っています。
もしかすると、この近似モードでも対応できない場面も
あるかもしれませんが、
Paraphrase の標準ワードには
文字列を加工するためのものもいくつか存在しますので、
それらを用いることで対処できるかと思います。

ワード raw&lt;&lt;&lt; の後には
空白文字やタブ文字が必要です。
raw&lt;&lt;&lt; と同じ行に書かれたヒアドキュメントの一部は
その先頭の空白文字が削除されます
（同じ行に存在する文字列のみデデントモードとなります）。
改行以降は &gt;&gt;&gt;raw というワードが
現れるまで、文字列は無加工でヒアドキュメントを
構成するものとなります。
もちろん &gt;&gt;&gt;raw をワードとして認識しなければ
ならないので、&gt;&gt;&gt; と同様に直前に空白文字等が
必要となります。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref begin_hd &lt;&lt;&lt;]> , 
<[ref end_hd &gt;&gt;&gt;]> , 
<[ref begin_raw_hd raw&lt;&lt;&lt;]> , 
<[ref end_raw_hd &gt;&gt;&gt;raw]>
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>実行レベル 2<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
&lt;&lt;&lt; および raw&lt;&lt;&lt; : DS: --- θ ω , RS: --- U <[br]>
&gt;&gt;&gt; および &gt;&gt;&gt;raw : DS: θ ω --- , RS: U ---
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では先頭の空白を含む複数行から構成される
メッセージを表示しています。
表示に際しては余分な空白が入ると
レイアウトが崩れてしまうので . （ドット）ではなく
write を用いて出力しています。
<[code-begin]>
&nbsp;&gt; raw&lt;&lt;&lt;
&gt;&gt;    +------+
&gt;&gt;    | GAME |
&gt;&gt;    | OVER |
&gt;&gt;    |      |
&gt;&gt; ---+------+---
&gt;&gt; &gt;&gt;&gt;raw write
   +------+
   | GAME |
   | OVER |
   |      |
---+------+---
 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
&lt;&lt;&lt; と raw&lt;&lt;&lt; については
エラーは発生しません。
&gt;&gt;&gt; と &gt;&gt;&gt;raw は、
スタックの状態が想定されているものと異なる場合は
エラーが発生します。
<[/ddt]>
<[dl-table-hr-end]>

## 文字列の長さ
TOS に積まれている文字列の長さは size にて
取得できます。
size にて取得できる数値は厳密に言えば
文字列を格納する際に使用しているメモリ量（バイト数）に
なりますので、日本語文字列などの
マルチバイト文字に関しては注意が必要です。

size は文字列のみに対応している
ワードではありません。
後述するリストや配列などにも対応しています。
リストや配列の場合は、そのトップレベルにある
要素数をスタックに積みます。

size はスタックより値を一つ取り出して、
その値に対して処理を行います。
参照形である @size では TOS にある値は取り出しません。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref size]> , <[ref ref_size @size]> 
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
size : &lcurly; A &verbar; R &verbar; S &verbar; T &rcurly; --- I <[br]>
@size : &lcurly; A &verbar; R &verbar; S &verbar; T &rcurly; --- &lcurly; A &verbar; R &verbar; S &verbar; T &rcurly; I
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では "Paraphrase" という文字列の長さ
（バイト長）を表示しています。
<[code-begin]>
 &gt; "Paraphrase" size .
 10 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状態が想定されているものと異なる場合は
エラーが発生します。
<[/ddt]>
<[dl-table-hr-end]>

TOS にある文字列が空であるか否かという確認については、
専用のワードが用意されています。
@empty-str? は TOS の値を参照し、
空の文字列である場合は true を、
さもなければ false をスタックに積みます。

@not-empty-str? は @empty-str? と逆で、
空の文字列でなければ true を、
空の文字列の場合は false をスタックに積みます。
@not-empty-str? は @empty-str? not と同値です。
@not-empty-str? も TOS の値を参照するのみで、
TOS から値は取り出しません。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref ref_is_empty-str @empty-str&quest;]> , 
<[ref ref_is_not-empty-str @not-empty-str&quest;]> 
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
S --- S B
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では "" という空の文字列が空であることを
確認しています。
<[code-begin]>
 &gt; "" @empty-str&quest; .
 true ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状態が想定されているものと異なる場合は
エラーが発生します。
<[/ddt]>
<[dl-table-hr-end]>

## 文字列の取り出し
とある文字列 S1 に格納されている I 番目の文字
（厳密には I バイト目の一文字）を
取り出すワードとして at があります。
このワードはセカンドに S1 が格納されており、
TOS に I が格納されているものとして動作します。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref at]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
S1 I --- S1 S2
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では "Paraphrase" という文字列の 5 番目の文字である
h をコピーして表示しています。
文字の位置の指定方法は 0 オリジンです
（先頭の文字は 0 文字目として数えます）。
<[code-begin]>
 &gt; "Paraphrase" 5 at show
      +---------------------+
TOS--&gt;|     string 'h'      |
      | string 'Paraphrase' |
DS:-----------------------------
 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状態が想定されているものと異なる場合は
エラーが発生します。
<[/ddt]>
<[dl-table-hr-end]>

## 文字列の変換
文字列中の英小文字を大文字に変換するワードとして
&gt;upper があります。
逆に英大文字を小文字にするのは &gt;lower です。
それぞれ to upper , to lower と読みます。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref to_upper &gt;upper]> ,
<[ref to_lower &gt;lower]>
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> S --- S <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では、"Hello 世界" という文字列に含まれている、
英小文字を大文字に変換しています。
<[code-begin]>
&gt; "Hello 世界" &gt;upper .
  HELLO 世界 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状態が想定されているものと異なる場合は
エラーが発生します。
<[/ddt]>
<[dl-table-hr-end]>

文字列の中身ではなく、長さを変更するワードもあります。
trim は、
文字列の先頭部分および末尾部分の空白を取り除きます。
空白には空白文字（スペース）以外にも
タブや改行も含まれるので注意して下さい。
ワード trim では、
正規表現で &#92;s として取り扱われるものを空白とみなして
処理を行います。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref trim]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> S --- S <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
TOS にある " Pilot Officer Dragon Fly  " という文字列から、
文頭および文末の空白文字列を削除しています。
<[code-begin]>
&gt; " Pilot Officer Dragon Fly " trim "'%s'\n" putf
'Pilot Officer Dragon Fly'
  ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状態が想定されているものと異なる場合は
エラーが発生します。
<[/ddt]>
<[dl-table-hr-end]>

## 文字列の検査
文字列がとある文字列で始まっているのか、
もしくは特定の文字列で終わっているのかを
調べるワードも存在します。

### 文字列の最初と末尾に対する検査
start-with&quest; はセカンドにある文字列が
TOS に格納されている文字列で
開始しているか否かを調べるワードです。
end-with&quest; では TOS に格納されている
文字列で終わっているか否かを調べます。
いずれのワードも開始されている、もしくは、
終了していれば true を、
さもなければ false をスタックに積みます。

なお、
いずれのワードにも参照形が存在しています。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref is_start-with start-with&quest;]> ,
<[ref ref_is_start-with @start-with&quest;]> ,
<[ref is_end-with end-with&quest;]> ,
<[ref ref_is_end-with @end-with&quest;]> 
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
start-with&quest; および end-with&quest; : S S --- B <[br]>
@start-with&quest; および @end-with&quest; : S1 S2 --- S1 S2 B
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では、スタックにある文字列 "sample.p8e" が
".p8e" で終わっているか否かを調べています。
<[code-begin]>
&gt; "sample.p8e" ".p8e" end-with? .
  true ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状態が想定されているものと異なる場合は
エラーが発生します。
<[/ddt]>
<[dl-table-hr-end]>

### 正規表現を用いた検査
先頭もしくは末尾のみを対象とするのではなく、
文字列中にとある文字列が含まれるか、
また、文字列の含有のみならず、
対象の文字列の書式までを含めた検査を行うため、
正規表現を用いた検査を行う search というワードも存在します。

search はセカンドにある文字列に対し、
TOS にある正規表現を用いてマッチする情報を取り出し、
後述する配列として、その結果をスタックに積みます。
適合する文字列が存在した場合、
配列の 0 番目にはパターンに適合する文字列の全体が
格納されます。
パターン中に少括弧「(」および「)」 が存在する場合、
配列の 1 番目以降に順次それら部分的に適合する
文字列が格納されます。

本ワードは C++ の std::regex を活用して
実装されています。
<[ref 正規表現の文法]>は、C++ の std::regex の
説明を参照して下さい。

なお、配列の最後の要素は適合しなかった残りの文字列が
格納されます。

マッチするものがなければ無効な値をスタックに積みます。
参照形は @search になります。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref search]> , <[ref ref_search @search]>
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
search : S S --- &lcurly; A &verbar; O &rcurly; <[br]>
@search : S1 S2 --- S1 &lcurly; A &verbar; O &rcurly;
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では、積まれている文字列が
郵便番号の書式であるか否かを確認しています。
<[code-begin]>
&gt; "441-3124" '^\d{3}-\d{4}$' search .
  (size=1 data=0x7f8770600288)
  [0]=441-3124
ok.
<[code-end]>
この例ではワードの実行後の TOS が無効な値ではないので、
郵便番号の書式で書かれていることが分かります。
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状態が想定されているものと異なる場合は
エラーが発生します。
<[/ddt]>
<[dl-table-hr-end]>

## 書式指定の文字列生成
少し変わった文字列変換を行うワードとして、
とある書式に基づいて各種の値を文字列中に埋め込む
sputf があります。

これはセカンドにある値を TOS にある書式指定を含む文字列にて
清書し、清書後の文字列をスタックに積むワードです。
書式として利用できるものは C 言語の printf と同じです。
ちなみに putf は sputf . と同値です。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref sputf]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> X S --- S <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
TOS にある 123 という値を 全 8 桁でかつ、
0 詰めの文字列として変換された文字列が
スタックに積まれる様子を示しています。
<[code-begin]>
&gt; 123 "SCORE:%08d" sputf show
      +-------------------------+
TOS-->| string 'SCORE:00000123' |
DS:---------------------------------
 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状態が想定されているものと異なる場合は
エラーが発生します。
<[/ddt]>
<[dl-table-hr-end]>

## 文字列の置き換え
replace-str というワードでは、
スタックの上から 3 番目に格納されている文字列を
セカンドに格納されている正規表現で検査し、
マッチした部分を TOS に格納されている文字列にて置換します。
セカンドに格納されている文字列は
正規表現を表す必要があるため、
例えば括弧一つを指定する場合などは、
\\(" 等としなければなりません。

なお、本ワードは C++ の std::regex を用いて
実装されているため、
セカンドおよび TOS に格納されている文字列で使用する
正規表現の文法については、
std::regex および std::regex&#95;replace に関する
文書を参照して下さい。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref replace-str]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> S S S --- S <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では、C++ の std::regex&#95;replace のサンプルコード
<[url https://en.cppreference.com/w/cpp/regex/regex_replace]>
にある、母音に括弧を付ける処理を本ワードにて実施したものです：
<[code-begin]>
&gt; "Quick brown fox" "a|e|i|o|u" "[$&]" replace-str .
  Q[u][i]ck br[o]wn f[o]x ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状態が想定されているものと異なる場合は
エラーが発生します。
<[/ddt]>
<[dl-table-hr-end]>

## 文字列の分解
文字列を分解するワードはいくつか存在します。
split-str-by ではセカンドにある文字列を
TOS にある文字列を区切り文字として分割し、
それら分割された文字列からなる配列をスタックに積みます。
区切り文字として指定する文字列は 1 文字からなる
文字列として下さい。

分割対象となる文字列中に区切り文字が連続する場合、
その部分では空の文字列が生成されます。
詳しくは以下の使用例を見て下さい。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref split-str-by]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> S S --- A <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では、"apple,banana,,DAIKON" という文字列を、
カンマで分割する様子を示しています：
<[code-begin]>
&gt; "apple,banana,,DAIKON" "," split-str-by .
 array:(size=4 data=0x10ee042c0)
[0]=apple
[1]=banana
[2]=
[3]=DAIKON
 ok.
<[code-end]>
生成された配列の 3 番目の文字列
（インデックスとして 2 の文字列）が空の文字列になっています。
これは、banana,,DAIKON という部分について
banana,（長さ 0 の空文字列),DAIKON と認識するためです。
空文字列を生成することを避けるためには、
ワード replace-str 等を用いて、
連続する区切り文字を単一の区切り文字に置き換えるなどの
前処理を行って下さい。

もしくは単に空白文字列で切り出したい場合は、
後述するワード &gt;array を使うと便利です。

なお、分割結果として表示されている配列情報の
16 進数部分（data=...の部分）は、
実行環境により異なります。
そのため、必ずしも上記の値になるとは限りません。
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状態が想定されているものと異なる場合は
エラーが発生します。
<[/ddt]>
<[dl-table-hr-end]>

文字列を文字コードに分解するワードも存在します。
make-literal-code-list は文字列を UTF8 でエンコード
されているものと仮定して、
文字コードの列を後述するリストの要素として格納します。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref make-literal-code-list]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> S --- R <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
文字列 "日本語" から、
それぞれの文字コードからなるリストを生成しています。
<[code-begin]>
&gt; "日本語" make-literal-code-list .
 ( 15112101 15113388 15248030 ) ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状態が想定されているものと異なる場合は
エラーが発生します。
<[/ddt]>
<[dl-table-hr-end]>

UTF8 ではなく、単にバイト列として取り出すワードも
存在します。
str-to-list-byte がそのワードです。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref str-to-list-byte]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> S --- R <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
文字列 "ABC" から、 それぞれの文字コードである
65, 66, 67 という数値からなるリストを 生成しています。
<[code-begin]>
&gt; "ABC" str-to-list-byte .
  ( 65 66 67 ) ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状態が想定されているものと異なる場合は
エラーが発生します。
<[/ddt]>
<[dl-table-hr-end]>

## 文字コードの変換
Paraphrase の言語処理形である para は、
Linux や MacOS といったいわゆる Unix 系の OS だけではなく、
Windows にも対応するマルチプラットフォーム対応な
言語処理系です。
Linux や MacOS では UTF8 が標準的な文字コードになっている
感がありますが（筆者の印象です）、
Windows ではまだまだシフト JIS（SJIS）
の需要もあると感じています。

日本語部分が SJIS で記述された
Paraphrase のプログラム（以下、単にプログラムと呼びます）を
SJIS 環境下で実行する場合には
何ら文字コードを意識する必要はありません。
そのまま問題無く実行できます。

UTF8 の場合も、つまり UTF8 で記述された
プログラムを UTF8 環境にて実行する場合も
同じく何ら文字コードを意識する必要はありません。

問題は、SJIS で記述されたプログラムを
UTF8 環境下で実行する場合や、
その逆に、UTF8 で記述されたプログラムを
SJIS 環境下で実行する場合には
出力時に文字コードの変換が必要となります
（文字列を変換せずそのまま出力すると
いわゆる文字化けした状況になってしまいます）。

このような状況を回避するため、
Paraphrase には出力時に
文字コード変換を行う仕組みが備わっています。
それを<[ref 文字コード変換モード]>
（<[ref CCC]> = Character Code
Conversion Mode）と呼びます。
ここではこの文字コード変換モードに関する
ワードを紹介します。

まずは変換モードの確認をするためのワードの紹介です。
ccc-str は変換モードを表す文字列をスタックに積みます。
CCC が無変換（=thru）のときは THRU を、
UTF8 から SJIS へ変換する場合は utf8&gt;sjis を、
逆に SJIS から UTF8 へ変換する場合は sjis&gt;utf8 なる文字列を
スタックに積みます。
なお、デフォルトの CCC は thru に設定されています。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]><[ref ccc-str]><[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> --- S <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
<[code-begin]>
&gt; ccc-str .
 TRHU ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
エラーは発生しません。
<[/ddt]>
<[dl-table-hr-end]>

変換モードを変更する場合には、
utf8&gt;sjis や sjis&gt;utf8 ならびに thru という
ワードを使用します。
それぞれ ccc-str で表示される変換モードと
同名の変換モードとなるように Paraphrase の
出力部分のモードを変更します。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref utf8&gt;sjis]> , <[ref sjis&gt;utf8]> , <[ref thuru]> 
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> --- <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
使用例はありません。
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
エラーは発生しません。
<[/ddt]>
<[dl-table-hr-end]>

