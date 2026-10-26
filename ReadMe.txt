-------------------------------------------------------------
　I2DXライブラリ
                              (c)opyright SNT Studio


                                                 ver 1.04
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
　再配布も可能ですが、再配布する物は何か改変を加えた物だけにしていただきたいです。
　まぁ、変更しなくても文句は言いませんが…ね？
　機能的な変更を加えていただきたいです。
　
　このライブラリを使用した作品、または改変したライブラリを公開する場合は、著作権表記を行っていただきたいです。
　表記の仕方は、Special Thanksに書くでも、ReadMeの末尾に書くでもなんでも構いません。
　なりにサークルの名前とかを書いてもらえると、喜びます。
　
　また、この利用規約は事前の告知無しで変更される可能性があります。
　変更後のライセンスが適用されるのは、原則として変更後のライセンスが付属されたバージョン以降とします。
　
■５．変更点
　・1.04
　　フルスクリーン時に、指定の解像度で描画されなくなっていた問題を修正しました。
　　なお、描画解像度の問題ですので、プログラム内の座標には影響はないと思われます。多分。
　　ついでに、ボーダーレスウィンドウに加えて、排他的フルスクリーンも使えるようにしました。

　・1.03
　　利用規約を緩くしました。
　　再配布の条件とかを…

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
　
■６．連絡先など
　サークルのeMail: sntstudio.ori@gmail.com
　私個人のeMail: sr.ori.jp@gmail.com
　ホームページ: https://sntstudioori-ux.github.io/
　note: https://note.com/prime_sntstudio
　GitHub: https://github.com/sntstudioori-ux/I2DX
　eMailは、個人の方に連絡したほうが、反応がある確率が高いです。

■English version
　＊Note:
　This English version was translated from the Japanese version by a Japanese speaker with the help of generative AI.
　It may contain errors.
　I recommend reading the Japanese version.
　Updates to the English version may be delayed.
　
Thank you very much for downloading the I2DX library.

This library may, maybe, possibly be useful to you.
The reason I'm being vague is that I've only implemented the things I wanted for myself.

For detailed usage, please read HowToUse.md.
It doesn't explain everything, and some parts are in a jumbled order, so please bear with me.
I've tried to keep things simple, so you should be able to figure most of it out by reading the code.

■2. What is this library?

It's a library that makes DirectX11 (just a little) more convenient to use from C++.
It was created mainly to assist with game development for Windows.
For details on its features, please also read HowToUse.md.

It's an ultra-lightweight library, with its main focus on being light and simple.
You can also choose which files to #include, so you can leave out features you don't need and slim it down even further.

Being lightweight is the flip side of having few features.
That said, since I made it for my own use, I think it's reasonably practical.

■3. Things I'd like you to understand

I have only verified that this library works on Windows 11.
Based on its specifications, it should work on Windows 7, Windows 10, and Windows 11.
Also, a Direct3D-compatible GPU is required to use the 3D features.

When updating, I'll maintain compatibility with previous versions as much as possible (so that you can simply swap in the new library files).
However, there may occasionally be times when I have no choice but to make an incompatible update.
In that case, I'll note it in the changelog in the ReadMe, so please check it when updating.

This library may contain bugs. It probably does.
If you find a bug, I'd appreciate it if you either contact me and ask me to fix it, or fix it yourself.
It would be even better if you could push your fix to my GitHub.
Even if you send me a fix request, I can't promise I'll fix it myself, but I'll do my best to respond.

■4. Terms of Use

By publishing a work that uses this library, or a modified version of this library, you are deemed to have agreed to these terms of use.

I take no responsibility whatsoever for any damages arising from the use of this library.
I don't think there's anything strange in it, but please use it at your own risk.

You are free to modify this library.
Redistribution is also allowed, but I'd like what you redistribute to be limited to versions in which you've made some modification.

If you publish a work that uses this library, or a modified version of this library, I'd like you to include a copyright notice.
The format is up to you: putting it in the Special Thanks, at the end of the ReadMe, or anywhere else is fine.

Also, these terms of use may be changed without prior notice.
As a general rule, the changed license applies only to versions that ship with the changed license.

■5. Changelog
・1.04
　Fixed an issue where the game would not render at the specified resolution when in full-screen mode.
　Note that since this is a rendering resolution issue, it should not affect the coordinates used within the program. (Probably.)
　Additionally, I have added support for exclusive full-screen mode, alongside the existing borderless window mode.

・1.03
　Relaxed the terms of use.

・1.02
　Fixed many parts that were likely to cause bugs.
　Fixed an issue where the code failed to compile due to a GitHub sync error.
　Fixed several typos.
　Changed the order in which versions are listed in the ReadMe.txt changelog.

・1.01
　Made it possible to specify the loop range in seconds in WavSound.
　Added to the ReadMe.

・1.00
　Added the library files, ReadMe, and HowToUse.

■6. Contact information, etc.

Circle email: sntstudio.ori@gmail.com
My personal email: sr.ori.jp@gmail.com
Homepage: https://sntstudioori-ux.github.io/
note: https://note.com/prime_sntstudio
GitHub: https://github.com/sntstudioori-ux/I2DX
You're more likely to get a response if you contact my personal email.