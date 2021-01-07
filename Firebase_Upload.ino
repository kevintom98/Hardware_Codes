#include <Firebase.h>
#include <FirebaseArduino.h>
#include <FirebaseCloudMessaging.h>
#include <FirebaseError.h>
#include <FirebaseHttpClient.h>
#include <FirebaseObject.h>

#include <ArduinoJson.h>

#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#include <ESP8266WiFi.h>
#include <DHT.h>


#define FIREBASE_HOST "YOUR_FIREBASE_NAME.firebaseio.com"                    
#define FIREBASE_AUTH "FIREBASE AUTHENTICATION KEY"            

#define WIFI_SSID "YOUR WIFI SSID"                                            
#define WIFI_PASSWORD "YOUR WIFI PASSWORD"                                    


#define DHTTYPE DHT11
#define LDRPIN A0
#define DHT11P D3

DHT dht(DHT11P, DHTTYPE);
TinyGPSPlus gps;
SoftwareSerial ss(D2,D1);


int LDRValue =0;
double latitude,longitude;

void setup()
{
 Serial.begin(9600);
 ss.begin(9600);
 delay(1000);
 WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                    
 Serial.print("Connecting to ");
 Serial.print(WIFI_SSID);
 while (WiFi.status() != WL_CONNECTED)
 {
   Serial.print(".");
   delay(1000);
 }
 Serial.println();
 Serial.print("Connected to ");
 Serial.println(WIFI_SSID);
 Serial.print("IP Address is : ");
 Serial.println(WiFi.localIP());                                            
 Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);                              
 dht.begin();
 delay(1000);
}


void loop()
{
//reading pin data
float h = dht.readHumidity();
float t = dht.readTemperature();
LDRValue = analogRead(LDRPIN);
while (ss.available() > 0)
if (gps.encode(ss.read()))
{
if (gps.location.isValid())
 {
   latitude = gps.location.lat();
   longitude = gps.location.lng();
 }
}
//printing data
Serial.print("Humidity =");
Serial.print(h);
Serial.println("%  ");
Serial.print("Temperature =");
Serial.print(t);
Serial.println("C ");
Serial.print("LDR =");
Serial.println(LDRValue);
Serial.println(latitude);
Serial.println(longitude);
delay(1000);

String fireHumid = String(h) + String("%");
String fireTemp = String(t) + String("°C");  
String fireLDR = String(LDRValue);
String firelat = String(latitude);
String firelong = String(longitude);


Firebase.pushString("/DHT11/Humidity", fireHumid);    
Firebase.pushString("/DHT11/Temperature", fireTemp);
Firebase.pushString("/LDR/Values", fireLDR);
Firebase.pushString("/GPS/Latitude", firelat);
Firebase.pushString("GPS/Longitude",firelong);                              
}
