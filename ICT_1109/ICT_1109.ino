#include <WiFi.h>
#include <SPI.h>

#include"AirQuality.h"
#include"Arduino.h"

AirQuality airqualitysensor;
int current_quality =-1;

char ssid[] = "Coalamovement";     //  연결하실 와이파이 이름
char pass[] = "a111111111";  // 네트워크 보안키
char serverIP[] = "164.125.68.162";

#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8
#define ledPin 9

int status = WL_IDLE_STATUS;
WiFiClient client;
int sendCount = 0 ;

char sendColorSum[3] ;
char* sensedColor = "" ;


void setup() {
  // put your setup code here, to run once:
  
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(ledPin, OUTPUT) ;
  
  pinMode(3,INPUT) ;
  pinMode(sensorOut, INPUT);
  
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);
  digitalWrite(ledPin, LOW) ;
  
  Serial.begin(9600);
  
  while ( status != WL_CONNECTED) { // 연결이 될 때 까지 반복
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // WPA/WAP2 네트워크에 연결
    status = WiFi.begin(ssid, pass);
    delay(2000);
  }

  // 연결에 성공했으면 연결확인 메시지와 연결된 네트워크 정보를 띄운다. 
  Serial.print("You're connected to the network");
  for (int i = 0 ; i < 3 ; i ++) {
      digitalWrite(ledPin, HIGH) ;
      delay(100) ;
      digitalWrite(ledPin, LOW) ;
      delay(100) ;
  }
  printCurrentNet();
  printWifiData();
  airqualitysensor.init(14);
  for (int i = 0 ; i < 3 ; i ++) {
      digitalWrite(ledPin, HIGH) ;
      delay(100) ;
      digitalWrite(ledPin, LOW) ;
      delay(100) ;
  }
}

void loop() { 
  
  digitalWrite(3, LOW) ;
  
  if( digitalRead(3) == HIGH ) {
    Serial.println("Button on") ;
    
    for (int i = 0 ; i < 3 ; i ++) {
      digitalWrite(ledPin, HIGH) ;
      delay(500) ;
      digitalWrite(ledPin, LOW) ;
      delay(500) ;
    }
    delay(3000) ;
    for (int i = 0 ; i < 3 ; i ++) {
      digitalWrite(ledPin, HIGH) ;
      delay(2000) ;
      sendColorSum[i] = colorSensor() ;
      Serial.println(sensedColor) ;
      digitalWrite(ledPin, LOW) ;
      delay(3000) ;
    }
    Serial.println(sendColorSum) ;
    sendColorData() ;
    for (int i = 0 ; i < 3 ; i ++) {
      digitalWrite(ledPin, HIGH) ;
      delay(200) ;
      digitalWrite(ledPin, LOW) ;
      delay(200) ;
    }
  }
  else if (sendCount == 100) {
    sendAirData() ;
    sendCount = 0 ;
  }
  else {
    Serial.println(airqualitysensor.first_vol) ;
    sendCount++ ;
    delay(1000) ;
  }
}

char colorSensor() {
  int red, green, blue ;
  int frequency = 0;
  
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  // Reading the output frequency
  frequency = pulseIn(sensorOut, LOW);
  //Remaping the value of the frequency to the RGB Model of 0 to 255
  frequency = map(frequency, 25,72,255,0);
  // Printing the value on the serial monitor
  Serial.print("R= ");//printing name
  Serial.print(frequency);//printing RED color frequency
  red = frequency;
  Serial.print("  ");
  delay(100);
  // Setting Green filtered photodiodes to be read
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  frequency = pulseIn(sensorOut, LOW);
  //Remaping the value of the frequency to the RGB Model of 0 to 255
  frequency = map(frequency, 30,90,255,0);
  // Printing the value on the serial monitor
  Serial.print("G= ");//printing name
  Serial.print(frequency);//printing RED color frequency
  green = frequency;
  Serial.print("  ");
  delay(100);
  // Setting Blue filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  frequency = pulseIn(sensorOut, LOW);
  //Remaping the value of the frequency to the RGB Model of 0 to 255
  frequency = map(frequency, 25,70,255,0);
  // Printing the value on the serial monitor
  Serial.print("B= ");//printing name
  Serial.print(frequency);//printing RED color frequency
  blue = frequency ;
  Serial.println("  ");
  
  int theColor = max(max(red, green),blue) ;
  delay(100);
  
  if (theColor == red ) {
    sensedColor = "RED" ;
    return '1' ;
  }
  else if (theColor == green ) {
    sensedColor = "GREEN" ;
    return '2' ;
  }
  else if (theColor == blue ) {
    sensedColor = "BLUE" ;
    return '3' ;
  }  

}

void sendAirData() {
  int airData = airqualitysensor.first_vol ;
  
  if(client.connect(serverIP, 80)){
    Serial.println("SendAirData...");
    client.print("GET /air.php?");
    client.print("airData=");
    client.print(airData); 
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(serverIP);
    client.println("Connection: close");
    client.println();
    client.println();
    Serial.print("Transfer terminated : ");
    Serial.println(airData) ;
    delay(2000);
    
  }else{
    Serial.println("Connection failed 1");
    delay(2000);
  }
  client.stop();
}

void sendColorData() {
  
  if(client.connect(serverIP, 80)){
    Serial.println("SendColorData...");
    client.print("GET /color.php?");
    client.print("colorData=");
    client.print(sendColorSum); 
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(serverIP);
    client.println("Connection: close");
    client.println();
    client.println();
    Serial.print("Transfer terminated : ");
    Serial.println(sendColorSum) ;
    delay(2000);
    
  }else{ 
    Serial.println("Connection failed 1");
    delay(2000);
  }
  client.stop();
  
  
}

ISR(TIMER1_OVF_vect)
{
  if(airqualitysensor.counter==61)//set 2 seconds as a detected duty
  {

      airqualitysensor.last_vol=airqualitysensor.first_vol;
      airqualitysensor.first_vol=analogRead(A0);
      airqualitysensor.counter=0;
      airqualitysensor.timer_index=1;
      PORTB=PORTB^0x20;
  }
  else
  {
    airqualitysensor.counter++;
  }
}

void printWifiData() {
  // WI-FI 실드의 IP를 출력한다.
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);
  
  // MAC어드레스를 출력한다.
  byte mac[6];  
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);
 
}

void printCurrentNet() {
  // 접속하려는 네트워크의 SSID를 출력한다.
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // 접속하려는 router의 MAC 주소를 출력한다.
  byte bssid[6];
  WiFi.BSSID(bssid);    
  Serial.print("BSSID: ");
  Serial.print(bssid[5],HEX);
  Serial.print(":");
  Serial.print(bssid[4],HEX);
  Serial.print(":");
  Serial.print(bssid[3],HEX);
  Serial.print(":");
  Serial.print(bssid[2],HEX);
  Serial.print(":");
  Serial.print(bssid[1],HEX);
  Serial.print(":");
  Serial.println(bssid[0],HEX);

  // 수신 신호 강도를 출력한다.
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // 암호화 타입을 출력한다.
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption,HEX);
  Serial.println();
}
