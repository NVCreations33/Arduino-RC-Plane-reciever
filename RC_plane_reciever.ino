//Get the Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

#define led 9

RF24 radio(3, 2);   
const byte address[6] = "00001";
unsigned long lastReceiveTime = 0;
unsigned long currentTime = 0;

Servo throttle;  
Servo rudderServo;
Servo elevatorServo;
Servo aileron1Servo;
Servo aileron2Servo;


int throttleValue, rudderValue, elevatorValue, aileron1Value, aileron2Value, travelAdjust;

struct Data_Package {
  byte j1PotX;
  byte j1PotY;
  byte j1Button;
  byte j2PotX;
  byte j2PotY;
  byte j2Button;
  byte pot1;
  byte pot2;
  byte tSwitch1;
  byte tSwitch2;
  byte button1;
  byte button2;
  byte button3;
  byte button4;
};
Data_Package data; 
void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening(); 
  resetData();
  throttle.attach(10);
  rudderServo.attach(4);   
  elevatorServo.attach(5); 
  aileron1Servo.attach(6); 
  aileron2Servo.attach(7); 
  pinMode(led, OUTPUT);    
}
void loop() {
  
  currentTime = millis();
  if ( currentTime - lastReceiveTime > 1000 ) { 
    resetData(); 
  }
  
  if (radio.available()) {
    radio.read(&data, sizeof(Data_Package)); 
    lastReceiveTime = millis(); 
  }

  
  throttleValue = constrain(data.j1PotY, 80, 255); 
  throttleValue = map(throttleValue, 80, 255, 1000, 2000);
  throttle.writeMicroseconds(throttleValue);

  
  travelAdjust = map(data.pot2, 0, 255, 0, 25);  
  
  
  elevatorValue = map(data.j2PotY, 0, 255, (85 - travelAdjust), (35 + travelAdjust));
  elevatorServo.write(elevatorValue);
  
  
  aileron1Value = map(data.j2PotX, 0, 255, (10 + travelAdjust), (80 - travelAdjust));
  aileron1Servo.write(aileron1Value);
  aileron2Servo.write(aileron1Value);

  
  if (data.j1PotX > 127) {
    rudderValue = data.pot1 + (data.j1PotX - 127);
  }
  if (data.j1PotX < 127) {
    rudderValue = data.pot1 - (127 - data.j1PotX);
  }
  
  rudderValue = map(rudderValue, 0, 255, (10 + travelAdjust), (90 - travelAdjust));
  rudderServo.write(rudderValue);

  
  int sensorValue = analogRead(A3);
  float voltage = sensorValue * (5.00 / 1023.00) * 3; 
  
  if (voltage < 11) {
    digitalWrite(led, HIGH);
  }
  else {
    digitalWrite(led, LOW);
  }
}
void resetData() {
  
  data.j1PotX = 127;
  data.j1PotY = 80; 
  data.j2PotX = 127;
  data.j2PotY = 127;
  data.j1Button = 1;
  data.j2Button = 1;
  data.pot1 = 1;
  data.pot2 = 1;
  data.tSwitch1 = 1;
  data.tSwitch2 = 1;
  data.button1 = 1;
  data.button2 = 1;
  data.button3 = 1;
  data.button4 = 1;
}
