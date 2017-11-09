
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8
#define ledPin 9 

int frequency = 0;

void setup() {  
  Serial.begin(9600) ;
}
void loop() {
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
