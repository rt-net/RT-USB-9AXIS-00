# RT-USB-9AXIS-00
本リポジトリはUSB出力9軸IMUセンサモジュールver2.0（以下、本製品）
のサンプルプログラムおよびマニュアルをまとめたものです.  

各種ファイルの内容は以下になります.  

## circuit 
本製品の回路図です.

## LPCXpresso Sample Program 
LPCXpressoとはLPCマイコン(lpc1343)の統合開発環境です.  
<http://www.nxp-lpc.com/lpc_boards/lpcxpresso/>
本ディレクトリに提供されているファームウェアの
プロジェクトファイルが格納されています.  
開発環境の構築方法およびサンプルプロジェクト
のインポート方法についてはmanualフォルダ内のマニュアルを
参照してください.  

以下のリンクからサンプルプログラムの説明ページに飛びます.
http://rt-net.github.io/RT-USB-9AXIS-00


## datasheet
9軸センサMPU9250およびlpc1343マイコンのデータシートです.

## firmware 
本製品のファームウェアです．

## manual
本製品のマニュアルです.

## driver
Windows環境用のUSBドライバです.  Windows環境でUSBを使用する際は
本ファイルを解凍し中のドライバをインストールして下さい. 
ドライバのインストール方法についてはmanualフォルダ内のマニュアルを
参照してください.
Linux環境, Mac環境においてはドライバーのインストールは必要ありません.
Windows環境用のドライバについては前バージョンと共通なので, 前バージョン
のセンサをご使用時に既にインストールしているならば, 新たにインストールする
必要はございません.  

## drawing
外形寸法図のpdfファイルです.  

## Processing Sample Program
Processingという言語でセンサデータを受信しデータの
値を表示するサンプルプログラムです.  

## 旧バージョン(ver1.0)との違いについて
旧バージョン(販売終了品)のサンプルプログラム等は次のリンクからダウンロード可能です.  
http://www.rt-shop.jp/download/RT-IMU9/

旧バージョンとver2.0では9軸センサがmpu9150からmpu9250に変更されています. 
そのため出力されるデータの形式が異なります.

ver2.0のファームウェアには次のような機能があります.

 a. 各種センサの原点手動キャリビュレーションモード
 b. センサの出力をbinary形式かASCII形式か切り替え
 
binary出力ではセンサデータはbyte列で送られてきます.  
そのため, byte列を意味のあるデータに変換する必要が
あります.(binary出力に関しては前バージョンのファームで
出力するbinary列と温度,地磁気以外は互換性があります.)
こちらのモードでは出力は57600bpsのUART出力とUSBから
になります.  

ASCII出力ではセンサデータは9軸センサのデータおよびタイムスタンプを
カンマ区切り形式で出力します.  こちらのモードではユーザーは
送られてくる文字列を直接読み取ることが可能です. 
