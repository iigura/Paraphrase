# リリースノート：バージョン 0.93 から 0.94 の変更点

## 言語そのものに関する変更点
* デバッガの追加
* 擬似標準入出力機能の追加
* アスペクト志向プログラミングへの対応
* ワードへの属性情報の追加
* 連想配列の追加
* 無効な値の追加（ユーザーへの開放）
* 相互参照機能の追加
* ワードの仮引数を支援する機能の追加
* ヒアドキュメント機能の追加

## 既存のワードに対する変更点
* 各種ループ処理の一貫性向上
* switch 〜 dispatch ブロックの仕様変更  
     ワード case が必須となりました。
* while 〜 repeat ブロックの仕様変更  
     ワード do が必要となりました（以前の do とは異なる同名のワード）。
* for+, for- ループの仕様変更  
     終了値を含まないような仕様に変更となりました。
* 機能を追加したワード  
     last, @last - 配列にも対応しました。  
     set, get - 連想配列にも対応しました。  
     exec - 標準コードにも対応しました。  
     map, filter - ワード i に対応しました。  
     i - map, filter, find, reduce にも対応しました。
* 機能が変わって再登場したもの  
     do

### 名称の変更
* ワードの接頭語に関する名称変更  
     valid? を @valid? へ  
     type を @type へ
* ワードの使用頻度に起因する名称変更  
     local を local-decl へ  
     local-with を local へ

## 新たに追加されたワード
* &lt;&lt;&lt;
* &gt;&gt;&gt;
* &gt;&gt;&gt;raw
* &gt;array
* &gt;mock-stdin
* &gt;param
* ...
* +@
* ++
* -@
* --
* &ast;@
* /@
* %@
* &#095;b
* &#095;c
* &#095;cb
* &#095;n
* &#095;s
* @even?
* @has-key?
* @invalid?
* @list?
* @word-name
* 2drop-rs
* add-enter-char
* at
* clear-breakpoint
* cont
* default
* default-docol
* defined?
* docol
* end-with?
* enum
* even?
* find
* get-attr
* get-line-from-mock-stdout
* get-std-code
* has-any-attr?
* has-attr?
* has-key?
* invalid
* invalid?
* max
* min
* new-assoc
* not-true?
* raw&lt;&lt;&lt;
* reduce
* remove-attr
* return
* search
* set-attr
* set-breakpoint
* set-code
* set-worded-docol-caller
* show-attr
* show-trace
* start-with
* step-over
* step-in
* troff
* tron
* update
* use-mock-stdin
* use-mock-stdout
* use-stdin
* use-stdout
* valid?
* word-name
* worded-docol-caller

## 処理系 para について
特に変更はありません。

## 同梱されるサンプルプログラムについて
* Project Euler No.56 を計算するサンプルプログラム PE56.pp を追加しました  
    PE56 は、a,b をそれぞれ 1 以上 100 未満の数とし、a^b を 10 進数で表現したとき、各桁の数字の合計が最大になるものを計算するサンプルプログラムです。
* サンプルプログラムのファイル名の変更  
     PE21mt-short.pp を PE21mt.pp へと変更しました。
* サンプルプログラムの単体テストへの対応  
     各サンプルプログラムが test という引数に対応するようにしました。

