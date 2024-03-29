# Paraphrase の概要
Paraphrase は Forth 系の言語です。
そのため少なからぬ影響を Forth から受けています。
その昔は随分と人気のあった Forth ですが、
今や古語といってもおかしくないような状況になっています。

本書を読むための準備として、
ここでは今や知る人も少なくなってしまった
Forth 由来の概念について簡単に説明を行います。

## 値とワードについて
Paraphrase のプログラムは文字列として入力されます。
入力された文字列は原則として空白により分割され、
これら分割された文字列毎にプログラム片として
解釈されていきます。
Forth 系の Paraphrase では Forth と同様に
これらの部分文字列の解釈を行っていきますが、
Forth と完全に同じ手順ではありません。

入力された文字列は以下の順序で解釈されます。
まず、引用符で囲まれた文字列であるか否かを確認します。
ここで言う文字列とはプログラムを表す文字列ではなく、
プログラム中で値としての文字列として用いられるものを
指しています。
もし文字列であったならばそれらを括っている
引用符と共に切り出された部分文字列として以後の
処理を行います。

文字列でなかった場合は空白
（厳密に言えば空白とみなされるタブ文字などを含んだ
いわゆるホワイトスペース）にて
プログラムとして表されている文字列を分割して
以後の処理を行います。

プログラムを示す文字列から取り出された
プログラム片としての部分文字列に対し、
まずワードであるか否かを調査します。
現在利用できるワードの集合に
同名のワードが存在する場合はワードとして処理を行います。

なお、ワードというのは Forth のワードと同じ意味で、
一般のプログラミング言語でいうところの
関数やサブルーチンのようなものです。
この「ようなもの」という表現ですが、
多くのプログラミング言語では
キーワードとして提供されている if や
for といったものですら Forth では
ワードにて提供されているためです。

話を元に戻します。
もし、対象の部分文字列が利用できるワードとして
存在しない場合は値としての処理を行います。
対象の部分文字列が引用符で開始されているのであれば
おそらく文字列を表す値であろうと推測し、
文字列データとして問題がないかを確認しつつ
問題がなければ文字列値として処理を行います。

バッククォートで始まる場合はシンボルとして
処理を行います。

それ以外のものは数値データとしての解釈を行います。
Forth では様々な基数による進法が利用できましたが、
Paraphrase では 10 進数と 16 進数にのも対応している
状況です。

厳密に言えば Paraphrase には実行レベルに関する閾値が
存在しており、部分文字列の解釈に影響を及ぼします。
例えば、リストを構成中はシンボルモードと呼ばれる
実行レベルの閾値が存在し、
その場合はワードと同名の文字列においても
シンボルと解釈される場合があります。
「場合があります」というのは、
同名のワードが実行レベル 2 以上である場合には
シンボルとは解釈されず、そのワードを実行するからです。

## モード
Paraphrase には Forth と同様にモードが存在します。
Forth にはインタプリタモードとコンパイルモードが
存在しており、これらは Paraphrase にもあります。
インタプリタモードは入力された値をスタックに積み、
ワードの場合はそのワードを呼び出します。
一方、コンパイルモードの場合は、
値についてはその値をスタックに積むコードが生成され、
ワードについてはそのワードを呼び出すコードが生成されます。

例として、2 つの値をスタックから取り出し、
その和をスタックに積むワード + を用いた
プログラム 1 2 + というものを考えてみます。

インタプリタモードではこれらは順次処理され、
つまり 1 と 2 という値を（データ）スタックに積み、
その後 + というワードが呼び出されこれら 2 つの値が取り出され、
結果としてそれらの和である 3 という値がスタックに積まれます。

コンパイルモードでもこれらは順次処理されますが、
1 という値をスタックに積むコードが生成され、
次に 2 という値をスタックに積むコードを生成し、
最後に + というワードを呼び出すコードが生成されます。
結果としてスタックは何ら変化しない、という状況となります。

このような動作を行うワードを
<[ref standard-level 標準実行レベル]>のワードと呼びます。

### 即時実行属性
Forth の興味深い特徴として即時実行属性があります。
これはワードに関する属性です。
この属性を持つワードはコンパイルモードであっても
コンパイルされず、インタプリタモードの時のように
（即時に）呼び出されます。

この属性があるため、Forth では条件分岐や
ループ構造までもワードで実現することができています。
例えば条件分岐を行うワード if 等は即時実行属性を有しており、
コンパイル中であっても即時にワード if が呼び出され、
この if というワードにより
条件分岐処理に関する処理が生成されます。
この属性によりワードは、コンパイル中における
マクロのような働きを実現しています。

この即時実行属性に関する機構を Paraphrase も有しています。

### シンボルモード
このように Forth ではインタプリタモードとコンパイルモードという
ある意味 2 値的なモードでしたが、
Paraphrase ではこれを整数値へと拡張し、
2 値ではなく複数のモードへ対応できるようになっています。
現状ではシンボルモードと呼ばれるモードを拡張したのみと
なっていますが、
このモードを活用して
リストの構築や注釈（コメント）処理を行っています。

モードの整数値化に伴い、
実行レベルという概念が Paraphrase には導入されています。
実行レベル 0 がインタプリタモードであり、
実行レベル 1 がコンパイルモード、
2 がシンボルモードとなっています。
それぞれのワードにも実行属性ではなく、
実行レベルという属性が付与されており、
現在の実行レベルとワードの実行レベルが等しいか
それ以上であればモードに関わらず実行されるようになっています。

これは Forth における実行属性の自然な拡張です。
例えば Forth における通常の属性
（即時実行属性ではないワード）は
<[ref level-zero 実行レベル0]> となり、
シンボルモードでは実行されない
即時実行属性のワードは
<[ref level-one 実行レベル1]> のワードとなります。

Paraphrase には<[ref level-two 実行レベル2]> の
ワードが存在しており、
この実行レベルを持つワードはシンボルモードであっても
即時に実行されます。
コメントの終了を表すワード /* 等がこの属性を有しています。

