#include <Arduino.h>
#include <Wire.h>

void sendPosition(int16_t x,int16_t y, int16_t avant);
void setPostion(int16_t x,int16_t y, int16_t theta);
void sendAngle(int16_t theta);
void ledOn(void);
void ledOff(void);
void getLinearError(void);
void getAngularError(void);
void getPosition(void);

void setup()
{
  Wire.begin(42);
  Serial.begin(9600);


  uint16_t x = 21;                  // 0x0015     
  uint16_t y = -400;                // 0xFE70
  uint16_t theta = 23;               // 0x0017
  setPostion(x,y,theta);

  sendPosition(400,-400,0);
  delay(5000);
  sendAngle(90);
  delay(5000);
  sendPosition(0,0,0);
  delay(5000);
  sendAngle(0);
  delay(5000);


  sendPosition(400,-400,1);
  delay(5000);
  sendAngle(90);
  delay(5000);
  sendPosition(0,0,1);
  delay(5000);
  sendAngle(0);
}

byte x = 'a';

void loop()
{
  ledOn();
  delay(100);
  ledOff();
  delay(100);

  getAngularError();
  getLinearError();
  getPosition();

  Serial.println();
}

//**************************************
// Set postion at the begining if the robot are not in the center
//**************************************
void setPostion(int16_t x,int16_t y, int16_t theta){
  x = 21;                  // 0x0015     
  y = -400;                // 0xFE70
  theta = 23;               // 0x0017
  Wire.beginTransmission(42);       // transmit to device #42
  Wire.write(21);                   // send command 21
  Wire.write((uint8_t)x);           // send 00
  Wire.write((uint8_t)(x>>8));      // send 15
  Wire.write((uint8_t)y);           // send FE
  Wire.write((uint8_t)(y>>8));      // send 70
  Wire.write((uint8_t)theta);        // send 00
  Wire.write((uint8_t)(theta>>8));   // send 17
  Wire.endTransmission();           // stop transmitting
}


//**************************************
// Set Linear control
//**************************************
void sendPosition(int16_t x,int16_t y, int16_t backwar){
  //(example x = 0x4523)
  //(example y = 0x4657)
  //(example avant = 0x0001) 1 to go backwar and 0 to go forwar
  Wire.beginTransmission(42);     // transmit to device #42
  Wire.write(30);                 // Write commande 30 to set the angle
  Wire.write((uint8_t)x);         // send 0x23
  Wire.write((uint8_t)(x>>8));    // send 0x45
  Wire.write((uint8_t)y);         // send 0x57
  Wire.write((uint8_t)(y>>8));    // send 0x46
  Wire.write((uint8_t)backwar);   // send 0x01
  Wire.write((uint8_t)(backwar>>8));// send 0x00
  Wire.endTransmission();         // stop transmitting
}

//**************************************
// Set angular control
//**************************************
void sendAngle(int16_t theta){
  Wire.beginTransmission(42); // transmit to device #42
  Wire.write(31);
  Wire.write((uint8_t)theta);
  Wire.write((uint8_t)(theta>>8));
  Wire.endTransmission();         // stop transmitting
}

//**************************************
// Led 1 On
//**************************************
void ledOn(void){
  Wire.beginTransmission(42); // transmit to device #42
  Wire.write(10);             // sends command 10
  Wire.endTransmission();     // stop transmitting
}

//**************************************
// Led 1 Off
//**************************************
void ledOff(void){
  Wire.beginTransmission(42); // transmit to device #42
  Wire.write(11);             // sends command 11
  Wire.endTransmission();     // stop transmitting
}

//**************************************
// Get Linear error
//**************************************
void getLinearError(void){
  Wire.beginTransmission(42);     // transmit to device #42
  Wire.write(34);                 // sends command 34 (get linear position)
  Wire.endTransmission();         // stop transmitting
  Wire.requestFrom(42, 2);        // request 6 bytes from peripheral device #42
  while (Wire.available()) {      // while byte are available (Normaly the while are process 1 time to get the ditance in mm)
    uint8_t c1 = Wire.read();     // receive the first byte (example : 0x01)
    uint8_t c2 = Wire.read();     // receive the second byte (example : 0x45)
    int final =  c1 | (c2<<8);    // assemble the two bytes (example : 0x0145)
    Serial.print(final,DEC);
    Serial.print("  ");
  }
}

//**************************************
// Get Angular error
//**************************************
void getAngularError(void){
  Wire.beginTransmission(42); // transmit to device #42
  Wire.write(33);             // sends command 33 (get angular position)
  Wire.endTransmission();     // stop transmitting
  Wire.requestFrom(42, 2);    // request 2 bytes from peripheral device #42
  while (Wire.available()) {  // while byte are available (Normaly the while are process 1 time to get the angle in degres)
    uint8_t c1 = Wire.read(); // receive the first byte (example : 0x01)
    uint8_t c2 = Wire.read(); // receive the second byte (example : 0x45)
    int final =  c1 | (c2<<8); // assemble the two bytes (example : 0x0145)
    Serial.print(final,DEC);
    Serial.print("  ");
  }
}

void getPosition(void){
  //**************************************
  // GET POSITION
  //**************************************
  Wire.beginTransmission(42);     // transmit to device #42
  Wire.write(20);                 // sends command 33 (get angular position)
  Wire.endTransmission();         // stop transmitting
  Wire.requestFrom(42, 6);        // request 2 bytes from peripheral device #42
  while (Wire.available()) {      // while byte are available (Normaly the while are process 3 time to get x y and theta)
    uint8_t c1 = Wire.read();     // receive the first byte (example : 0x01)
    uint8_t c2 = Wire.read();     // receive the second byte (example : 0x45)
    int final =  c1 | (c2<<8);    // assemble the two bytes (example : 0x0145)
    Serial.print(final,DEC);
    Serial.print("  ");
  }
}