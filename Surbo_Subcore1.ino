/*
 *  Sub1.ino - MP Example to communicate message data
 *  Copyright 2019 Sony Semiconductor Solutions Corporation
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#if (SUBCORE != 1)
#error "Core selection is wrong!!"
#endif

#include <MP.h>
#include <Servo.h>

Servo servo;

void setup()
{
  Serial.begin(115200);
  while (!Serial);

  int ret = 0;

  ret = MP.begin();
  if (ret < 0) {
    errorLoop(2);
  }
  servo.attach(10, 500, 2400);
  pinMode(10, OUTPUT);

  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);

  digitalWrite(8, LOW);
  digitalWrite(9, LOW);

}

void loop()
{
  int      ret;
  int8_t   msgid;
  uint32_t msgdata;

  /* Echo back */

  ret = MP.Recv(&msgid, &msgdata);
  if (ret < 0) {
    errorLoop(3);
  }
  delay (200);
  Serial.print("Received value: ");
  Serial.println(msgdata);

  //ret = MP.Send(msgid, msgdata);
  //if (ret < 0) {
  //  errorLoop(4);
  //}
 
  Servo_drive(msgdata);
  //morter_drive(msgdata);

}

void errorLoop(int num)
{
  int i;

  while (1) {
    for (i = 0; i < num; i++) {
      ledOn(LED0);
      delay(300);
      ledOff(LED0);
      delay(300);
    }
    delay(1000);
  }
}

void Servo_drive(int value)
{
  int i;
  int delaytime = 800/value ;
  int count = 3*value +3;
  for (i = 0 ; i < count ; i++){
    servo.write(60);
    delay(200);
    servo.write(120);
    delay(delaytime);
  }
}

void morter_drive(int value)
{
  //int duty; //(0-100)
  int T;
  T = 1000;  // 10khz = 1/100us
  int ontime;
  int offtime;
  switch (value){
    case 1:
      ontime = 100;
      offtime = T - ontime;
      for (int i=0; i<5000; i++){
      digitalWrite(8, HIGH);
      delayMicroseconds(ontime);
      digitalWrite(9, LOW);
      delayMicroseconds(offtime);
      }
      break;
    case 2:
      ontime = 300;
      offtime = T - ontime;
      for (int i=0; i<5000; i++){
      digitalWrite(8, HIGH);
      delayMicroseconds(ontime);
      digitalWrite(9, LOW);
      delayMicroseconds(offtime);
      }
      break;
    case 3:
      ontime = 600;
      offtime = T - ontime;
      for (int i=0; i<5000; i++){
      digitalWrite(8, HIGH);
      delayMicroseconds(ontime);
      digitalWrite(9, LOW);
      delayMicroseconds(offtime);
      }
      break;
    case 4:
      ontime = 900;
      offtime = T - ontime;
      for (int i=0; i<5000; i++){
      digitalWrite(8, HIGH);
      delayMicroseconds(ontime);
      digitalWrite(9, LOW);
      delayMicroseconds(offtime);
      }
      break;
  }
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);

}
