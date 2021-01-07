/*******************************************************************************************************************************
Authors:
Rashmitha K M
Kevin Tom 

Table Name: Bool
ID 45 - Humidity
ID 46 - Temperature
ID 47 - Light intensity
ID 48 - Latitude
ID 49 - longitude


Importnat Note:
Please use HTTP links for updating not HTTPS. IF you are using HTTPS please include the secure library for HTTPS for NodeMCU.
We are using HTTP links for updating.
*********************************************************************************************************************************/



//Libraries
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <DHT.h>


//WiFi credentials
const char* wifiName = "WiFI_Name";
const char* wifiPass = "Password";


//PIN Definitions
#define DHTTYPE DHT11
#define LDRPIN A0
#define DHT11P D3


//Sensor intializations
DHT dht(DHT11P, DHTTYPE);
TinyGPSPlus gps;
SoftwareSerial ss(D2,D1);

HTTPClient http;

int LDRValue =0;
double latitude,longitude;


//Get method link
const char *host = "PUT HTTP GET METHOD LINK";



//Put function, this function is used to put (edit values in DB)
void putindb(double a, int id)
{
 http.begin("PUT METHOD LINK"+String(id));
 http.addHeader("Content-Type", "application/json");
 http.PUT("{\"id\":" +String(id)+",\"status\":"+String(0)+",\"value\":"+String(a)+"}");
 http.end();
}




void updatedb(int id)
{  
  http.begin(host+String(id));  
  int httpCode = http.GET();         
  String payload = http.getString();  // Getting the string
  payload.replace("<pre>","");        //Removing <pre> string from the get request reply and replay with ""
  Serial.print("Response Code:");
  Serial.println(httpCode);           //Printing response code
  Serial.print("Returned data from Server:");
  Serial.println(payload);           //Parsed data printing
  
  if(httpCode == 200)
  {
    const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;  
    DynamicJsonBuffer jsonBuffer(capacity);
    JsonObject& root = jsonBuffer.parseObject(payload);
    if (!root.success())
    {
      Serial.println(F("Parsing failed!"));
      return;
    }
    int a = root["status"].as<int>(); //Collecting only status value of the given ID
    http.end();

    

    if(a==1 && id==45) //Humidity
    {
      float h = dht.readHumidity();
      Serial.println("Humidity:"); 
      Serial.print(h);
      putindb(h,id);
          
    }
    
    if(a==1 && id==46) //Temperature
    {
      float t = dht.readTemperature();
      Serial.println("Temperature");
      Serial.print(t);
      putindb(t,id);
    }
    if(a==1 && id==47) //Light Intensity
    {
      LDRValue = analogRead(LDRPIN);
      Serial.println("Light Intensity");
      Serial.print(LDRValue);
      putindb(LDRValue,id);
    }
    
    if(a==1 && id==48) //Latitude
    {
      while (ss.available() > 0)
        if (gps.encode(ss.read()))
        {
          if (gps.location.isValid())
          {
            latitude = gps.location.lat();
          }
        }
      Serial.println("Latitude");
      Serial.print(latitude);
      putindb(latitude,id); 
    }
    
    if(a==1 && id==49) //Longitude
    {
      while (ss.available() > 0)
        if (gps.encode(ss.read()))
        {
          if (gps.location.isValid())
          {
            longitude = gps.location.lng();
          }
        }
      Serial.println("Longitude");
      Serial.print(longitude);
      putindb(longitude,id); 
    }   
  }
}




void setup() 
{
 Serial.begin(9600);
 ss.begin(9600);
 delay(1000);
 Serial.println();
 Serial.print("Connecting to ");
 Serial.println(wifiName);
 WiFi.begin(wifiName, wifiPass);
 while (WiFi.status() != WL_CONNECTED) 
 {
   delay(500);
   Serial.print(".");
 }
 Serial.println("");
 Serial.println("WiFi connected");
 Serial.println("IP address: ");
 Serial.println(WiFi.localIP());
}





void loop() 
{
updatedb(45);
updatedb(46);
updatedb(47);
updatedb(48); 
updatedb(49);
delay(15000);  //GET Data at every 15 seconds
}
