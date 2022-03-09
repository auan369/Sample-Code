/*
  AWS IoT WiFi

  This sketch securely connects to an AWS IoT using MQTT over WiFi.
  It uses a private key stored in the ATECC508A and a public
  certificate for SSL/TLS authetication.

  It publishes a message every 5 seconds to arduino/outgoing
  topic and subscribes to messages on the arduino/incoming
  topic.

  The circuit:
  - Arduino MKR WiFi 1010 or MKR1000

  The following tutorial on Arduino Project Hub can be used
  to setup your AWS account and the MKR board:

  https://create.arduino.cc/projecthub/132016/securely-connecting-an-arduino-mkr-wifi-1010-to-aws-iot-core-a9f365

  This example code is in the public domain.
*/

//Variables and libaries for IoT
#include <ArduinoBearSSL.h>
#include <ArduinoECCX08.h>
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>


#include "arduino_secrets.h"
/////// Enter your sensitive data in arduino_secrets.h
const char ssid[]        = SECRET_SSID;           //WIFI SSID
const char pass[]        = SECRET_PASS;           //WIFI PASSWORD
const char broker[]      = SECRET_BROKER;        //AWS broker/endpoint
const char* certificate  = SECRET_CERTIFICATE;   //ApertuaThing# Certificate

WiFiClient    wifiClient;            // Used for the TCP socket connection
BearSSLClient sslClient(wifiClient); // Used for SSL/TLS connection, integrates with ECC508
MqttClient    mqttClient(sslClient);

unsigned long lastMillis = 0;
char recvChar;
String recvStr;
int recvStr_len;
int angle;
const String iotTopic = "portal2"; //MQTT Topic




//Variables and libaries for controlling apertua
#include <Servo.h>

//Pinout variable initialisation
int AutoLS = 2;       //auto mode limit switch
int CWLS = 3;         //CW limit switch
int CCWLS = 4;        //CCW limit switch; pin same
int MotorPin = 12;    //servo motor pin; pin same
Servo myservo;
int LEDPin = 13;    //LED to indicate when connected to AWS


// code variable initialisation
bool Automode = LOW;    //whether auto mode or not
int MotorPos = 0;       //Motor position, from 0 to 180
bool TurnCW = LOW;      //whether CW rotation
bool TurnCCW = LOW;     //whether CCW rotation  
int CloseInt = 0;      //angle/position for closed Apertua
int OpenInt = 130;      //angle/position for open Apertua








unsigned long getTime() {
  // get the current time from the WiFi module  
  return WiFi.getTime();
}

void connectWiFi() {
  Serial.print("Attempting to connect to SSID: ");
  Serial.print(ssid);
  Serial.print(" ");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }
  Serial.println();

  Serial.println("You're connected to the network");
  Serial.println();
}

void connectMQTT() {
  
  Serial.print("Attempting to MQTT broker: ");
  Serial.print(broker);
  Serial.println(" ");

  while (!mqttClient.connect(broker, 8883)) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }
  Serial.println();

  Serial.println("You're connected to the MQTT broker");
  Serial.println();
  digitalWrite(LEDPin, HIGH);

  // subscribe to a topic
  mqttClient.subscribe(iotTopic);
}

void publishMessage() {
  Serial.println("Publishing message");

  // send message, the Print interface can be used to set the message contents
  mqttClient.beginMessage(iotTopic);
  mqttClient.print("hello ");
  mqttClient.print(millis());
  mqttClient.endMessage();
}

void onMessageReceived(int messageSize) {
  // we received a message, print out the topic and contents
  Serial.print("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");
  recvStr = "";
  int percent = 0;
  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    //Serial.print((char)mqttClient.read());
    recvChar = (char)mqttClient.read();
    recvStr = String(recvStr + (String)recvChar);
  }
  recvStr_len = recvStr.length();
  for (int i = 0; i< recvStr_len; i++){percent += (((int)recvStr[i]-48)*pow(10,recvStr_len-1-i));} //converts to integer angle
  angle = percent*(OpenInt-CloseInt)/100 + CloseInt;
  Serial.println(angle);
}









