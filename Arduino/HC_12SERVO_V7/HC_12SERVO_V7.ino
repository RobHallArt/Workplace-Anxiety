#include <Servo.h>              // import servo library
#include <SoftwareSerial.h>     // import serial library for HC-12

String readString;

int machineID = 1;              // unique ID of each machine

int machineX = 30;              // unique position of machine in 180x180 grid
int machineY = -20;

int timeLastMessage = 0;        // set to the millis(); value every time a message is recieved to keep track of time since last message

int recievedX = 0;              // position of user received from serial
int recievedY = 0;

int toneLevel = 3000;           // base value for frequency used for 'speech'

int r2count = 0;                // counter to keep track of the iterations of the r2D2seq speech function
int servoVal = 1;               // value read from servo to keep head motions less than 50 degrees for any one movement
int calcVal = 0;                // value calculated from recievedX and recievedY using atan2()


int speakerPin = 2;             // set pins for all devices attached to arduino.
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

SoftwareSerial HC12(HC12TX,HC12RX);   // create instance of SoftwareSerial for the HC-12 chip
Servo myservod;                       // create instance of Servo for our single servo.

void setup() {

        HC12.begin(115200);                 // start serial communication with the HC-12 board at a baud rate (data rate) of 115200 bits per second
        Serial.begin(115200);               // start serial communication any connected PC at a baud rate (data rate) of 115200 bits per second
        Serial.println("RECIEVER GO");      // send startup complete message to connected PC

        randomSeed(analogRead(0));          // read value from a disconnected pin to seed randomness and prevent machines using the same random values

        // initialize all the readings to 0:
        for (int thisReading = 0; thisReading < numReadings; thisReading++) {
                readings[thisReading] = 0;
        }

        pinMode(speakerPin, OUTPUT);        // set pinModes for all non-data pins
        pinMode(rPin,OUTPUT);
        pinMode(gPin,OUTPUT);
        pinMode(bPin,OUTPUT);

        myservod.attach(servoPin);          // connect instance of Servo to correct pin for servo

        setColour(0,255,0);                 // set LEDs to green to indicate successful startup
        myservod.write(90);                 // move servo to central position to calibrate
        delay(1000);                        // wait one second
        setColour(0,0,0);                   // turn LEDs Off
        delay(1000);                        // wait one second before beginning main program
}

void loop() {
        if (HC12.available()) {                                 // when HC-12 is recieving
                setColour(255,0,0);                             // set LEDs to red
                timeLastMessage = millis();                     // set current tome to timeLastMessage
                char c = HC12.read();                           // gets one byte from serial buffer

                if (c == ',') {                                 // if most recently revieved byte is ','
                        if (readString.length() == 3) {         // check if message is 3 bytes long for error detection

                                int n = readString.toInt();     // convert message into a number
                                Serial.print("Y");              // send value to connected PC (for Debugging)
                                Serial.println(readString);
                                recievedY = n;                  // set value of n to recievedY as it will be reset later

                        }

                        readString="";                          // clears variable for new input

                } else if (c == '-') {                          // if most recently revieved byte is '-'
                        if (readString.length() == 3) {         // check if message is 3 bytes long for error detection

                                int n = readString.toInt();     // convert readString into a number
                                Serial.print("X");              // send value to connected PC (for Debugging)
                                Serial.println(readString);
                                recievedX = n;                  // set value of n to recievedX as it will be reset later

                        }

                        readString="";                          // clears variable for new input

                } else if (c == '.') {                          // if most recent byte is '.' this indicates message conclusion and triggers the calculation and servo code

                        setColour(255,0,0);                     // set LEDs to red

                        calcVal = abs(180-atan2(recievedY - machineY, recievedX - machineX) * 57296 / 1000);
                        // arc tangent is used to calculate the direction the servo is pointing
                        // it is multiplied by 57296 / 1000 to convert from radians to degrees
                        // the arduino is slow at calculating with decimals so we try to keep everything in integers


                        total = total - readings[readIndex];    // remove the oldest servo position from the total
                        readings[readIndex] = calcVal;          // add the most recent calculated servo position to the array
                        total = total + readings[readIndex];    // add the most revent calculated servo position to the total
                        readIndex = readIndex + 1;              // advance to the next position in the array

                        if (readIndex >= numReadings) {         // if end of array is reached
                                readIndex = 0;                  // return to start of array
                        }

                        average = total / numReadings;          // calculate average
                        myservod.write(average);                // send average value to servo
                        Serial.print("ANGLE");                  // send angle to connected PC (for Debugging)
                        Serial.print(average);
                        Serial.println();

                } else {                              // if the most recent byte isn't a control character, continue reading
                        readString += c;              // adds most revent byte to the string of recieved bytes
                }
        }

        if(abs(timeLastMessage-millis())>1000) {      // if it has been more than a second since the last message run the 'speech' function
                r2D2seq();                            // run the speech function
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

        for (x=0; x<loopTime; x++) {

                digitalWrite(speakerPin,HIGH);
                delayMicroseconds(microsecondsPerWave);
                digitalWrite(speakerPin,LOW);
                delayMicroseconds(microsecondsPerWave);

        }
}

void r2D2seq(){                                      // 'speech' synthesis function

        // this function uses randomness inserted at different levels at varying frequency to create speech in as natural a way as possible
        // the pitch of the 'voice' changes every 20 'words', with the chance of a pause every time the function is run
        // longer pauses are less likely and therefore less frequent

        if(r2count%20 == 0) {                        // if the counter of times the function has been run is divisible by 20, set a new pitch for the voice

                toneLevel = random(0,500);           // set the base pitch to a value between 0 and 500 (Hz)

        }

        if(random(0,100)<60) {                       // pick a random value from 0 to 100 and check if it is less than 60

                setColour(random(0,255),random(0,255),random(0,255));                   // set LEDs to a random colour.
                servoVal = myservod.read()+random(-50,+50);                             // read from servo and find a value within 50 degrees of that position
                myservod.write(constrain(servoVal,5,175));                              // constrain value to the servo boundaries and write it to the servo
                beep(speakerPin,random(toneLevel+20,toneLevel+300),random(20,200));     // use base pitch to calculate a more granular pitch and play for a random period up to 0.2 seconds

        } else if(random(0,100)<10) {                // pick a random value and check if it is less than 10

                setColour(20,50,20);                 // set LEDs to a darker greenish hue
                delay(4000);                         // wait 4 seconds to simulate listening to another machine

        } else {                                     // if none of the above is true

                setColour(20,50,20);                 // set LEDs to a dark greenish hue
                delay(random(20,400));               // wait for a shorter period between 0.02 and 0.4 of a second

        }

        r2count = r2count + 1;                       // increment fucntion counter
}

void setColour(int _red, int _green, int _blue){     // simple function to streamline setting LED colour

        analogWrite(rPin,_red);                      // set each pin based on passed values
        analogWrite(gPin,_green);
        analogWrite(bPin,_blue);

}
