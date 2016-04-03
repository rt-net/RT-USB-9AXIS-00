import processing.serial.*;
import processing.opengl.*;

Serial port; 
int[] buf     = new int[100];
int[] inByte  = new int[100];

String byte_info[] = {
  "ヘッダー", //0  byte
  " ", 
  " ", 
  " ", 
  "製品固有識別子L", //4  byte
  "製品固有識別子H", //5  byte
  "製品バージョン", //6  byte  
  "タイムスタンプ", //7  byte
  "ACC X", //8  byte
  " ", 
  "ACC Y", //10 byte
  " ", 
  "ACC Z", //12 byte
  " ", 
  "TEMP", //14 byte
  " ", 
  "GYRO X", //16 byte
  " ", 
  "GYRO Y", //18 byte
  " ", 
  "GYRO Z", //20 byte
  " ", 
  "MAG X", //22 byte
  " ", 
  "MAG Y", //24 byte
  " ", 
  "MAG Z", //26 byte
  " "
};
float[] omega_vec         = {
  0.0, 0.0, 0.0
};           //角速度ベクトル (x,y,z)[rad]
float[] acc_vec           = {
  0.0, 0.0, 0.0
};           //加速度ベクトル (x,y,z) [g]
float[] mag_vec           = {
  0.0, 0.0, 0.0
};           //地磁気ベクトル (x,y,z) [uT]

float   acc_norm          = 0.0;                       //加速度ベクトルのノルム 
float   mag_norm          = 0.0;                       //地磁気ベクトルのノルム
float   mag_cor_norm      = 0.0;                       //補正した地磁気ベクトルのノルム
float   temperature       = 0.0;                       //センサ温度[度C]


//起動時に一回だけ呼ばれる
void setup() 
{
  size(650, 600, P3D);
  frameRate(60);
  println(Serial.list());
  port = new Serial(this, "COM20", 115200);  // select port
}

//60fpsで描画するので1秒に60回呼ばれる
void draw()
{
  background(0);  
  writeSenValue();
  draw3DGraphs();
}

//COMポートで受信したときに呼ばれる関数
void serialEvent(Serial p)
{

  if (port.available() != 0)
  {
    for (int i=27; i>=1; i--)
    {
      buf[i] = buf[i-1];
    }
    buf[0] = port.read();
   
}

  //受信データの先頭4byteが0xff,0xff,0x52,0x54なのでこのパターンを目印に
  //データをinByteに格納し物理量に変換
  // 0xff,0xff,0x52,0x54のパターンが現れるのは10msec毎
  if (
  buf[24] == 0x54  
    && buf[25] == 0x52 
    && buf[26] == 0xff 
    && buf[27] == 0xff )
  {
  
    for (int i = 0; i < 28; i ++)
    {
      inByte[i] = buf[27-i];
    }
    port.clear();

    //コンソールに受信データを表示
    println("=================="); 
    println("Byte", "16進", "10進", "結合データ(符号なし)", "結合データ(符号付)", "リファレンス");

    for (int i = 0; i<28; i++)
    {
      if (i<=9)
      {
        if (i %2 == 0 && i> 7 ) println(i, "   ", hex(inByte[i], 2), inByte[i], inByte[i] + (inByte[i+1]<<8), byte_info[i]);
        else println(i, "   ", hex(inByte[i], 2), inByte[i], byte_info[i]);
      } else
      {
        if (i %2 == 0 && i> 7 ) println(i, "  ", hex(inByte[i], 2), inByte[i], inByte[i] + (inByte[i+1]<<8), byte_info[i] );
        else println(i, "  ", hex(inByte[i], 2), inByte[i], byte_info[i] );
      }
    }

    //センサ値を角加速度ベクトルに変換
    omega_vec[0] = radians(((float)(concatenate2Byte_int(inByte[17], inByte[16]) ) )/16.4);
    omega_vec[1] = radians(((float)(concatenate2Byte_int(inByte[19], inByte[18]) ) )/16.4);
    omega_vec[2] = radians(((float)(concatenate2Byte_int(inByte[21], inByte[20]) ) )/16.4);
    //センサ値を加速度ベクトルに変換
    acc_vec[0]   = (float)(concatenate2Byte_int(inByte[9], inByte[8]))/2048.0; 
    acc_vec[1]   = (float)(concatenate2Byte_int(inByte[11], inByte[10]))/2048.0; 
    acc_vec[2]   = (float)(concatenate2Byte_int(inByte[13], inByte[12]))/2048.0;  
    acc_norm = sqrt(acc_vec[0]*acc_vec[0]+acc_vec[1]*acc_vec[1]+acc_vec[2]*acc_vec[2]);
    //センサ値を地磁気ベクトルに変換
    mag_vec[0]   = (float)(concatenate2Byte_int(inByte[23], inByte[22])) * 0.15;  
    mag_vec[1]   = (float)(concatenate2Byte_int(inByte[25], inByte[24])) * 0.15; 
    mag_vec[2]   = (float)(concatenate2Byte_int(inByte[27], inByte[26])) * 0.15;
    mag_norm = sqrt(mag_vec[0]*mag_vec[0]+mag_vec[1]*mag_vec[1]+mag_vec[2]*mag_vec[2]);
    //センサ値を温度に変換
    temperature = (float)(concatenate2Byte_int(inByte[15], inByte[14]))/333.87 + 21.0;
 
  }
}

