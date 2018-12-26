/*
  Sweep

  by BARRAGAN <http://barraganstudio.com>
  This example code is in the public domain.

  modified 8 Nov 2013  by Scott Fitzgerald
  http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

// Thinkspeak channel information
const char * writeAPIKey = "HMWUP4OYMMHD3ZMT"; 
unsigned long channelID = 379602;

WiFiClient client;

//SSID of your network
char ssid[] = ""; //SSID of your Wi-Fi router 
char pass[] = ""; //Password of your Wi-Fi router 

//SERVO
unsigned int interval_waktu = 5000;
unsigned long waktu_terakhir_reset = 0;
int frekuensi = 0;
int waterfloat_pin = D2;
int waterfloat_state = 0;
int last_waterfloat_state = 0;

//HC-SR04
//int echoPin = 7; //pin for arduino uno
int echoPin = D3;
//int trigPin = 8; //pin for arduino uno
int trigPin = D4;
unsigned long duration = 0;

Servo servo_9;

void setup()
{
  pinMode(waterfloat_pin, INPUT);
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT);
  Serial.begin(115200);
//  servo_9.attach(9); //pin for arduino uno
  servo_9.attach(D1);
  delay(150);

  // Connect to Wi-Fi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to..");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  ThingSpeak.begin(client); //start the Thingspeak

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Wi-Fi connected successfully");
}
void loop()
{
  loopServo();
}

void loopServo(){
  frekuensi = 0;
  waktu_terakhir_reset = millis();
  while ((millis() - waktu_terakhir_reset) < interval_waktu) {
    waterfloat_state = digitalRead(waterfloat_pin);
    Serial.print(waterfloat_state);
    if (waterfloat_state != last_waterfloat_state) {
        frekuensi++;
    }
    last_waterfloat_state = waterfloat_state;
    delay(500);
  }
  // sweep the servo from 0 to 180 degrees in steps
  // of 1 degrees
  Serial.println();
  Serial.print("Frekuensi : ");
  Serial.println(frekuensi);
  if (frekuensi >= 5) {
    servo_9.write(90);
    delay(3000); // Delay a little bit to improve simulation performance
    servo_9.write(-90);
    delay(1000);
    senseUltrasonic();
  }
}

void senseUltrasonic(){
  // put your main code here, to run repeatedly: 
  float duration, distance, berat; 
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(10); 
  
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10); 
  //delay 10 micro suited with datasheet 
  digitalWrite(trigPin, LOW); 
  duration = pulseIn(echoPin, HIGH); 
  
  distance = (duration/2)*0.03448; 
  //sound speed formula

  //convert to kilogram
  berat = (17 - distance) / 0.01; //this formula based on observation 1 cm = 100 gr (0.01 cm = 1 gr)
  if(berat < 0){
    berat = 0;
  }
  
  //display on serial monitor 
  Serial.print("Berat = "); 
  Serial.print(berat); 
  Serial.println(" gram"); 

  Serial.print("Jarak = ");
  Serial.print(distance);
  Serial.println(" cm");
  delay(500); 

  ThingSpeak.writeField(channelID, 1, berat, writeAPIKey); //write to field jarak 
  delay(2000);
}

