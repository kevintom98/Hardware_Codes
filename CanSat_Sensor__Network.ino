//Libraries
#include  <SoftwareSerial.h>
#include <DS3231.h>
#include <Wire.h>
#include <XBee.h>
#include <AltSoftSerial.h>
#include <TinyGPS++.h>
#include <Adafruit_BMP085.h>
#include <String.h>

//Objects
AltSoftSerial altSerial;
TinyGPSPlus gps;
Adafruit_BMP085 bmp;
DS3231  rtc(SDA,SCL);
SoftwareSerial XBee(2, 3); // RX, TX
#define NUM_SAMPLES 10
//Variavbles
String times;
String hh,mm,ss,packetcounts;
String st,sp,sa,lati,longi,alt,num_sat,hrs,mins,secs;
int na,np,nt,nlati,nlongi,nalt,n_numSat,nhrs,nmins,nsecs,ntimes,packetcount=0,packclen,npitch,nroll,nvolt,sai;
char char_array7[10],char_array8[10],char_array9[10],char_array10[10],char_array11[10],char_array12[10],char_array13[10],char_array14[10], char_array15[25], char_array16[25], char_array17[10];

int ADXL345 = 0x53; // The ADXL345 sensor I2C address
float X_out, Y_out, Z_out,volte;  // Outputs
float roll,pitch,rollF,pitchF;
String s_roll,s_pitch;

int sum = 0;                    // sum of samples taken
unsigned char sample_count = 0; // current sample number
float voltage = 0.0;            // calculated voltage


//Buzzer
const int buzzer = 10;

void setup()
{
  XBee.begin(9600);
  Serial.begin(9600);
  while (!Serial) ;
  altSerial.begin(9600);
  
  //BMP Sensor checking
  if (!bmp.begin())
  {
   Serial.println("Could not find a valid BMP085 sensor, check wiring!");
   while (1) {}
  }
  rtc.begin();
  rtc.setTime(0,0,0);
  Wire.begin(); // Initiate the Wire library
  Wire.beginTransmission(ADXL345); // Start communicating with the device
  Wire.write(0x2D); // Access/ talk to POWER_CTL Register - 0x2D
  Wire.write(8); // Bit D3 High for measuring enable (8dec -> 0000 1000 binary)
  Wire.endTransmission();
  delay(10);
  //Off-set Calibration
  //X-axis
  Wire.beginTransmission(ADXL345);
  Wire.write(0x1E);
  Wire.write(1);
  Wire.endTransmission();
  delay(10);
  //Y-axis
  Wire.beginTransmission(ADXL345);
  Wire.write(0x1F);
  Wire.write(-2);
  Wire.endTransmission();
  delay(10);
  //Z-axis
  Wire.beginTransmission(ADXL345);
  Wire.write(0x20);
  Wire.write(-9);
  Wire.endTransmission();
  delay(10);

  pinMode(buzzer, OUTPUT);
}


