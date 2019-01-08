# Paraphrase ver. 0.9.2
Copyright (c) 2019 Koji Iigura  
Released under the MIT license  
[https://opensource.org/licenses/mit-license.php](
https://opensource.org/licenses/mit-license.php)


## マルチコア対応 Forth 系言語
Paraphrase はマルチコアに対応した Forth 系言語です。  
スレッド間通信もサポートしており、
並列処理も簡単に記述できます。

    // サンプルプログラム： 1 から 100 万までの素数を数える
    reset-pipes
    // 奇数については並列処理で調べる
    [ /* scatter */  3 10000000 do i >pipe 2 step loop ]
    [[  // 複数スレッドで検証作業（=woker thrread)
        while-pipe
            dup prime? if >pipe else drop then
        repeat
    ]]
    // gather
    () 2 dup prime? if append then // 2 については、ここで調査
    while-pipe append repeat
    ( "numOfPrimes=%d (%d ... %d)\n" ${ size } ${ car } ${ last } ) printf
    drop

上のプログラムの出力

    numOfPrimes=664579 (2 ... 9999991)

※ ワード prime? については、[countPrimeMT-short.pp](
http://github.com/iigura/paraphrase/blob/master/samples/countPrimeMT-short.pp) を
見て下さい。


## マルチプラットフォーム対応
Windows, Linux, MacOS に対応しています。


## すぐ使える：ビルド済みパッケージを使う場合
ダウンロードして解凍（展開）するだけです。  
インストール不要で、すぐに実行できます。

* Windows 版 [__PACK_FOR_WIN__](
http://github.com/iigura/paraphrase/releases/download/v0.9.2/__PACK_FOR_WIN__)
* Linux 版 [__PACK_FOR_LINUX__](
http://github.com/iigura/paraphrase/releases/download/v0.9.2/__PACK_FOR_LINUX__)
* MacOS 版 [__PACK_FOR_MAC__](
http://github.com/iigura/paraphrase/releases/download/v0.9.2/__PACK_FOR_MAC__)

展開して para （Windows 版は para.exe）を実行して下さい
（同梱されるファイルについては、
[FILES.txt](http://github.com/iigura/paraphrase/blob/master/FILES.txt) を
見て下さい）。

終了方法は、Linux や Mac の場合 Ctrl-D または Ctrl-C です。  
Windows では Ctrl-Z を入力後リターンキーを押すか、Ctrl-C で終了します。


## ソースコードからビルドする場合
git clone https://github.com/iigura/Paraphrase.git でクローン。  
ビルド方法の詳細は、
[BUILD.txt](http://github.com/iigura/paraphrase/blob/master/BUILD.txt) を
見て下さい。


## その他
サンプルプログラムの概要は samples ディレクトリの中の
[SAMPLES.txt](http://github.com/iigura/paraphrase/blob/master/samples/SAMPLES.txt) を
見て下さい。

開発者が日本人なので、質問などは日本語で OK。 
Paraphrase に関する情報は Twitter ( @[paraphrase_lang](
https://twitter.com/paraphrase_lang) ) や Facebook ( [Paraphrase ユーザー会](
https://www.facebook.com/groups/219684655627070/) ) にて発信中です。

バグレポートや要望・質問などは Facebook の方に書いて頂ければ助かります。

