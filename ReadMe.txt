-------------------------------------------------------------
　I2DXライブラリ
                              (c)opyright SNT Studio


                                                 ver 1.01
　ReadMe れあどめ～　　　　　　　　　　SNT Studio 代表 SR
-------------------------------------------------------------

Scroll down for the English version.

■１．はじめに挨拶
　この度は、ITDXライブラリをダウンロードしていただき、誠にありがとうございます。
　
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
　このライブラリは、Windowsでのみ動作確認をしております。
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
　・1.00
　　ライブラリのファイル、ReadMe、HowToUseを追加しました。
　
　・1.01
　　WavSoundで、ループ範囲を秒数で指定できるようにしました。
　　ReadMeに加筆しました。
　
■５．連絡先など
　サークルのeMail: sntstudio.ori@gmail.com
　私個人のeMail: sr.ori.jp@gmail.com
　ホームページ: https://snt-studio.jimdofree.com/
　note: https://note.com/prime_sntstudio
　GitHub: https://github.com/sntstudioori-ux/I2DX
　eMailは、個人の方に連絡したほうが、反応がある確率が高いです。

■English version
　＊Note:
　This text has been translated by a Japanese speaker using generative AI.
　It may contain errors.
　I Recommend to Read Japanese Version.
　Updates to the English version will be delayed.
　
■1. Introduction / Greeting
　Thank you very much for downloading the ITDX library.
　
　This library may possibly, maybe, be of use to you.
　The reason for this vague wording is that I've only implemented the things I personally wanted.
　
　For detailed usage instructions, please read HowToUse.md.
　It doesn't cover everything, and some parts are a bit out of order, so please bear with it.
　It's been made with simplicity in mind, so I think reading the code should make most things clear.

■2. What is this library?
　It's a library that makes DirectX11 (slightly) more convenient to use with C++.
　It was created mainly to assist with making Windows games.
　Please also read HowToUse.md for detailed features.
　
　It's an ultra-lightweight library built with a focus on being light and simple.
　You can also choose which files to #include, so you can exclude features you don't need to make it even lighter.
　
　Being lightweight is the flip side of having fewer features.
　That said, since it's something I use myself, I think it's reasonably practical.

■3. Things We'd Like You to Understand
　This library has only been verified to work on Windows.
　Based on its specifications, it should work on Windows 7, Windows 10, and Windows 11.
　Also, to use the 3D features, a Direct3D-compatible GPU is required.
　
　This library may contain bugs. In fact, it probably does.
　If you find a bug, I'd appreciate it if you either contact me and ask for a fix, or fix it yourself.
　It would be even more appreciated if you could submit your fix to my GitHub.
　Even if a fix request comes in, I can't guarantee I'll fix it, but I'll do my best to respond.

■4. Terms of Use
　By publishing a work that uses this library, or a modified version of this library,
　you are considered to have agreed to these Terms of Use.
　
　I bear no responsibility whatsoever for any damages resulting from the use of this library.
　I don't believe there's anything strange included, but please use it at your own risk.
　
　This library may be freely modified.
　Redistribution is permitted only if meaningful modifications (changes to the source code, addition/removal of features, etc.) have been made to part of this library.
　Redistributing this library without modification, or with only essentially meaningless modifications (such as changes to comments), is prohibited.
　If you make a fairly significant modification and release it publicly, I'd appreciate it if you did so as a fork on GitHub.
　In that case, please also contact the author.
　
　If you want to pass the original data on to someone else, please do so by, for example, providing a link to GitHub.
　However, unmodified redistribution is permitted only in the case where all distribution sites for this library have disappeared.
　
　When publishing a work that uses this library, or a modified version of the library, please include a copyright notice.
　The notice can be written anywhere—in a Special Thanks section, at the end of a ReadMe, or elsewhere—it doesn't matter.
　For the notice, the standard "Copyright (c) 2026 SNT Studio" or "Copyright (c) 2026 SR" is recommended.
　This notice format may change in the future.
　
　Additionally, these Terms of Use may be changed without prior notice.
　In principle, any revised license will apply only to versions released with that revised license attached, going forward.

■5. Contact Information, etc.
　Circle eMail: sntstudio.ori@gmail.com
　Personal eMail: sr.ori.jp@gmail.com
　Website: https://snt-studio.jimdofree.com/
　note: https://note.com/prime_sntstudio
　GitHub: https://github.com/sntstudioori-ux/I2DX
　You're more likely to get a response by contacting the personal eMail address.
