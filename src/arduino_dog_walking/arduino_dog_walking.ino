#include <Servo.h>

// Create servo objects for each leg
Servo frontLeftLeg;
Servo frontRightLeg;
Servo backLeftLeg;
Servo backRightLeg;

// Define servo pin connections
const int frontRightPin = 3;
const int frontLeftPin = 5;
const int backLeftPin = 6;
const int backRightPin = 9;

// Define movement angles
int forwardAngle = 45;
int backwardAngle = 135;
int restAngle = 90;


int pos = 0;    // variable to store the servo position



void setup() {

  // Attach servos to pins
  frontLeftLeg.attach(frontLeftPin);
  frontRightLeg.attach(frontRightPin);
  backLeftLeg.attach(backLeftPin);
  backRightLeg.attach(backRightPin);

  // Set initial positions to the resting angle
  frontLeftLeg.write(restAngle);
  frontRightLeg.write(restAngle);
  backLeftLeg.write(restAngle);
  backRightLeg.write(restAngle);

  delay(1000); // Wait for the servos to stabilize
}



void loop() {

  // Step 1: Move front-left and back-right legs forward
  frontLeftLeg.write(forwardAngle);
  backRightLeg.write(forwardAngle);
  delay(500);

  // Step 2: Move front-right and back-left legs backward
  frontRightLeg.write(backwardAngle);
  backLeftLeg.write(backwardAngle);
  delay(500);

  // Step 3: Move front-left and back-right legs backward
  frontLeftLeg.write(backwardAngle);
  backRightLeg.write(backwardAngle);
  delay(500);

  // Step 4: Move front-right and back-left legs forward
  frontRightLeg.write(forwardAngle);
  backLeftLeg.write(forwardAngle);
  delay(500);

  // Reset to resting position
  frontLeftLeg.write(restAngle);
  frontRightLeg.write(restAngle);
  backLeftLeg.write(restAngle);
  backRightLeg.write(restAngle);
  delay(500);

  // for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
  //   // in steps of 1 degree
  //   frontLeftLeg.write(pos);              // tell servo to go to position in variable 'pos'
  //   frontRightLeg.write(pos);              // tell servo to go to position in variable 'pos'
  //   delay(15);                       // waits 15 ms for the servo to reach the position
  // }
  // for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
  //   frontLeftLeg.write(pos);              // tell servo to go to position in variable 'pos'
  //   frontRightLeg.write(pos);              // tell servo to go to position in variable 'pos'
  //   delay(15);                       // waits 15 ms for the servo to reach the position
  // }



}