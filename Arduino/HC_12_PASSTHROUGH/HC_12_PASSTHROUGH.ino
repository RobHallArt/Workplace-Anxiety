#include <SoftwareSerial.h>                      // import SoftwareSerial library for HC-12

SoftwareSerial HC12(4,5); // RX,TX               // initialise SoftwareSerial instance with HC-12 pins

void setup() {

        Serial.begin(115200);                    // begin communication with PC at 115200 bits per second
        HC12.begin(115200);                      // begin communication with HC-12 115200 bits per second these must match to prevent sync issues
}

void loop() {

        while(HC12.available()) {                // while HC-12 is recieving
                Serial.write(HC12.read());       // send to PC via standard Serial
        }

        while(Serial.available()) {              // while recieving from connected PC
                HC12.write(Serial.read());       // send to HC-12 to send to machines
        }
}
