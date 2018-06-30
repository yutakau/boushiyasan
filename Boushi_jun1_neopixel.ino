// Boushiyasan  
//
//   ESP32, ADXL343
//   Y.usui 2018/5/24
//

#include <Wire.h>
#include <Adafruit_NeoPixel.h>

byte DEVICE_ADDRESS = 0x53;

#define LEDPIN 16

// ジェスチャー判定基準
#define THRES_YES_Y_MIN 20
#define THRES_YES_Y_MAX 35
#define THRES_YES_X_MIN -10
#define THRES_YES_X_MAX 10

#define THRES_WONDER_X_MIN 20
#define THRES_WONDER_X_MAX 50


Adafruit_NeoPixel strip = Adafruit_NeoPixel(6,LEDPIN, NEO_RGB+NEO_KHZ800);

// buffer
int16_t  tmpx[8],tmpy[8];

int cnt;
int pcnt;
  
void setup() {
  int SDA = 21;
  int SCL = 22;
  Wire.begin(SDA, SCL);

  Serial.begin(115200);
  Serial.print("start");

  // check ADXL343
  // Read Device ID reg(0x00)
  Wire.beginTransmission(DEVICE_ADDRESS);
  Wire.write(0x00);
  Wire.endTransmission();
  Wire.requestFrom(DEVICE_ADDRESS,1); 
  byte val = Wire.read();
  if (val == 0xE5) {
    Serial.print("ADXL343 response OK\n");  // Device IDが読めればOK
  }else{
    Serial.print("ADXL343 response ERR\n");
  }

  //加速度センサ 処理開始
  Wire.beginTransmission(DEVICE_ADDRESS);
  Wire.write(0x2D);  // POWER_CTL register
  Wire.write(0x08);
  Wire.endTransmission();

  //加速度センサ　データフォーマット設定
  Wire.beginTransmission(DEVICE_ADDRESS);
  Wire.write(0x31);  // FORMAT reg
  Wire.write(0x00);
  Wire.endTransmission();

  //加速度センサ　サンプリング周波数設定(データシートTable7参照)
  Wire.beginTransmission(DEVICE_ADDRESS);
  Wire.write(0x2c); 
  //Wire.write(0x08);   // 25Hz
  Wire.write(0x07);   // 12.5Hz
  Wire.endTransmission();
  
  //カラーLED初期化
  strip.begin();

  strip.setBrightness(255);
  cnt=0;
  
  strip.setPixelColor(0, 0, 0, 0);
  strip.setPixelColor(1, 0, 0 ,0);
  strip.setPixelColor(2, 0, 0, 0);
  strip.setPixelColor(3, 0, 0, 0);
  strip.setPixelColor(4, 0, 0, 0);
  strip.setPixelColor(5, 0, 0, 0);
  delay(300); 
  strip.show();  
  
  strip.setPixelColor(0, 128, 0,255);
  strip.setPixelColor(1, 0, 128,255);
  strip.setPixelColor(2, 0,   0,255);
  strip.setPixelColor(3, 0,   0,255);
  strip.setPixelColor(4, 0, 128,255);
  strip.setPixelColor(5, 128, 0,255);
  strip.show();  
  delay(500); 

  strip.setPixelColor(0, 0, 0, 0);
  strip.setPixelColor(1, 0, 0 ,0);
  strip.setPixelColor(2, 0, 0, 0);
  strip.setPixelColor(3, 0, 0, 0);
  strip.setPixelColor(4, 0, 0, 0);
  strip.setPixelColor(5, 0, 0, 0);
  pcnt=0;
  
  delay(1);
  strip.show();  
  
}
  
  
void loop() {
  uint8_t  val[6];
  int16_t  f;
  uint8_t  gesture_yes_flag, gesture_wonder_flag;
  int16_t  deltax, deltay;


  //
  Wire.beginTransmission(DEVICE_ADDRESS);
  Wire.write(0x32);
  Wire.endTransmission(false);              //(bugfix)  I2C Stop Conditionを送らない
  
  Wire.requestFrom(DEVICE_ADDRESS,6,false); //(bugfix)  I2C Stop Conditionを送らない
  for (int i=0; i< 6; i++){
    while (Wire.available() == 0 ){}
    val[i] = Wire.read();
  }
  Wire.endTransmission(true);
    
  int16_t x = (((uint16_t)val[1]) << 8) | val[0];  //(bugfix) Signed10bitのため,uint16にcastする
  int16_t y = (((uint16_t)val[3]) << 8) | val[2];
  int16_t z = (((uint16_t)val[5]) << 8) | val[4];  

  int16_t  xangle = (atan2( x/512.0, z/512.0) / 3.14159 * 180.0) ; //各軸の角加速度から角度変化を求める
  int16_t  yangle = (atan2( y/512.0, z/512.0) / 3.14159 * 180.0) ;
  
  delay(80);   // 80msウェイト= サンプリング周期　12.5Hzに合わせてある

  // 判定処理

  for(int i=7; i>0; i--){
    tmpx[i] = tmpx[i-1];
    tmpy[i] = tmpy[i-1];
  }
  tmpx[0] = xangle;
  tmpy[0] = yangle;
 
  deltax = xangle - tmpx[7];   //X方向の角度変化
  deltay = yangle - tmpy[7];   //Y方向の角度変化

 
  // YES(首を縦にふる) 検出  
  if (((deltax < THRES_YES_X_MAX) && (deltax > THRES_YES_X_MIN )) &&
      ((deltay < THRES_YES_Y_MAX) && (deltay > THRES_YES_Y_MIN )))  
      {
        gesture_yes_flag=1; 
      } else { 
        gesture_yes_flag=0;
  }

  // WONDER(首をかしげる）検出
  
 // if (((deltax < THRES_WONDER_X_MAX) && (deltax > THRES_WONDER_X_MIN )) &&
 //     ((deltay < THRES_WONDER_Y_MAX) && (deltay > THRES_WONDER_Y_MIN )))  
if ((deltax < THRES_WONDER_X_MAX) && (deltax > THRES_WONDER_X_MIN ))
      {
        gesture_wonder_flag=1; 
      } else { 
        gesture_wonder_flag=0;
  }
  /*
  Serial.print(  deltax );
  Serial.print("  ");
  Serial.print(  deltay );
*/

  /*if ((deltax < THRES_YES_X_MAX) && (deltax > THRES_YES_X_MIN )){
    Serial.print(" <YES x> ");
  }
  if ((deltay < THRES_YES_Y_MAX) && (deltay > THRES_YES_Y_MIN )){
    Serial.print(" <YES y> ");
  }
  if ((deltax < THRES_WONDER_X_MAX) && (deltax > THRES_WONDER_X_MIN )){
    Serial.print("  <W x> ");
  }*/

  Serial.print(pcnt);
  Serial.print("   ");

  Serial.print(cnt);
  Serial.print("   ");

  if (gesture_yes_flag) {
    Serial.print("  *YES* ");
  }
  if (gesture_wonder_flag) {
    Serial.print("  *W* ");
  }
  
  Serial.print("\n");
  
  //LED表示処理
  
  if (gesture_yes_flag && (cnt==0)) {
    //strip.setPixelColor(0, 0, 255,0); 
    //strip.setPixelColor(1, 0, 255,0); 
    //delay(1);   
    //strip.show();
    cnt=1;
    
    if (pcnt==3) pcnt=pcnt;
    else if (pcnt>=0) pcnt++;
    else if (pcnt<0) pcnt=1;
    //else pcnt=1;
    
  } 

  if (gesture_wonder_flag && (cnt==0)) {
    //strip.setPixelColor(0, 0, 0,64);   
    //strip.setPixelColor(1, 0, 0,64);   
    //delay(1);
    //strip.show();
    cnt=1;
    if (pcnt==-3) pcnt=pcnt;
    else if (pcnt<=0) pcnt--;
    else if (pcnt>0) pcnt=-1;
    //else pcnt=-1;
  } 

  if ((gesture_wonder_flag | gesture_yes_flag) && (cnt==1)){
  if (pcnt==0){
      strip.setPixelColor(0, 0, 0,0);
      strip.setPixelColor(1, 0, 0,0);
      strip.setPixelColor(2, 0, 0,0);
      strip.setPixelColor(3, 0, 0,0);
      strip.setPixelColor(4, 0, 0,0);
      strip.setPixelColor(5, 0, 0,0);
  } else if (pcnt==1) {
      strip.setPixelColor(0, 0, 0,255);
      strip.setPixelColor(1, 0, 0,0);
      strip.setPixelColor(2, 0, 0,0);
      strip.setPixelColor(3, 0, 0,0);
      strip.setPixelColor(4, 0, 0,0);
      strip.setPixelColor(5, 0, 0,255);
  } else if (pcnt==2) {
      strip.setPixelColor(0, 0, 0,255);
      strip.setPixelColor(1, 0, 0,255);
      strip.setPixelColor(2, 0, 0,0);
      strip.setPixelColor(3, 0, 0,0);
      strip.setPixelColor(4, 0, 0,255);
      strip.setPixelColor(5, 0, 0,255);
  } else if (pcnt==3) {
      strip.setPixelColor(0, 0, 0,255);
      strip.setPixelColor(1, 0, 0,255);
      strip.setPixelColor(2, 0, 0,255);
      strip.setPixelColor(3, 0, 0,255);
      strip.setPixelColor(4, 0, 0,255);
      strip.setPixelColor(5, 0, 0,255);
  } else if (pcnt==-1) {
      strip.setPixelColor(0, 255, 255,0);
      strip.setPixelColor(1, 0, 0,0);
      strip.setPixelColor(2, 0, 0,0);
      strip.setPixelColor(3, 0, 0,0);
      strip.setPixelColor(4, 0, 0,0);
      strip.setPixelColor(5, 255, 255,0);
  } else if (pcnt==-2) {
      strip.setPixelColor(0, 255, 255,0);
      strip.setPixelColor(1, 255, 255,0);
      strip.setPixelColor(2, 0, 0,0);
      strip.setPixelColor(3, 0, 0,0);
      strip.setPixelColor(4, 255, 255,0);
      strip.setPixelColor(5, 255, 255,0);
  } else if (pcnt==-3) {
      strip.setPixelColor(0, 255, 255,0);
      strip.setPixelColor(1, 255, 255,0);
      strip.setPixelColor(2, 255, 255,0);
      strip.setPixelColor(3, 255, 255,0);
      strip.setPixelColor(4, 255, 255,0);
      strip.setPixelColor(5, 255, 255,0);
    
  }
        strip.show();
      delay(1);
  }
  

  // LED消灯処理
  if (cnt!=0) {
    cnt++;
    if (cnt>=7) {
      cnt=0;
    }
  }
  /*
  if (cnt!=0) {
    cnt++;
    if (cnt>=5) {
      strip.setPixelColor(0, 0, 0,0);
      strip.setPixelColor(1, 0, 0,0);

      strip.show();
      delay(1);
      cnt=0;
    }
  }*/
  

}
