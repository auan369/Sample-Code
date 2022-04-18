

#include <SoftwareSerial.h>
#define RxD 11  
#define TxD 10
// Pins for 74C922 keypad encoder
int KB_DataAvailable=A5;//Data ready pin from Keyboard
int KB_A=8; //Output A
int KB_B=9; //Output B
int KB_C=12; //Output C
int KB_D=13; //Output D

char recvChar;                                        //variable to store character received
String recvStr;
SoftwareSerial master(RxD, TxD);                      //Master Arduino acts as a serial communication device

// Declare keypad layout
char keys[] = {'1','2','3','F',
               '4','5','6','E',
               '7','8','9','D',
               'A','0','B','C'};

void setup(){ 
  Serial.begin(9600);                                 //start serial monitor, for communicating with the PC
  pinMode(RxD, INPUT);                                //set mode of receive pin (from slave arduino)
  pinMode(TxD, OUTPUT);                               //set mode of transmit pin (to slave arduino)
  master.begin(9600);                                 //start the master serial "port"

   //initialize pins for keypad
  pinMode(KB_DataAvailable,INPUT);
  pinMode(KB_A,INPUT);
  pinMode(KB_B,INPUT);
  pinMode(KB_C,INPUT);
  pinMode(KB_D,INPUT);
 
}
void loop(){
  //check if the keypad is hit
    if(digitalRead(KB_DataAvailable)) {
      char c=KB_Read(); //read the keypad      
      Serial.println(c);// print out the key
      master.write(c);
      delay(100);// debounce
    }

  if (master.available()){
    recvChar = master.read();                        //store character received
    Serial.print(recvChar);   
  }
}

  char KB_Read() {
  int ka,kb,kc,kd,k;
  ka=digitalRead(KB_A); //read encoder output A
  kb=digitalRead(KB_B); //read encoder output B
  kc=digitalRead(KB_C); //read encoder output C
  kd=digitalRead(KB_D); //read encoder output D
  k=ka+kb*2+kc*4+kd*8; // combine the encoder outputs 
  return keys[k];
}
