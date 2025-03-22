# 連想配列
Paraphrase の配列は、他の多くのプログラミング言語と同様に
配列の要素を示すインデックス値として整数値を用いるものでした。
この章で説明する連想配列は、このインデックス値を
整数値以外の値にも拡張したものです。
文字列はもちろん、他の値もインデックスとして用いることができます。

## 連想配列の作成
連想配列の作成はワード new-assoc にて行います。
連想「配列」とは言っていますが、
new-array 等で作成される配列とは異なり、
サイズを指定する必要はありません
（必要に応じて自動的に配列の要素数は増えていきます）。
そのため、new-assoc を実行するだけで、
TOS に連想配列が積まれます。
<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref new-assoc]>
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]> --- H <[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では連想配列を作成し、
"Xevious" というキーに対して "NAMCO" という値を紐付ける
連想配列要素を格納しています。
<[code-begin]>
&gt; new-assoc
  ok.
&gt; "Xevious" "NAMCO" @set
  ok.
&gt; show

      +---------------------------+
TOS--&gt;| assoc ( &gt;Xevious,NAMCO&lt; ) |
DS:-----------------------------------
 ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
エラーは発生しません。
<[/ddt]>
<[dl-table-hr-end]>

## 連想配列への代入と取得
通常の配列と同様に set や @set 、
get 、 @get で連想配列への代入ならびに
格納されている値の取得ができます。

例えば、'threshold' というインデックス値に
123 という値を格納し、それを取得する場合の例は
次のようになります：
<[code-begin]>
 > new-assoc
 ok.
 > 'threshold' 123 @set
 ok.
 > 'threshold' @get .
 123 ok.
<[code-end]>

## キーの確認
連想配列に特定のキーが格納されているか否かを
確認したい場合もあろうかと思います。
has-key&quest; および @has-key&quest; でキーの確認ができます。
いずれのワードもセカンドにある連想配列は消費しません
（スタックからは取り除きません）。

has-key&quest; と @has-key&quest; の違いについてですが、
TOS に存在している確認対象のキーを取り出すか、
取り出さないかという部分の挙動の差のみということになります。

<[dl-table-hr-begin]>
<[dl-table-item ワード名]>
<[ddt]><[gt]>
<[ref has-key&quest;]> <[ref ref_has-key @has-key&quest;]>
<[/gt]><[/ddt]>
<[dl-table-item 実行レベル]>
<[ddt]>標準実行レベル<[/ddt]>
<[dl-table-item スタックの変化]>
<[ddt]>
has-key&quest; : H X --- H B <[br]>
@has-key&quest; : H X --- H X B <[br]>
<[/ddt]>
<[dl-table-item 使用例]>
<[ddt]>
次の例では連想配列を作成し、
"Xevious" というキーおよび "Mappy" というキーの
存在を確認しています。
<[code-begin]>
&gt; new-assoc
  ok.
&gt; "Xevious" "NAMCO" @set
  ok.
&gt; "Mappy" has-key&quest; .cr
  false
  ok.
&gt; "Xevious" has-key? .cr
  true
  ok.
<[code-end]>
<[/ddt]>
<[dl-table-item エラー]>
<[ddt]>
スタックの状態が想定されているものと異なる場合はエラーとなります。
<[/ddt]>
<[dl-table-hr-end]>

