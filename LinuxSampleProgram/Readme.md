# Linux, Mac環境でBinary出力を確認するサンプルプログラム

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


