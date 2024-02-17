# Paraphrase ver. 0.97
Copyright (c) 2018-2024 Koji Iigura  
Released under the MIT license  
[https://opensource.org/licenses/mit-license.php](
https://opensource.org/licenses/mit-license.php)


## マルチコア対応 Forth 系言語
Paraphrase はマルチコアに対応した Forth 系言語です。  
スレッド間通信もサポートしており、
並列処理も簡単に記述できます。

    // サンプルプログラム： 1 から 1,000 万までの素数を数える
    `prime? :
        switch
            case 1 ==    -> false
            case 2 ==    -> true
            case 2 % 0?  -> false
            default ->> dup sqrt ceil >int 1+ 3 swap
                        for+
                            dup i % 0? if drop false return then 2 step
                        next
                        drop true
        dispatch
    ;

    reset-pipes

    // 奇数については並列処理で調べる
    [ /* scatter */  3 10000000 for+ i >pipe 2 step next ]>>

    // 複数スレッドで検証作業（=woker thrread)
    >>[[
        while-pipe
            dup prime? if >pipe else drop then
        repeat
    ]]>

    // gather
    () 2 dup prime? if append then // 2 については、ここで調査
    while-pipe @append repeat { < } sort
    ( "numOfPrimes=%d (%d ... %d)\n" {{ @size }}, {{ @car }}, {{ last }}, ) printf

上のプログラムの出力

    numOfPrimes=664579 (2 ... 9999991)


## マルチプラットフォーム対応
Windows, Linux, MacOS に対応しています。


## すぐ使える：ビルド済みパッケージを使う場合
ダウンロードして解凍（展開）するだけです。  
インストール不要で、すぐに実行できます。

以下のリンク（Google Drive）をクリックし、
それぞれのプラットフォーム用のパッケージを入手して下さい。

[https://drive.google.com/drive/folders/18c2NPG09uOakcBnqGM43bLHMD90hGxVZ](
https://drive.google.com/drive/folders/18c2NPG09uOakcBnqGM43bLHMD90hGxVZ)

Windows 版 para-0-97-win.zip  
Linux 版 para-0-97-linux.tar.gz  
MacOS 版 para-0-97-mac-M1.tar.gz  

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

