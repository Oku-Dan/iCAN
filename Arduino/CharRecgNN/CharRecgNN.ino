#include "MPU6050.h"
#include "NewralNetwork.h"

//#define LEARNING
//#define HID
#ifdef HID
#include "HID.h"
#endif

#define PIN PB8
#define RATE 20

MPU6050 mpu;
SerialPort sp;
float buffer[RATE * 3 * 3];
float buffer20[20 * 3];
int learning_number = 0;
unsigned long loop_start = 0;

void printVec(){
  for(int i = 0;i < 20;i++){
    Serial.print(buffer20[3 * i + 0]);
    Serial.print(' ');
    Serial.print(buffer20[3 * i + 1]);
    Serial.print(' ');
    Serial.print(buffer20[3 * i + 2]);
    Serial.print(' ');
  }
  Serial.println(learning_number);
}

void process(){
  float* res = calcurate(buffer20);
#ifdef HID
#else
  for(int i = 0;i < 5;i++){
    Serial.print(i);
    Serial.print(' ');
    Serial.print(res[i]);
    Serial.print(' :');
    for(int j = 0;j < 20;j++){
      if(1.0 / 20 * j < res[i]){
        Serial.print('#');
      }else{
        Serial.print(' ');
      }
    }
    Serial.println('|');
  }
  Serial.println();
#endif
}

void startRead(){
  int end_counter = 0;
  int i;
  for(i = 0;i < RATE * 3;i++){
    loop_start = millis();
    mpu.ReadGyr(&buffer[3 * i + 0],&buffer[3 * i + 1],&buffer[3 * i + 2]);
    if(digitalRead(PIN) == HIGH){
      end_counter++;
      if(end_counter >= 5){
        break;
      }
    }
    delay(1000 / RATE - (millis() - loop_start));
  }
  for(int j = 0;j < 20;j++){
    int pos = (i / 20.0) * j;
    buffer20[3 * j + 0] = buffer[3 * pos + 0];
    buffer20[3 * j + 1] = buffer[3 * pos + 1];
    buffer20[3 * j + 2] = buffer[3 * pos + 2];
  }
#ifdef LEARNING
  printVec();
#else
  process();
#endif
}

void setup() {

  mpu.Initialize();
  pinMode(PIN,INPUT_PULLUP);
#ifdef HID
#else
  Serial.begin(115200);
#endif
}

void loop() {
#ifdef LEARNING
  if(Serial.available() > 0){
    char c = Serial.read();
    for(char i = 0;i < 10;i++){
      if(c == '0' + i){
        learning_number = i;
      }
    }
  }
#endif

  if(digitalRead(PIN) == LOW){
    startRead();
  }
}
