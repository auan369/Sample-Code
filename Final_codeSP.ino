
#include "CytronMotorDriver.h"
// Configure the motor driver.
CytronMD motor1(PWM_PWM, 5, 6);   // PWM 1A = Pin 3, PWM 1B = Pin 9.
CytronMD motor2(PWM_PWM, 10, 11); // PWM 2A = Pin 10, PWM 2B = Pin 11.
#define limitPin1 2
#define limitPin2 3
int limit1;
int limit2;
int turn_delay = 500; //300 is 45-turn, 400 is 80-turn, 450 is 90-turn
int left_power = 100;
int right_power = 98;




void setup(){
  Serial.begin(9600);
  pinMode(limitPin1, INPUT);
  pinMode(limitPin2, INPUT);
  //attachInterrupt(digitalPinToInterrupt(limitPin1), leftObstacle, RISING);
  //attachInterrupt(digitalPinToInterrupt(limitPin2), rightObstacle, RISING);
}

void loop(){
  
  //Serial.println(limit1);
  limit1 = digitalRead(limitPin1);
  limit2 = digitalRead(limitPin2);
  if (limit1){
    Serial.println("moving back");
    motor1.setSpeed(-left_power);   // Motor 1 runs forward
    motor2.setSpeed(-right_power);
    delay(500);
    Serial.println("Right");
    motor1.setSpeed(-left_power);   // Motor 1 runs forward
    motor2.setSpeed(right_power);
    delay(turn_delay);
  }
  else if(limit2){
    Serial.println("moving back");
    motor1.setSpeed(-left_power);   // Motor 1 runs forward
    motor2.setSpeed(-right_power);
    delay(500);
    Serial.println("left");
    motor1.setSpeed(left_power);   // Motor 1 runs forward
    motor2.setSpeed(-right_power);
    delay(turn_delay);
  }

  else{
    Serial.println("Forward");
    motor1.setSpeed(left_power);   // Motor 1 runs forward
    motor2.setSpeed(right_power);
    delay(200);
  }  
}
