-------------------------------------------------------------
　I2DXライブラリ
                              (c)opyright SNT Studio


                                                 ver 1.02
　ReadMe れあどめ～　　　　　　　　　　SNT Studio 代表 SR
-------------------------------------------------------------

Scroll down for the English version.

■１．はじめに挨拶
　この度は、I2DXライブラリをダウンロードしていただき、誠にありがとうございます。
　
　このライブラリは、多分メイビーあなたの役に立つ可能性があります。
　曖昧な言い方をしている理由は、私が欲しいと思ったものだけを実装しているからです。
　
　詳しい使い方は、HowToUse.mdをお読みください。
　全部は解説しきれていませんし、順番がごちゃごちゃな部分がありますが、ご容赦ください。
　シンプルを意識して作っているので、コードを読めばだいたい分かると思います。
　
■２．このライブラリって何？
　DirectX11を、C++で（ほんの少し）便利に扱えるようにしたライブラリです。
　主に、Windows向けのゲーム制作を補助する目的で作られました。
　詳しい機能もHowToUse.mdをお読みください。
　
　軽量＋シンプルであること主眼においた、超軽量ライブラリになっています。
　また、#includeするファイルを選べるので、必要のない機能を除外して、さらに軽量化を図ることもできます。
　
　軽量ということは、機能が少ないことの裏返し。
　ただ、自分が使うものなので、ある程度実用に足るものにはなっていると思います。
　
■３．ご理解いただきたいこと
　このライブラリは、Windows11でのみ動作確認をしております。
　仕様上、Windows7, Windows10, Windows11で動作すると思われます。
　また、3D機能を使用するためには、Direct3Dに対応したGPUが必要です。
　
　アップデートを行う際は、極力前のバージョンとの互換性を持たせます。（そのままライブラリだけ差し替えられるように）
　ですが、時折互換性のないアップデートを行わざるを得ないときがあるかもしれません。
　その時はReadMeの更新内容に書いておきますので、アップデートのときは確認をお願いします。
　
　このライブラリは、バグを含む可能性があります。多分含んでいます。
　もしバグを見つけた場合は、私に連絡を入れて修正を頼むか、自力で修正していただけるとありがたいです。
　修正したやつを私のGitHubのとこに入れてもらえると、もっとありがたいです。
　修正依頼が来ても、私が修正するかはわかりませんが、できる限り対応を行います。
　
■４．利用規約
　このライブラリを使用した作品、または改変したライブラリを公開した時点で、
　この利用規約に同意したものとみなします。
　
　このライブラリを使用したことにより、いかなる損害が発生したとしても、私は一切責任を負いません。
　変なものは入っていないと思いますが、自己責任で使用してください。
　
　このライブラリは自由に改変が可能です。
　このライブラリの一部に意味のある改変（ソースコードの変更、機能の追加・削除など）をした場合に限り、再配布を認めます。
　
　このライブラリを改変せずに、もしくはほぼ実質的な意味のない改変（コメントの改変など）しかせずに、再配布することは禁止します。
　ある程度大きな改変をして、一般公開する場合は、GitHubでのフォークにしていただけるとありがたいです。
　その際は、作者への連絡もお願いします。
　
　元データを誰かに渡したい場合は、GitHubのリンクを教えるなどしてください。
　ただ、このライブラリの配布サイトがすべて消失した場合に限り、未改変での再配布を許可します。
　
　このライブラリを使用した作品、または改変したライブラリを公開する場合は、著作権表記を行ってください。
　表記の仕方は、Special Thanksに書くでも、ReadMeの末尾に書くでもなんでも構いません。
　表記は、一般的な「Copyright (c) 2026 SNT Studio」か「Copyright (c) 2026 SR」を推奨します。
　表記は、将来的に変わる可能性があります。
　
　また、この利用規約は事前の告知無しで変更される可能性があります。
　変更後のライセンスが適用されるのは、原則として変更後のライセンスが付属されたバージョン以降とします。
　
■６．変更点
　・1.02
　　バグを誘発しそうな部分を沢山修正しました。
　　Githubへの同期ミスにより、コンパイルが通らなくなっていた不具合を修正しました。
　　誤字をいくつか修正しました。
　　ReadMe.txtの変更点のバージョンの並べ方を変えました。
　
　・1.01
　　WavSoundで、ループ範囲を秒数で指定できるようにしました。
　　ReadMeに加筆しました。
　　
　・1.00
　　ライブラリのファイル、ReadMe、HowToUseを追加しました。
　
■５．連絡先など
　サークルのeMail: sntstudio.ori@gmail.com
　私個人のeMail: sr.ori.jp@gmail.com
　ホームページ: https://snt-studio.jimdofree.com/
　note: https://note.com/prime_sntstudio
　GitHub: https://github.com/sntstudioori-ux/I2DX
　eMailは、個人の方に連絡したほうが、反応がある確率が高いです。

