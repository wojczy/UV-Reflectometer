// Firmware for UV-Reflectometer v0.1 KWO Sept2020

String inputString = "";          // a String to hold incoming data
boolean stringComplete = false;   // whether the string is complete
#include <CheapStepper.h>
CheapStepper stepper (8,9,10,11); // connect pins 8,9,10,11 to IN1,IN2,IN3,IN4 on ULN2003 board
boolean moveClockwise = false;    // initial direction of rotation
int analogPin = A0;               // UV-Sensor Input Pin
int val = 0;                      // variable to store the value read
char Out_string[7];               // String that will send via usb port.

void setup() {

  inputString.reserve(200);
  stepper.setRpm(10);              // accepted RPM range: 6RPM (may overheat) - 24RPM (may skip)
  Serial.begin(9600);
  pinMode(3,INPUT_PULLUP);         // Belt-Switch
  pinMode(4,INPUT_PULLUP);         // Lower Panel Switch
  pinMode(5,INPUT_PULLUP);         // Upper Panel Switch
  pinMode(2,OUTPUT);               // UV Lamp Relais
}

void loop() {

  moveClockwise = false;           // initialize variables
  int statBeltSw = 0;
  int UVvalues[50];
  int k = 0;
  int UVmean = 0;

 do { // Startup...make sure that stepper is in end position, otherwise move it until end-switch activated
  if (digitalRead(3) == HIGH) {
    stepper.moveDegrees (moveClockwise, 1);
  }
  else {
    statBeltSw = 1;
  }
 
  delay(100);
   } while(statBeltSw == 0);
   moveClockwise = true;
   delay(100);
   while (stringComplete == false) {
    serialEvent();
   }
   
if (inputString == "check.") { // Simple check if ARDUINO is responsive
 Serial.print("Ready.");
 }
 
 if (inputString == "scan.") { // Scan routine: stepwise moving of stepper with return of UV intensity value
 val = 0;
   for (int i=1; i<= 200; i++) {
    stepper.moveDegrees (moveClockwise, 1);
    delay(50);
     for (int j=1; j<= 50; j++) {
      val = val + analogRead(analogPin);  // read the input pin
      UVvalues[j] = analogRead(analogPin);
     }
     val = val / 50;
    for (int j=1; j<= 50; j++) {
     if (UVvalues[j] > 0.5 * val && UVvalues[j] <  2.0 * val) {
      k = k + 1;
      UVmean = UVmean + UVvalues[j];
     }
    }
    if (k!=0){
     UVmean = UVmean / k; // algorithm to smooth sensor reading
    }
    else {
     UVmean = 0;
    }
   Serial.print(String(UVmean));
   Serial.print(".");
   delay(100);
   UVmean=0;
   k=0;
  }
  moveClockwise = false;
  statBeltSw = 0;


 do {
  if (digitalRead(3) == HIGH) { // End scan...make sure that stepper is in end position, otherwise move it until end-switch activated
   stepper.moveDegrees (moveClockwise, 1);
  }
  else {
   statBeltSw = 1;
  }
  delay(100);
  } while(statBeltSw == 0);
   moveClockwise = true;
   delay(100);
   Serial.print("Ready.");
  }

  
  if (inputString == "upanel.") { // Check if upper panel is in
   if (digitalRead(5) == LOW) { 
    Serial.print("ok.");
   }
   else {
    Serial.print("no.");
   }
  }
  
 if (inputString == "lpanel.") {  // Check if lower panel is in
  if (digitalRead(4) == LOW) { 
   Serial.print("ok.");
  }
  else {
   Serial.print("no.");
  }
 }

 if (inputString == "UVon.") {  // Switch UV source on
  digitalWrite(2, HIGH);
  Serial.print("UVison.");
 }
 
 if (inputString == "UVoff.") { // Switch UV source off
  digitalWrite(2, LOW);
  Serial.print("UVisoff.");
 }
  
 inputString = "";
 stringComplete = false;

}

void serialEvent() { // Serial input routine
   while (Serial.available()) {
     // get the new byte:
     char inChar = (char)Serial.read();
     // add it to the inputString:
     inputString += inChar;
     // if the incoming character is a newline, set a flag so the main loop can
     // do something about it:
     if (inChar == '.') {
       stringComplete = true;
     }
   }
}
