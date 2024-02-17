# リリースノート：バージョン 0.96 から 0.97 の変更点

## 主な変更点
言語システムにおける大きな変更点はありません。

## 既存のワードに対する変更点
* args - args の変更がスクリプトの引数に反映するようにしました

### 名称変更
* fgets を @fgets へ
* var を global-decl へ

## 新たに追加されたワード
* global
* caller
* word-short-name , @word-short-name
* fcopy , fexist? , fwrite , mkdir

## 廃止されたワード
廃止されたワードはありません。

## 処理系 para について
* スクリプトファイルでもエラー発生行を表示するようにしました
* args が指すリストとスクリプトファイルへの引数とを同一にしました

## 同梱されるサンプルプログラムについて
* ドキュメントシステム Paradocs を追加しました
* ワード caller の使い方を示す caller.p8e を追加しました
* AOP を用いたユーザー定義の警告表示サンプル user-defined-deprecated.p8e を追加しました

## その他の変更
ありません。