■English version
　＊Note:
　This English version was translated from the Japanese version by a Japanese speaker with the help of generative AI.
　It may contain errors.
　I recommend reading the Japanese version.
　Updates to the English version may be delayed.
　
■1. Introduction / Greeting
　Thank you very much for downloading the I2DX library.
　
　This library may, perhaps, be useful to you.
　The reason for this vague wording is that I have only implemented the things I personally wanted.
　
　For detailed usage instructions, please read HowToUse.md.
　It does not explain everything, and some parts may be out of order, but please bear with it.
　It was designed with simplicity in mind, so reading the code should make most things understandable.
　
■2. What is this library?
　This is a library that makes DirectX 11 slightly more convenient to use with C++.
　It was created mainly to assist with Windows game development.
　Please also read HowToUse.md for details about its features.
　
　It is an ultra-lightweight library focused on being lightweight and simple.
　You can also choose which files to #include, allowing you to exclude unnecessary features and make it even lighter.
　
　Being lightweight is the flip side of having fewer features.
　That said, since it is something I use myself, I believe it is practical enough for general use.
　
■3. Things We Would Like You to Understand
　This library has only been tested on Windows 11.
　Based on its specifications, it is expected to work on Windows 7, Windows 10, and Windows 11.
　A Direct3D-compatible GPU is also required to use the 3D features.
　
　When updating the library, we try to maintain compatibility with previous versions as much as possible, so that the library can be replaced directly.
　However, there may occasionally be cases where an incompatible update is unavoidable.
　If that happens, it will be described in the changes listed in the ReadMe, so please check them when updating.
　
　This library may contain bugs. In fact, it probably does.
　If you find a bug, I would appreciate it if you contacted me to request a fix or fixed it yourself.
　I would be even more grateful if you submitted your fix to my GitHub.
　Even if I receive a request for a fix, I cannot promise that I will implement it, but I will do my best to respond.
　
■4. Terms of Use
　By publishing a work that uses this library, or a modified version of it, you are considered to have agreed to these terms of use.
　
　I accept no responsibility whatsoever for any damages caused by using this library.
　I do not believe that it contains anything unusual, but please use it at your own risk.
　
　This library may be freely modified.
　Redistribution is permitted only when meaningful modifications—such as source-code changes or the addition or removal of features—have been made to some part of this library.
　
　Redistribution without modification, or with only essentially meaningless modifications such as comment changes, is prohibited.
　If you make a reasonably substantial modification and publish it, I would appreciate it if you did so as a fork on GitHub.
　Please also contact the author in that case.
　
　If you want to give the original data to someone, please provide a link to GitHub or use a similar method.
　However, unmodified redistribution is permitted only if all distribution sites for this library have disappeared.
　
　When publishing a work that uses this library, or a modified version of it, please include a copyright notice.
　The notice may be placed anywhere, such as in a Special Thanks section or at the end of the ReadMe.
　The recommended notices are “Copyright (c) 2026 SNT Studio” or “Copyright (c) 2026 SR”.
　This recommendation may change in the future.
　
　These terms of use may also be changed without prior notice.
　In principle, a revised license will apply from the version that includes the revised license onward.
　
■6. Changes
　• 1.02
　　Fixed many areas that could have caused bugs.
　　Fixed an issue that prevented compilation due to a synchronization mistake with GitHub.
　　Corrected several typos.
　　Changed the order in which versions are listed in the ReadMe.txt change log.
　
　• 1.01
　　Added support for specifying the loop range in seconds in WavSound.
　　Added more information to the ReadMe.
　
　• 1.00
　　Added the library files, ReadMe, and HowToUse.
　
■5. Contact Information, etc.
　Circle eMail: sntstudio.ori@gmail.com
　Personal eMail: sr.ori.jp@gmail.com
　Website: https://snt-studio.jimdofree.com/
　note: https://note.com/prime_sntstudio
　GitHub: https://github.com/sntstudioori-ux/I2DX
　You are more likely to receive a response if you contact me at my personal eMail address.
