# Linux, Mac環境でBinary出力を確認するサンプルプログラム

## データの出力形式をBinary出力に変更する

[製品マニュアル](https://github.com/rt-net/RT-USB-9AXIS-00/tree/master/manual)
の「データ出力形式の変更方法」を参考に、Binary出力へ変更して下さい。

ASCII出力の場合はサンプルプログラムが正常に動作しません。

## ポートの権限を設定する

サンプルプログラムを動かす前に、デバイスファイルの権限を設定してください。

9軸センサモジュールをPCに接続したあと、下記のコマンドを実行してください。

```sh
# 例
# /dev/ttyACM0 は環境によって名前が変わります。
$ sudo chmod 666 /dev/ttyACM0
```

## サンプルプログラムのデバイスファイル名を変更する

usb9axis_test.cの70行目にてデバイスファイル名を指定してください。
```c
// ===省略===

int main(int argc, char **argv){
    char serial_port[128]="/dev/ttyACM0"; // ここにデバイスファイル名を入力する

// ===省略===
```
## サンプルプログラムを実行する

usb9axis_test.cをgccでコンパイルし、実行してください。

```sh
# 例
$ gcc usb9axis_test.c
$ ./a.out
```


