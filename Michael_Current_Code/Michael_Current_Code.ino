#include <Tic.h>

#ifdef SERIAL_PORT_HARDWARE_OPEN
#define ticSerial SERIAL_PORT_HARDWARE_OPEN
#else
#include <SoftwareSerial.h>
SoftwareSerial ticSerial(11, 10);
#endif

TicSerial tic(ticSerial);

//Set up pin locations.  These will not change
const int buttonPinA = 4;
const int buttonPinB = 5;
const int buttonPinC = 6;
const int buttonPinD = 7;

// Initialize each buttonState status as being 0
int buttonStateA = 0; 
int buttonStateB = 0;
int buttonStateC = 0;
int buttonStateD = 0;

// Define the initial runmode of the system
auto mode = "constantVolume";
// Possible mode options:
// constantVolume
// constantVelocity
// setVolume

// Define button press states
auto buttonState = "noPress";
// Possible button state options include:

// Define initial position of the actuator
double currentPosition = 0;
// Define the target position of the actuator, which will be used to tell the actuator how much to move
double targetposition = 0;
// Precalculated steps to fill volume
double setVstep = 500;
// Precalculated velocity for flow rate
double targetVelocity = 4000000;

//*************************************************//
// Arduino Startup Code
//*************************************************//
void setup()
{
  // initialize the pushbutton pin as inputs:
  pinMode(buttonPinA, INPUT);
  pinMode(buttonPinB, INPUT);
  pinMode(buttonPinC, INPUT);
  pinMode(buttonPinD, INPUT);
  
  // Set up Driver and Serial Monitor
  ticSerial.begin(9600);
  Serial.begin(9600);
  Wire.begin();

  // Give time for the TIC to start up
  delay(20);
  
  // Set the Tic's current position to 0, so that when we command
  // it to move later, it will move a predictable amount.
  tic.haltAndSetPosition(0);

  // Tells the Tic that it is OK to start driving the motor.  The
  // Tic's safe-start feature helps avoid unexpected, accidental
  // movement of the motor: if an error happens, the Tic will not
  // drive the motor again until it receives the Exit Safe Start
  // command.  The safe-start feature can be disbled in the Tic
  // Control Center.
  tic.exitSafeStart();
}

//*************************************************//
// Arduino Operating Code
//*************************************************//
void loop()
{
// read the state of the pushbutton values:
checkButtons();

//*************************************************//
// Operating Code for Preset Volume Fill
//*************************************************//
if(mode == "constantVolume"){
  checkButtons();
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonStateA == HIGH) {
    tic.setTargetPosition(+500);
    waitForPosition(500);
  }
  else if (buttonStateB == HIGH) {
    tic.setTargetPosition(-500);
    waitForPosition(-500);
  }
  else if (buttonState == "dPressed") {
    mode = "setVolume";
    buttonState = "";
  }
}

//*************************************************//
// Operating Code for Operator Set Volume Fill
//*************************************************//
else if(mode == "setVolume"){
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonStateA == HIGH) {
    tic.setTargetPosition(+500);
    waitForPosition(500);
    Serial.println(1);
  }
  else if (buttonStateB == HIGH) {
    tic.setTargetPosition(-500);
    waitForPosition(-500);
    Serial.println(1);
  }
  else if (buttonStateC == HIGH){
    while(buttonStateC == HIGH){
    checkButtons();
    tic.setTargetVelocity(targetVelocity);
    }
  }
  else if (buttonState == "dPressed") {
    mode = "freeFill";
    buttonState = "";
  }
}

//*************************************************//
// Operating Code for Free Fill
//*************************************************//
else if(mode == "freeFill"){
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonStateA == HIGH) {
    while(buttonStateA == HIGH){
    checkButtons();
    tic.setTargetVelocity(targetVelocity);
    }
  }
  else if (buttonStateB == HIGH) {
    while(buttonStateB == HIGH){
    checkButtons();
    tic.setTargetVelocity(-targetVelocity);
    }
  }
  else if (buttonStateC == HIGH) {
    tic.setTargetPosition(-currentPosition);
    waitForPosition(-currentPosition);
  }
  else if (buttonState == "dPressed") {
    mode = "constantVolume";
    buttonState = "";
  }
}

// Saves the current position of the actuator
currentPosition = currentPosition + tic.getCurrentPosition();
Serial.println(currentPosition);
// Stop moving the actuator and set its position to zero
tic.haltAndSetPosition(0);
Serial.println(mode);
}


//*************************************************//
// Functions for operating the tic motor controller
//*************************************************//
void resetCommandTimeout()
{
  tic.resetCommandTimeout();
}

void delayWhileResettingCommandTimeout(uint32_t ms)
{
  uint32_t start = millis();
  do
  {
    resetCommandTimeout();
  } while ((uint32_t)(millis() - start) <= ms);
}


void waitForPosition(int32_t targetPosition)
{
  do
  {
    resetCommandTimeout();
  } while (tic.getCurrentPosition() != targetPosition);
}

void checkButtons(){
  // Read the button state of each button
  buttonStateA = digitalRead(buttonPinA);
  buttonStateB = digitalRead(buttonPinB);
  buttonStateC = digitalRead(buttonPinC);
  buttonStateD = digitalRead(buttonPinD);

  while(buttonStateD == HIGH){
    buttonStateD = digitalRead(buttonPinD);
    
    buttonState = "dPressed";
    Serial.println(buttonState);
  }
}
