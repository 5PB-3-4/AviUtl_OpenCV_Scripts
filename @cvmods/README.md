# OpenCVスクリプト ver[1.0]

## ▼これは何？
てつ(XIAO) さんが作成したAviutl用OpenCVアニメーションスクリプトを再現してみたスクリプトです。元のopencvスクリプトで使われているOpenCV本体のバージョンを新しいものにして機能追加を行いました。

再現元であるスクリプト製作者のてつ(XIAO) さん、ゆうきさんに感謝申し上げます。

<br>

## ▼導入方法
aviutlのscriptフォルダに@cvmodsフォルダを入れてください。

<br>

## ▼機能のご紹介
<details><summary>現在実装されている機能は以下の通りです。</summary>

- anmファイル
    - メディアンブラー
    - ボックスブラー
    - ガウシアンブラー
    - バイラテラルフィルター
    - リサイズ
    - リサンプリング
    - エッジ抽出(sobel)
    - エッジ抽出(laplacian)
    - エッジ抽出(canny)
    - モルフォロジー(GRAY)
    - モルフォロジー(RGB)
    - 二値化(ノーマル)
    - 二値化(適応的)
    - 拡散フィルター
    - ノイズフィルター
    - 画像ブラー
    - ドロステ効果
    - メビウス変換
    - メビウス変換2
    - meanshiftフィルター
    - ボロノイ図変換
    - ドロネー図変換
    - ペンシルスケッチ
    - stylize
    - 鮮明化フィルター
    - エッジ保存フィルター
    - 油絵風
    - ヒストグラム平坦化
    - カラーマップ
    - チャンネル入替
- objファイル
    - CV_画像読込
    - CV_動画読込

</details>

<br>

## ▼注意点
- 元のopencvスクリプトのソースコードが入手できていないため、一部スクリプトに関して元々の実装と異なる結果となる場合があります。
- 元のopencvスクリプトの内で一部効果（**膨張収縮**など）は、本スクリプトでは、機能の統合を行っています。統合を行ったスクリプトは以下の通りです。
    - 膨張収縮, モルフォロジー -> モルフォロジー(GRAY)
    - 膨張収縮RGB -> モルフォロジー(RGB)
    - 減色クラスタリング -> 削除（[こちら](https://github.com/5PB-3-4/beRL_scripts)へ移行しました）
    - ディフュージョン -> **拡散フィルター**へ名称変更
    - フィギュアブラー -> **画像ブラー**へ名称変更
- CV_画像読込ではSVG画像を読み込むことが可能です。

<br>

## ▼バグ
- ver1.0 ('24/07/14)

<br>

## ▼参照したもの
- [てつ(XIAO)さんのOpenCVアニメーションスクリプト](https://www.youtube.com/watch?v=dnLQuBWPwr8)
- [ボロノイ・ドロネーモザイク](http://aa-deb.hatenablog.com/entry/2015/12/13/opencv%25e3%2581%25a7%25e3%2583%2589%25e3%2583%25ad%25e3%2583%258d%25e3%2583%25bc%25e5%2588%2586%25e5%2589%25b2%25e3%2583%25bb%25e3%2583%259c%25e3%2583%25ad%25e3%2583%258e%25e3%2582%25a4%25e5%2588%2586)
- [拡散フィルター](https://ameblo.jp/delphinus1024/entry-12125312766.html)
- [メビウス変換、ドロステ効果](http://nive.jp/NiVE2/index.php?%A5%D7%A5%E9%A5%B0%A5%A4%A5%F3)
- svg参照
    - https://github.com/sammycage/lunasvg
    - https://www.exceedsystem.net/2021/06/09/how-to-resolve-build-errors-when-including-svgpp-in-visual-cpp/
    - https://github.com/svgpp/svgpp?tab=readme-ov-file

<br>

---
## ▼バグ報告等の連絡先はこちら
- Twitter : [@blue_beRL](https://twitter.com/blue_beRL)
- misskey : [@blue_beRL](https://misskey.io/@blue_beRL)