void setup() {
  Serial.begin(115200);
  //while (!Serial);
  pinMode (LEDPin, OUTPUT);
  digitalWrite(LEDPin, LOW);
  pinMode (AutoLS, INPUT);
  pinMode (CWLS, INPUT);
  pinMode (CCWLS, INPUT);
  myservo.attach(MotorPin);
  digitalWrite(MotorPin, LOW);
  //Pin setup for apertua control



  
  //IoT section
  if (!ECCX08.begin()) {
    Serial.println("No ECCX08 present!");
    while (1);
  }

  // Set a callback to get the current time
  // used to validate the servers certificate
  ArduinoBearSSL.onGetTime(getTime);

  // Set the ECCX08 slot to use for the private key
  // and the accompanying public certificate for it
  sslClient.setEccSlot(0, certificate);

  // Optional, set the client id used for MQTT,
  // each device that is connected to the broker
  // must have a unique client id. The MQTTClient will generate
  // a client id for you based on the millis() value if not set
  //
  // mqttClient.setId("clientId");

  // Set the message callback, this function is
  // called when the MQTTClient receives a message
  mqttClient.onMessage(onMessageReceived);

  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!mqttClient.connected()) {
    // MQTT client is disconnected, connect
    digitalWrite(LEDPin, LOW);
    connectMQTT();
  }
  else{digitalWrite(LEDPin, HIGH);}

  // poll for new MQTT messages and send keep alives
  mqttClient.poll();
  
}

void loop() {
  

  Automode = digitalRead(AutoLS);
  if (Automode == LOW)
  {
    Serial.print("Manual mode\n");
    digitalWrite (MotorPin, LOW); //signal to servo motor set to LOW, allows for manual mode
  }
  else
  { 
    Serial.println("Auto Mode");
    MotorPos = myservo.read();
    TurnCW = digitalRead(CWLS);
    TurnCCW = digitalRead(CCWLS);
    if (TurnCW == HIGH && TurnCCW == LOW)
    {
      Serial.print("CW\n");
      for (int pos = MotorPos; pos >= CloseInt; pos--) //rotate all the way till close
      {
        myservo.write(pos);
        angle = pos;
        delay(15);
        TurnCW = digitalRead(CWLS);
        if (TurnCW == LOW) {break;} //check if knob reset
      }
    }

    if (TurnCCW == HIGH && TurnCW == LOW)
    {
      Serial.print("CCW\n");
      for (int pos = MotorPos; pos <= OpenInt; pos++) //rotate all the way till open; fine tune pos value needed?
      {
        myservo.write(pos);
        angle = pos;
        delay(15);
        TurnCCW = digitalRead(CCWLS);
        if (TurnCCW == LOW){break;} //check if knob reset
      }
    }
    
    if (TurnCCW == LOW && TurnCW == LOW) //Iot mode
    {
      Serial.print("Iot mode\n");

      if (WiFi.status() != WL_CONNECTED) {connectWiFi();}
      if (!mqttClient.connected()) {digitalWrite(LEDPin, LOW);connectMQTT();} // MQTT client is disconnected, connect
      else{digitalWrite(LEDPin, HIGH);}
      // poll for new MQTT messages and send keep alives
      mqttClient.poll();

      
      
      MotorPos = myservo.read();
      if(MotorPos>angle)
      {
        Serial.print("IoT CW\n");
        for (int pos = MotorPos; pos >= angle; pos--) 
        {
          myservo.write(pos);
          delay(15);
        }
      }
      
      else if(MotorPos<angle)
      {
        Serial.print("IoT CCW\n");
        for (int pos = MotorPos; pos <= angle; pos++) 
        {
          myservo.write(pos);
          delay(15);
        }
      }

    }  
  }

  
}
