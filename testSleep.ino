#include "avr/sleep.h"
//#define sensorAPin A0
#define sensorDPin 2
//int sensorA;
int sensorD;
#define buttonPin 3
int button;
int timeNow;
int timeJn;

#define LEDPin 7
int activated = 0;

#include <Servo.h>
Servo leftServo;
Servo rightServo;
#define leftServoPin 5
#define rightServoPin 6
int angle = 155;

void action(){
  timeNow = millis();
  sensorD = digitalRead(sensorDPin);
  button = digitalRead(buttonPin);
  
  Serial.println(sensorD);
  Serial.print(activated);
  Serial.println("state");
  Serial.println(timeNow);
  Serial.println(timeJn);
  if ((timeNow - timeJn)>1000){
    timeJn = timeNow;

    if ((activated == 0) & (sensorD == 0)){
      for (int pos = angle; pos >= 35; pos --) { // goes from 180 degrees to 0 degrees
        leftServo.write(155 - (pos-35));              //155 - (pos-35) tell servo to go to position in variable 'pos'
        rightServo.write(pos);
        delay(100);                       // waits 15ms for the servo to reach the position
        Serial.println("CCW");
        angle = leftServo.read();
      }
      activated = 1;
    } 

    else if((activated == 0) & (button == 1)){
      for (int pos = angle; pos >= 35; pos --) { // goes from 180 degrees to 0 degrees
        leftServo.write(155 - (pos-35)); //155 - (pos-35)             // tell servo to go to position in variable 'pos'
        rightServo.write(pos);
        delay(1000);                       // waits 15ms for the servo to reach the position
        Serial.println("CCW");
        angle = leftServo.read();
      }
      activated = 1;
    }
  
    else if((activated == 1) & (button == 1)){
      for (int pos = angle; pos <= 155; pos ++) { // goes from 180 degrees to 0 degrees
        leftServo.write(35 + (155-pos));//35 + (155-pos)              // tell servo to go to position in variable 'pos'
        rightServo.write(pos);
        delay(100);                       // waits 15ms for the servo to reach the position
        Serial.println("CW");
        angle = leftServo.read();
      }
      activated = 0;
      
    }
  
  
  }
  
  
}


void setup(){
//  pinMode(sensorAPin, INPUT);
  pinMode(LEDPin, OUTPUT);

  pinMode(sensorDPin, INPUT);
  pinMode(buttonPin, INPUT);
  leftServo.attach(leftServoPin);
  rightServo.attach(rightServoPin);
  Serial.begin(9600);
  timeJn = millis();

  
  
  
  leftServo.write(35 + (155-angle));
  rightServo.write(angle);
  delay(500);
}

void loop(){
  if(activated){digitalWrite(LEDPin, HIGH);}
  else{digitalWrite(LEDPin, LOW);}
  sleep_enable();
  attachInterrupt(digitalPinToInterrupt(sensorDPin), action, FALLING);
  
  attachInterrupt(digitalPinToInterrupt(buttonPin), action, RISING);
  delay(1000);
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  Serial.println("Sleeping");
  //delay(2000);
  sleep_cpu();
  Serial.println("Woke up");
  
}
