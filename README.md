AutoWah (AudioUnitEffect)
===
工事中。

## Demo
Comming Soon!!

## Requirement
GarageBand, AU LabなどのAudioUnitsが対応したソフトウェアで読み込むことで使えます。

## Usage
工事中。


+	`Range` :
バンドパスフィルタの有効範囲

+	`Resonance` :
ピークの強度


## Install
1. cloneしたディレクトリの中のDerivedData/killmachine/Build/Products/Development/にAutoWah.componentがあるか確認。なければXcodeを用いてAutoWah.xcodeprojを開けてBuildして生成してください。

2. AutoWah.componentを/Users/$USER/Library/Audio/Plug-Ins/Componentsに移す。

3. Terminal.appで"auval -a"を用いて"aufx KILL MUKS  -  Muks: AutoWah (Effect AU)"がリストに含まれているか確認。

4. リストに含まれていたら正しく読み込まれています。AudioUnitsに対応しているソフトウェアでお使いください。

## Other
中身を弄る際にデバッグをしたい方は以下の方法を用意しています。非常に簡単なものですが・・・。

* AutoWah.cppに#define DEBUG_PATH ""がコメントアウトされているので、""の中にディレクトリ内にあるdebug.txtのパスを書き込んでコメントアウトを外してください。
* 書き込む内容を変える際は、debugを利用してください。

* もしデバッグを外したい場合は先ほどの#define DEBUG_PATHをコメントアウトしてください。 

## Licence
Copyright &copy; 2015 muk
Distributed under the [MIT License][mit].
[MIT]: http://www.opensource.org/licenses/mit-license.php

## Author
muk:[よなかのすうがく](http://muk99.hateblo.jp/)


