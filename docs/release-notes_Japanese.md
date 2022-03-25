# リリースノート：バージョン 0.94.0 から 0.95rc1 の変更点

## 主な変更
* 局所変数への代入を参照に変更
* 無名ワードをメモリ管理対象へと変更
* デバッガのマルチスレッド対応
* @ から始まるシンボルの許可
* Paraphrase プログラムの拡張子を .p8e に変更
* Vim 用のシンタックスハイライトファイルを追加

## 既存のワードに対する変更点
* ワード定義時にシンボルをワード名として使用できるようになりました
* switch - dispatch ブロックにおいて必ずしも break を使う必要がなくなりました
* case , -&gt; , -&gt;&gt; , dispatch が cond に対応しました
* default が case true と同値になりました
* docol が即時実行ワードになりました
* ワード i が foreach 内でも機能するようになりました
* いくつかのワードがシンボルに対応しました
    alias , def , update , defined? , forget , set-breakpoint , _b
* dump が連想配列とシンボルに対応しました
* &gt;int , &gt;double が文字列に対応しました
* &gt;long , &gt;float が文字列とアドレス値に対応しました
* &gt;symbol が真偽値に対応しました
* &gt;word がシンボルとワードに対応しました
* 多段 load 時の機能強化を行いました
* de-list を expand へと名称変更し配列にも対応しました
* スカラ値とコンテナ内要素に対する == と != の挙動を統一しました
* 文字列に対するワード . （ドット）の挙動を変更しました
* reduce , 1+ , 1- , 2/ を廃止しました
* 以下のワードに対しデバッグを行いました：
    alias , printf , &gt;array , dup , for- , &gt;int , car , @car , cdr , @cdr<br>
    pop-front , &gt;long , docol , expand
    
### 名称変更
* 以下に示すワードの名前を変更しました
    | 旧名称         | 新名称          |
    |----------------|-----------------|
    | &gt;INT        | &gt;big-int     |
    | &gt;FLOAT      | &gt;big-float   |
    | &gt;str        | &gt;string      |
    | &gt;to-hex-str | &gt;hex         |
    | local          | local-decl      |
    | local-with     | local           |
    | type           | @type           |
    | type-name      | @type-name      |
    | empty-str?     | @empty-str?     |
    | not-empty-str? | @not-empty-str? |
    | valid-index?   | @valid-index?   |
    | pop-front      | @pop-front      |
    | add            | @push-back      |
    | append         | @append         |
    | get            | @get            |

## 新たに追加されたワード
* :( , );
* (< , >)
* ++ , -- , &lt;- , += , -= , &lowast;= , /= , %=
* +@ , -@ , &lowast;@ , /@ , %@
* &gt;array , &gt;assoc , &gt;bool , &gt;list
* &gt;int? , &gt;long? , &gt;float? , &gt;double?
* @set-attr
* @sqrt
* @type&gt; , &gt;type
* bool-type , int-type , float-type , double-type , word-type
* array? , list? , assoc? , @array? , @assoc? , string?
* int? , long? , big-int? , float? , double? , big-float?
* append , pop-front , push-back
* as-counter
* cond
* docol-target-word
* is?
* last-defined-word
* map-local , map-let
* otherwise-drop , otherwise-2drop
* panic
* replace-str , split-str-by , trim
* reverse-foreach
* round
* self
* set-delete-by-forget , clear-delete-by-forget , check-delete-by-forget
* set-random-seed
* sputf
* this-word-is-removed
* tron?
* when
* zip , min-zip , max-zip , max-zip-with

## 処理系 para について
* show-size オプションを追加
* 終了ステータスの変更（ヘルプやバージョンを表示しても正常表示となります）

## 同梱されるサンプルプログラムについて
* Lisp インタプリタ LisP8E を同梱
* 各サンプルプログラムにテスト機能を追加
* PE10mt.p8e, PolishNotation.p8e, fastInvSqrt.p8e, failure.p8e, pass.p8e を追加