void loop()
{
  Serial.println("RTC values");
  times=rtc.getTimeStr();
  ntimes=times.length();
  char char_array0[ntimes+1];
  strcpy(char_array0, times.c_str());
  Serial.println(char_array0);
 
  hh=times.substring(0,2);
  nhrs=hh.length();
  char char_array1[nhrs+1];
  strcpy(char_array1, hh.c_str());
  Serial.println(char_array1);
 
  mm=times.substring(3,5);
  nmins=mm.length();
  char char_array2[nmins+1];
  strcpy(char_array2, mm.c_str());
  Serial.println(char_array2);
 
  ss=times.substring(6,8);
  nsecs=ss.length();
  char char_array3[nsecs+1];
  strcpy(char_array3, ss.c_str());
  Serial.println(char_array3);

        Serial.println("BMP values");
        st = bmp.readTemperature();
        nt=st.length();
        char char_array4[nt+1];
        strcpy(char_array4, st.c_str());
        Serial.println(char_array4);
       
        sp = bmp.readPressure();
        np=sp.length();
        char char_array5[np+1];
        strcpy(char_array5, sp.c_str());
        Serial.println(char_array5);

        sa = bmp.readAltitude();
        sai=sa.toInt();
        na=sa.length();
        char char_array6[na + 1];
        strcpy(char_array6, sa.c_str());
        Serial.println(char_array6);
        //delay(500);


 //Packetcount
 packetcounts=String(packetcount);
 packclen=packetcounts.length();
 char char_array14[packclen+1];
 strcpy(char_array14,packetcounts.c_str());
 Serial.println(char_array14);
 

 //First Printing
       XBee.write("5515,");
       //XBee.write(char_array0);
       //XBee.write(",");
       XBee.write(char_array1);  //hours
       XBee.write(":");
       XBee.write(char_array2);  //minutes
       XBee.write(":");
       XBee.write(char_array3);  //seconds
       XBee.write(",");
       XBee.write(char_array14);  //Packetcount
       XBee.write(",");
       XBee.write(char_array6);  //Altitude
       XBee.write(",");
       XBee.write(char_array5);  //Pressure
       XBee.write(",");
       XBee.write(char_array4);  //Temprature
       XBee.write(",");

       
   //voltage regultor
   while (sample_count < NUM_SAMPLES)
   {
        sum += analogRead(A2);
        sample_count++;
        delay(10);
    }
    voltage = ((float)sum / (float)NUM_SAMPLES * 4.3) / 1024.0;
    volte=voltage*12.67;
    Serial.print(volte);
    Serial.println (" V");
   
   
    sample_count = 0;
    sum = 0;

    String svoltage;
    svoltage=String(volte);
    nvolt = svoltage.length();
    char char_array17[nvolt+1];
    strcpy(char_array17,svoltage.c_str());

   
    XBee.write(char_array17);
  gps.encode(altSerial.read());
      Serial.println("GPS values");
      lati = gps.location.lat();
      nlati = lati.length();
      char char_array7[nlati+1];
      strcpy(char_array7, lati.c_str());
      Serial.println(char_array7);
     
     
       
      longi = gps.location.lng();
      nlongi = longi.length();
      char char_array8[nlongi+1];
      strcpy(char_array8, longi.c_str());
      Serial.println(char_array8);

      alt = gps.altitude.meters();
      nalt = alt.length();
      char char_array9[nalt+1];
      strcpy(char_array9, alt.c_str());
      Serial.println(char_array9);
     

      num_sat = gps.satellites.value();
      n_numSat = num_sat.length();
      char char_array10[n_numSat+1];
      strcpy(char_array10,num_sat.c_str());
      Serial.println(char_array10);

      hrs = gps.time.hour();
      nhrs = hrs.length();
      char char_array11[nhrs+1];
      strcpy(char_array11,hrs.c_str());
      Serial.print(char_array11);


      mins = gps.time.minute();
      nmins = mins.length();
      char char_array12[nmins+1];
      strcpy(char_array12,mins.c_str());
      Serial.print(char_array12);


      secs = gps.time.second();
      nsecs = secs.length();
      char char_array13[nmins+1];
      strcpy(char_array13,secs.c_str());

       
       XBee.write(",");
       XBee.write(char_array11);   //hours
       XBee.write(":");
       XBee.write(char_array12);   //minute
       XBee.write(":");
       XBee.write(char_array13);   //seconds
       XBee.write(",");
       XBee.write(char_array7);   //latitude
       XBee.write(",");
       XBee.write(char_array8);   //longitude
       XBee.write(",");
       XBee.write(char_array9);    //altitude
       XBee.write(",");
       XBee.write(char_array10);   //number of satellite
       
     
       //accelerometer
       Serial.println("adxl values");
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  X_out = ( Wire.read() | Wire.read() << 8); // X-axis value
  X_out = X_out / 256; //For a range of +-2g, we need to divide the raw values by 256, according to the datasheet
  Y_out = ( Wire.read() | Wire.read() << 8); // Y-axis value
  Y_out = Y_out / 256;
  Z_out = ( Wire.read() | Wire.read() << 8); // Z-axis value
  Z_out = Z_out / 256;
  // Calculate Roll and Pitch (rotation around X-axis, rotation around Y-axis)
  roll = atan(Y_out / sqrt(pow(X_out, 2) + pow(Z_out, 2))) * 180 / PI;
  pitch = atan(-1 * X_out / sqrt(pow(Y_out, 2) + pow(Z_out, 2))) * 180 / PI;
  // Low-pass filter
  rollF = 0.94 * rollF + 0.06 * roll;
  pitchF = 0.94 * pitchF + 0.06 * pitch;
  s_roll = String(rollF,3);
  s_pitch = String(pitchF,3);
  Serial.println(s_pitch);
  npitch = s_pitch.length();
  char char_array15[npitch+1];
  strcpy(char_array15,s_pitch.c_str());
  Serial.println(char_array15);
  Serial.println(s_roll);
  nroll = s_roll.length();
  char char_array16[nroll+1];
  strcpy(char_array16,s_roll.c_str());
   
  Serial.println(char_array16);
 
  XBee.write(",");
  XBee.write(char_array15);
  XBee.write(",");
  XBee.write(char_array16);
  XBee.write(",");
  //Blade spin rate
  XBee.write("0,");
  //Software state
  if(sai > 600)
  XBee.write("1");
  if(sai<450)
  XBee.write("2");
  XBee.write("\n");


  //Buzzer
  if(sai<340)
  digitalWrite(10,HIGH);
  Serial.println();
  Serial.println("end of frame");
  packetcount++;

 
  delay(1000);
}