//画面上に各センサの出力値を描画
void writeSenValue()
{
  fill(255, 255, 255);
  textSize(20);
  text("accX[g]", 10, 20);
  text("accY[g]", 10, 40);
  text("accZ[g]", 10, 60);
  text("|a|=", 10, 80);
  text(acc_vec[0], 90, 20);
  text(acc_vec[1], 90, 40);
  text(acc_vec[2], 90, 60); 
  text(acc_norm, 60, 80); 

  text("magX[uT]", 180, 20);
  text("magY[uT]", 180, 40);
  text("magZ[uT]", 180, 60);
  text("|m|=", 180, 80);
  text(mag_vec[0], 300, 20);
  text(mag_vec[1], 300, 40);
  text(mag_vec[2], 300, 60);  
  text(mag_norm, 230, 80);  

  text("omegaX[deg/s]", 400, 20);
  text("omegaY[deg/s]", 400, 40);
  text("omegaZ[deg/s]", 400, 60);
  text(degrees(omega_vec[0]), 550, 20);
  text(degrees(omega_vec[1]), 550, 40);
  text(degrees(omega_vec[2]), 550, 60); 

  text(temperature, 400, 80);
  text("[degree C]", 500, 80);

}

int concatenate2Byte_int(int H_byte, int L_byte) {
  int con; 
  con = L_byte + (H_byte<<8);
  if (con > 32767) {
    con -=  65536;
  }
  return con;
}


int concatenate2Byte_uint(int H_byte, int L_byte) {
  int con; 
  con = L_byte + (H_byte<<8);
  return con;
}


//地磁気と加速度センサのベクトルを描画
void draw3DGraphs()
{
  stroke(0, 0, 0);
  pushMatrix();
  translate(width/2, height/2);    
  rotateY(radians(-20));
  rotateX(radians(20));
  fill(255, 0, 0);
  box(200, 5, 5);   
  fill(0, 255, 0);
  box(5, 200, 5);
  fill(0, 0, 255);
  box(5, 5, 200);
  fill(100, 100, 100, 80);  

  for (int i=1; i<=40; i++) {
    pushMatrix();
    translate(acc_vec[0] *50 *i/20, -acc_vec[2] *50*i/20, -acc_vec[1] *50*i/20 );
    fill(255, 55, 0);
    box(5, 5, 5);
    popMatrix();
  }

  for (int i=1; i<=40; i++) {
    pushMatrix();
    translate(mag_vec[0]  *i/20, -mag_vec[2] *i/20, -mag_vec[1] *i/20 );
    fill(200, 50, 200);
    box(5, 5, 5);
    popMatrix();
  }
  popMatrix();
}




