#define extrude_step 1800
#define swab_time 8000//milliseconds
#define eject_step 200

#define redLEDPin 40
#define greenLEDPin 41

#include "pitches.h"
#define buzzerPin 28

void alarm(){
  for (int i=0; i<= 5; i++){
    tone(buzzerPin, NOTE_G4, 200);
    delay(300);
  }
  noTone(buzzerPin);
}

char recvChar;
String recvStr;

void master_in(){
  if (Serial1.available()){
    recvChar = Serial1.read();                        //store character received
    recvStr = String(recvChar);
    Serial.println(recvStr);
  }  
}

#include <Stepper.h>
#define A1 3
#define B1 4
#define C1 5
#define D1 6
#define motorSteps 2048
Stepper myStepper1 = Stepper(motorSteps, A1,C1,B1,D1);




#define A 8
#define B 9
#define C 10
#define D 11
#define motorSteps2 2048
Stepper myStepper2 = Stepper(motorSteps2, A,C,B,D);




#define limitPin 2
bool limit_pressed = false;

//initialize dc encoder motor


int pin1 = 13;   // motor pin1
int pin2 = 12;  // motor pin2



void MotorClockwise(int power){
  if(power >60){  // set the min value as 60 because of the motor inertia
  analogWrite(pin1, power);
  digitalWrite(pin2, LOW);
  }else{
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
  }
}

void MotorCounterClockwise(int power){
  if(power > 60){
  analogWrite(pin2, power);
  digitalWrite(pin1, LOW);
  }else{
    digitalWrite(pin2, LOW);
    digitalWrite(pin1, LOW);
  }
}


void swab(){
  digitalWrite(redLEDPin, HIGH);
  digitalWrite(greenLEDPin, LOW);
  Serial.println("Dispensing");
  myStepper2.step(2048);
  delay(100);
  
  Serial.println("Extruding");
  for (int i =0;i<=extrude_step;i++){myStepper1.step(-1);limitCheck();if(limit_pressed == false){break;}}
  if(limit_pressed == false){alarm();delay(1000);return;}
  delay(100);
  
  Serial.println("Swabbing");
  MotorClockwise(150);
  for (int t=0;t<=swab_time;t+=100){delay(swab_time/100);limitCheck();if(limit_pressed == false){break;}}
  limitCheck();
  if(limit_pressed == false){MotorClockwise(0);alarm();delay(1000);return;}
  MotorClockwise(0);
  


  Serial.println("Waiting for user to move off");
  digitalWrite(redLEDPin, LOW);
  digitalWrite(greenLEDPin, HIGH);
  while(limit_pressed){delay(10);limitCheck();}
  delay(5000);

  Serial.println("Eject");
  for (int i =0;i<=eject_step;i++){myStepper1.step(-1);}
  delay(100);

  Serial.println("Retract");
  for (int i =0;i<=extrude_step+eject_step;i++){myStepper1.step(1);}
  delay(100);
  recvStr = "Z";
}

void limitCheck(){
  int limit = digitalRead(limitPin);
  if (limit == HIGH){
    Serial.println("Closed");
    limit_pressed = true;
    Serial.write("C");
  }
  else{
    Serial.println("Open");
    limit_pressed = false;
  }
  
}

void cwDispense(){myStepper2.step(5);}
void acwDispense(){myStepper2.step(-5);}

void posExtrude(){myStepper1.step(10);}
void negExtrude(){myStepper1.step(-10);}

//real code
void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  //pinMode(RxD, INPUT);
  //pinMode(TxD, OUTPUT);
  //slave.begin(9600);

  pinMode(redLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);
  
  pinMode(limitPin, INPUT);
  
  myStepper1.setSpeed(5);

  pinMode(A,OUTPUT);
  pinMode(B,OUTPUT);
  pinMode(C,OUTPUT);
  pinMode(D,OUTPUT);
  myStepper2.setSpeed(5);
  //stepper 2 pins

  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(limitPin), limitCheck, CHANGE); // Enable the external interrupt
}

void loop() {
  
  digitalWrite(redLEDPin, LOW);
  digitalWrite(greenLEDPin, HIGH);
  if(recvStr == "A" & limit_pressed){swab();recvStr = "Z";}
  else if(recvStr == "1"){acwDispense();recvStr = "Z";}
  else if(recvStr == "2"){cwDispense();recvStr = "Z";}
  else if(recvStr == "4"){negExtrude();recvStr = "Z";}
  else if(recvStr == "5"){posExtrude();recvStr = "Z";}
  else{recvStr = "Z";}
  
  master_in();


}
