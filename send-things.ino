#include <DFRobot_sim808.h>
#include <SoftwareSerial.h>
#define PIN_TX 10
#define PIN_RX 11
SoftwareSerial mySerial(PIN_TX, PIN_RX);//allows you to create a serial communication interface on digital pins (class=mySerial, Library=SoftwareSerial)
DFRobot_SIM808 sim808(&mySerial);  //used to initalize the sim808 and the & parameter is used to passed the  address of the mySerial (This allows the DFRobot_SIM808 library to use the mySerial object for serial communication with the SIM808 module.)

float lat, lon;
char latStr[15];             
char lonStr[15];


void setup() {
  mySerial.begin(9600);// software-based serial communication with a specific baud rate.
  Serial.begin(9600);


  //*** Initialize sim808 module ******
  while (!sim808.init()) {
    delay(2000);

    Serial.print("Sim808 init error\r\n");
  }
  Serial.println("Sim808 init success");

  //***** Turn on the GPS power******
  if (sim808.attachGPS())
    Serial.println("Open the GPS power success");
  else
    Serial.println("Open the GPS power failure");

  delay(1000);
}

void loop() {

  if (sim808.getGPS()) {
    getGPSData();

    upload();
  }
}


void upload() {
  
  if (mySerial.available())
    Serial.write(mySerial.read());

  mySerial.println("AT");
  delay(1000);

  mySerial.println("AT+CPIN?");//checks is the sim inserted
  delay(1000);

  mySerial.println("AT+CREG?");//registation status of the gsm module with the network
  delay(1000);

  mySerial.println("AT+CGATT?");//checks the gsm module is attached to the gprs network
  delay(1000);

  mySerial.println("AT+CIPSHUT");//shutdown the previous TCP/IP connection
  delay(1000);

  mySerial.println("AT+CIPSTATUS");//retrieves the current status of the TCP/IP connection
  delay(2000);

  mySerial.println("AT+CIPMUX=0");
  delay(2000);

  

  mySerial.println("AT+CSTT=\"dialogbb\"");  //start task and setting the APN, (the APN is a gateway between a mobile network and the internet. It is a specific network setting that devices use to establish a data connection with the mobile carrier's network.)
  delay(1000);

  

  mySerial.println("AT+CIICR");  //bring up wireless connection
  delay(3000);

  

  mySerial.println("AT+CIFSR");  //get local IP adress
  delay(2000);

  

  mySerial.println("AT+CIPSPRT=0");
  delay(3000);

  

  mySerial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");  //start up the connection
  delay(2000);

  

  mySerial.println("AT+CIPSEND");  //begin send data to remote server
  delay(4000);
  

  dtostrf(lat, 10, 7, latStr);  // convert the latitude float to a string with 7 decimal places
  dtostrf(lon, 10, 7, lonStr);  // convert the longitude float to a string with 7 decimal places
  String str = "GET https://api.thingspeak.com/update?api_key=G19YCOK5KYFLXQ60&field1=" + String(latStr) + "&field2=" + String(lonStr);
  str.replace("field1= ", "field1=");  // remove the space between the latitude/longitude and "&field3"


  Serial.println(str);
  mySerial.println(str);  //begin send data to remote server


  delay(4000);
  

  mySerial.println((char)26);  //sending
  delay(2000);                 //waitting for reply, important! the time is base on the condition of internet
  mySerial.println();

  

  mySerial.println("AT+CIPSHUT");  //close the connection
  delay(100);
  
  delay(2000);

}

void getGPSData() {
  lat = (sim808.GPSdata.lat);
  lon = (sim808.GPSdata.lon);
 

  Serial.print(sim808.GPSdata.year);
  Serial.print("/");
  Serial.print(sim808.GPSdata.month);
  Serial.print("/");
  Serial.print(sim808.GPSdata.day);
  Serial.print(" ");
  Serial.print(sim808.GPSdata.hour);
  Serial.print(":");
  Serial.print(sim808.GPSdata.minute);
  Serial.print(":");
  Serial.print(sim808.GPSdata.second);
  Serial.print(":");
  Serial.println(sim808.GPSdata.centisecond);
  Serial.print("latitude :");
  Serial.println(lat, 5);//in here we sending 5 because 
  Serial.print("longitude :");
  Serial.println(lon, 5);
 
  Serial.print("heading :");
  Serial.println(sim808.GPSdata.heading);
}

