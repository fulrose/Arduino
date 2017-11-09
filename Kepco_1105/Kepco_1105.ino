#include <WiFi.h>
#include <SPI.h>
#include "EmonLib.h"                   // Include Emon Library
#include <LiquidCrystal.h>

LiquidCrystal lcd(3, 4, 10, 11, 12, 13) ;

EnergyMonitor emon1;                   // Create an instance

char ssid[] = "AndroidHotspot7278";     //  연결하실 와이파이 이름
char pass[] = "asdf12333";  // 네트워크 보안키
int status = WL_IDLE_STATUS;
char serverIP[] = "164.125.68.162";
WiFiClient client;

  float power_g ;
  float power_s ;

void setup() {

  Serial.begin(9600); 
  lcd.begin(16, 2) ;
  lcd.clear() ;
  //emon1.current(1, 111.1);
  
  // 현재 아두이노에 연결된 실드를 확인
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present"); 
    // 와이파이 실드가 아닐 경우
    while(true);
  } 
  
 // 와이파이에 연결 시도
  while ( status != WL_CONNECTED) { // 연결이 될 때 까지 반복
    
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    lcd.clear() ;
    lcd.setCursor(0,0) ;
    lcd.print("Wifi Init...") ;
    // WPA/WAP2 네트워크에 연결
    status = WiFi.begin(ssid, pass);
    delay(3000);
  }
   
  // 연결에 성공했으면 연결확인 메시지와 연결된 네트워크 정보를 띄운다. 
  Serial.print("You're connected to the network");
  lcd.setCursor(0,1) ;
  lcd.print("Connected!") ;
  delay(1000) ;
  printCurrentNet();
  printWifiData();
  lcd.clear() ;
}

void loop() {

  //double Irms = emon1.calcIrms(1480);  // Calculate Irms only
  
  //Serial.print(Irms*220.0);         // Apparent power
  //Serial.print(" ");
  //Serial.println(Irms);          // Irms

  updatePowerValue() ;  
  
  lcd.setCursor(0,0) ;
  lcd.print("G : ") ;
  lcd.setCursor(4,0) ;
  lcd.print(power_g) ;
  lcd.setCursor(9,0) ;
  lcd.print(" [kWh]");
  
  lcd.setCursor(0,1) ;
  lcd.print("S : ") ;
  lcd.setCursor(4,1) ;
  lcd.print(power_s) ;
  lcd.setCursor(9,1) ;
  lcd.print(" [kWh]");
  
  if(client.connect(serverIP,80)){
    Serial.println("Connected 1...");
    client.print("GET /used.php?");
    client.print("e_num=1&");
    client.print("value=");
    client.print(power_g); 
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(serverIP);
    client.println("Connection: close");
    client.println();
    client.println();
    Serial.println("Transfer terminated 1");
    lcd.setCursor(15,0) ;
    lcd.print(".") ;
    delay(3000);
  }else{
    delay(1000);
    Serial.println("Connection failed 1");
    delay(2000);
  }
  delay(1000) ;
  client.stop();
  
  if(client.connect(serverIP,80)){
    Serial.println("Connected 2...");
    client.print("GET /update_c.php?");
    client.print("e_num=1&");
    client.print("cap=");
    client.print(power_s); 
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(serverIP);
    client.println("Connection: close");
    client.println();
    client.println();
    Serial.println("Transfer terminated 2");
    lcd.setCursor(15,1) ;
    lcd.print(".") ;
    delay(3000);
  }else{
    delay(1000);
    Serial.println("Connection failed 2");
    delay(000);
  }  
  delay(1000);
  client.stop();
  delay(1000) ;
  lcd.clear() ;
}

void updatePowerValue() {
  int power_g_r = random(290, 312) ;
  power_g = power_g_r * (0.01F) ;

  int power_s_r = random(134, 196) ;
  power_s = power_s_r * (0.01F) ;
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
