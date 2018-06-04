#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

#include <Servo.h>

#define ROLL_AXIS_PIN A0 // X axis
#define PITCH_AXIS_PIN A1 // Y axis

#define YAW_AXIS_PIN A2 // Z axis
#define THRUST_AXIS_PIN A3

#define TESTER_MODE false

unsigned long lastUpdate = 0;

Servo esc;
Servo leftAileron;
Servo rightAileron;
Servo elevator;

int rollAxis = 0; // rotation by ailerons
int pitchAxis = 0; // rotation by elevator

int yawAxis = 0; // rotation by rudder
// Motor state
int thrust = 0;

const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

struct WirelessPackage {
  int thrust;
  int elevator;
  int aileron;
  int rudder;
};

WirelessPackage dataPacket = {};

RF24 radio(7, 8);
void setup() {
  Serial.begin(9600);
  radio.begin();
  //  pinMode(13, OUTPUT);

  //  digitalWrite(13, HIGH);
  delay(1000);
  //  digitalWrite(13, LOW);
  radio.setPALevel(RF24_PA_MAX);
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1, pipes[1]);
  radio.startListening();

  esc.attach(9);
  esc.writeMicroseconds(1000);

  rightAileron.attach(6);
  leftAileron.attach(5);
  elevator.attach(3);
}

void loop() {
//#if TESTER_MODE
//  getLocaJoysticState();
//#else
  reciveRemouteState();
//#endif
  moveServos();
   
   
}

int escSpeed(int val) {
  return map(val, 0, 1023, 1000, 2000); //mapping val to minimum and maximum(Change if needed)
}

int aileronDeg(int val) {
  return map(val, 0, 1023, 70, 98);  //  60-100 0-20-40 // 0 - 10 - 28
}

void moveServos() {
  int s = escSpeed(thrust);
  //    Serial.print("thrust=");
  //      Serial.print(s);
  //      Serial.print("  ");
  esc.writeMicroseconds(s);

  //    Serial.print("rollAxis=");
  //    Serial.print(t);
  //    Serial.print("  ");

  rightAileron.write(aileronDeg(rollAxis));
  //  if (lamdingMode) {
  //    leftAileron.write(180 - t);
  //  } else

  leftAileron.write(aileronDeg(rollAxis));

  elevator.write(map(pitchAxis, 0, 1023, 60, 100));
}

void reciveRemouteState() {

  unsigned long got_time = millis();
  if ( radio.available() ) {
    Serial.println("Radio");
    
      
//    Serial.println(thrust);
    bool done = false;
    while (!done){ 
      done = radio.read( &dataPacket, sizeof(dataPacket) );
      delay(20);
    }
    //printPackage(dataPacket);

    thrust = dataPacket.thrust;
    pitchAxis = dataPacket.elevator;
    yawAxis = dataPacket.rudder;
    rollAxis = dataPacket.aileron; 
  }

  if (got_time - lastUpdate > 10000) {
    thrust = 0;
  }
  if (got_time - lastUpdate > 5000) {
    thrust = 0;
    radio.begin();

    radio.setPALevel(RF24_PA_MAX);
    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1, pipes[1]);
    radio.startListening();
    delay(100);
  }
}

void getLocaJoysticState() {
  thrust = analogRead(A2);
  Serial.println(thrust);
}

void printPackage(const WirelessPackage &pckg) {
  Serial.print("packet: {thrust=");
  Serial.print(pckg.thrust);
  Serial.print(", elevator='");
  Serial.print(pckg.elevator);
  //  Serial.print("', rudder=");
  //  Serial.print(pckg.rudder);
  Serial.print("', aileron=");
  Serial.print(pckg.aileron);
  Serial.print("}\n");
}

