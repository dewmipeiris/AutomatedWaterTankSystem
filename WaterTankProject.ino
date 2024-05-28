#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <Arduino.h>

#define FIREBASE_HOST "wtank-a555e-default-rtdb.asia-southeast1.firebasedatabase.app" // Firebase host
#define FIREBASE_AUTH "VMZMwrn32cW4qYkENflQHdJngA8I3qwni0kHkWE4" // Firebase Auth code
#define WIFI_SSID "Gamaya" // Enter your WiFi Name
#define WIFI_PASSWORD "senalgamage3" // Enter your WiFi password

const int trigPin = D2;  // Define the trigger pin //static
const int echoPin = D1; // Define the echo pin
const int waterPump = D8;  

int tankStatus;
int Refill;

void setup() {
  Serial.begin(9600);
  pinMode(waterPump, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println("Connected.");
  Serial.println(WiFi.localIP());
  
  // Initialize Firebase after the WiFi connection is established
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  // Check if Firebase connection is successful
  if (Firebase.failed()) {
    Serial.println("Firebase connection failed.");
  } else {
    Serial.println("Firebase connection successful.");
  }
}

void loop() {
  // Your loop logic here
  //tankStatus = Firebase.getInt("/TankStatus");
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float speedOfSound = 343.0;  // Speed of sound in meters per second (adjust as needed)
  float distance_cm = (duration * speedOfSound) / 20000.0;  // Convert to millimeters
  float distance_mm = distance_cm * 10;

  Serial.println(distance_cm);
  Firebase.setFloat("System/cm", distance_cm);
  delay(500);

  //logic to store information in firebase
  if (distance_cm < 5){
    digitalWrite(waterPump, LOW);
    Firebase.setInt("System/Refill", 0);
    Firebase.setString("System/TankStatus", "Tank is Full");
    //setfirebase to off the refill button
  }else{
    //logic waits for firebase information to refill
    Refill = Firebase.getInt("System/Refill");
    if (Refill == 1){
      digitalWrite(waterPump, HIGH);
      Serial.println(1);
    }else{
      digitalWrite(waterPump, LOW);
      Serial.println(0);
    }
  }

  if(distance_cm >= 3 && distance_cm < 4.5){
    Firebase.setString("System/TankStatus", "Tank is Half Filled");
  }else if(distance_cm > 4.5){
    Firebase.setString("System/TankStatus", "Water level is low, need a Refill!");
  }else if(distance_cm < 3){
    Firebase.setString("System/TankStatus", "Tank is Full");
  }
}
