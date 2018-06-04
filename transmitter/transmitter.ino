#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <Servo.h>

#define ROLL_AXIS_PIN A1 // X axis rotation by ailerons
#define PITCH_AXIS_PIN A0 // Y axis rotation by elevator
#define YAW_AXIS_PIN A3 // Z axis rotation by rudder
#define THRUST_AXIS_PIN A2 //Thrust ,.


struct WirelessPackage {
  int thrust;
  int elevator;
  int aileron;
  int rudder;
};

const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
RF24 radio(5, 6);
WirelessPackage dataPacket = {};

void setup() {
  Serial.begin(9600);

  radio.begin();

  pinMode (3, OUTPUT);
  pinMode (4, INPUT);
  pinMode (A5, INPUT);
  pinMode(THRUST_AXIS_PIN, INPUT);

  // Reads the initial state of the outputA

  radio.setPALevel(RF24_PA_MAX);
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1, pipes[0]);  
  radio.setRetries(15,15);
  radio.startListening();
}
void loop() {
  readJoysticValues(); 
  sendData(); 
  delay(20);
}

void readJoysticValues() {
  //  if (analogRead(A5) > 500)
  //    dataPacket.thrust = 1023;
  //  else if (analogRead(A3)  > 500)
  //    dataPacket.thrust = 0;
  //  else
  dataPacket.thrust = analogRead(A2);
  dataPacket.elevator = analogRead(PITCH_AXIS_PIN);
  dataPacket.aileron =  analogRead(ROLL_AXIS_PIN);

}

void sendData() {
  radio.stopListening();

  Serial.print("Sending ");
//  printPackage(dataPacket);
  digitalWrite(3, HIGH);

  
  bool ok = radio.write(&dataPacket, sizeof(dataPacket)); 
  if (ok)
    Serial.println(" ok...");
  else  
    Serial.println(" FAIL...");

  digitalWrite(3, LOW);
  dataPacket = {};
  radio.startListening();

}

void printPackage(const WirelessPackage &dataPacket) {
  Serial.print("packet: {thrust=");
  Serial.print(dataPacket.thrust);
  Serial.print(", elevator='");
  Serial.print(dataPacket.elevator);
  //TODO rudder is not supported
  //  Serial.print("', rudder=");
  //  Serial.print(dataPacket.rudder);
  Serial.print("', aileron=");
  Serial.print(dataPacket.aileron);
  Serial.print("}\n");
}
