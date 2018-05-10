#include <Servo.h>            // import servo library
#include <SoftwareSerial.h>   // import serial library for HC-12

String readString;

int machineID = 1;            // unique ID of each machine

int machineX = 30;            // unique position of machine in 180x180 grid
int machineY = -20;

int timeLastMessage = 0;      

int recievedX = 0;
int recievedY = 0;

int toneLevel = 3000;

int r2count = 0;
int servoVal = 1;
int calcVal = 0;

int randRed = 0;

int speakerPin = 2;
int rPin = 3;
int gPin = 5;
int bPin = 6;
int servoPin = 8;
int HC12TX = 9;
int HC12RX = 10;

const int numReadings = 10;     // how many recieved numbers are averaged

int readings[numReadings];      // recieved data
int readIndex = 0;              // index of the current data
int total = 0;                  // running total
int average = 0;                // average

SoftwareSerial HC12(HC12TX,HC12RX); // TX,RX
Servo myservod;

void setup() {

  HC12.begin(115200);
  Serial.begin(115200);
  Serial.println("RECIEVER GO");

  randomSeed(analogRead(0));

  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

  pinMode(speakerPin, OUTPUT);
  pinMode(rPin,OUTPUT);
  pinMode(gPin,OUTPUT);
  pinMode(bPin,OUTPUT);

  myservod.attach(servoPin);

  setColour(0,255,0);
  myservod.write(90);
  delay(1000);
  setColour(0,0,0);
  delay(1000);
}

void loop() {
  if (HC12.available()){ //When HC12 Available
    setColour(255,0,0);
    timeLastMessage = millis();
    char c = HC12.read();  //gets one byte from serial buffer
    //Serial.write(c);

    if (c == ','){
      if (readString.length() == 3){

        int n = readString.toInt();  //Convert readString into a number
        Serial.print("Y");
        Serial.println(readString);
        recievedY = n;

      }

      readString=""; //clears variable for new input

    } else if (c == '-'){
      if (readString.length() == 3){

        int n = readString.toInt();  //Convert readString into a number
        Serial.print("X");
        Serial.println(readString);
        recievedX = n;

      }

      readString=""; //clears variable for new input

    } else if (c == '.') {

      setColour(255,0,0);

      //Added check of servo position in an attempt to stop chugging servo.
      //This didn't work. check calcVal against previous calcVal maybe better solution. if within 5 don't update servo.
      //servoVal = myservod.read();
      calcVal = abs(180-atan2(recievedY - machineY, recievedX - machineX) * 57296 / 1000);


      total = total - readings[readIndex];
      // read from the sensor:
      readings[readIndex] = calcVal;
      // add the reading to the total:
      total = total + readings[readIndex];
      // advance to the next position in the array:
      readIndex = readIndex + 1;

      // if we're at the end of the array...
      if (readIndex >= numReadings) {
        // ...wrap around to the beginning:
        readIndex = 0;
      }

      // calculate the average:
      average = total / numReadings;
      //if(servoVal != calcVal){
      myservod.write(average);
      //}
      Serial.print("ANGLE");
      Serial.print(average);
      //Serial.println(recievedY,recievedX);
      Serial.println();

    } else {

      readString += c; //makes the string readString

    }
  }

  if(abs(timeLastMessage-millis())>1000){

    r2D2seq();

  }

}

void beep (int speakerPin, float noteFrequency, long noteDuration){

  //Beep function from https://github.com/IoTMakers/Arduino

  int x;
  // Convert the frequency to microseconds
  float microsecondsPerWave = 1000000/noteFrequency;
  // Calculate how many HIGH/LOW cycles there are per millisecond
  float millisecondsPerCycle = 1000/(microsecondsPerWave * 2);
  // Multiply noteDuration * number or cycles per millisecond
  float loopTime = noteDuration * millisecondsPerCycle;
  // Play the note for the calculated loopTime.

  for (x=0;x<loopTime;x++){

    digitalWrite(speakerPin,HIGH);
    delayMicroseconds(microsecondsPerWave);
    digitalWrite(speakerPin,LOW);
    delayMicroseconds(microsecondsPerWave);

  }
}

void r2D2seq(){

  if(r2count%20 == 0){

    toneLevel = random(0,500);

  }

  if(random(0,100)<60){

    setColour(random(0,255),random(0,255),random(0,255));
    servoVal = myservod.read()+random(-50,+50);
    myservod.write(constrain(servoVal,5,175));
    beep(speakerPin,random(toneLevel+20,toneLevel+300),random(20,200));

  } else if(random(0,100)<10) {

    setColour(20,50,20);
    delay(4000);

  } else {

    setColour(20,50,20);
    delay(random(20,400));

  }

  r2count = r2count + 1;
}

void setColour(int _red, int _green, int _blue){

  analogWrite(rPin,_red);
  analogWrite(gPin,_green);
  analogWrite(bPin,_blue);

}
