#include <SoftwareSerial.h>
#include <VSync.h>

SoftwareSerial HC12(4,5); // r,t
ValueReceiver<2> receiver;

int mouseX, mouseY;

void setup() {
  receiver.observe(mouseX);
  receiver.observe(mouseY);
  
  // put your setup code here, to run once:
  Serial.begin(115200);
  HC12.begin(115200);
}

void loop() {
  receiver.sync();
  // put your main code here, to run repeatedly:

  while(HC12.available()){
    Serial.write(HC12.read());
  }

    while(Serial.available()){
      HC12.write(Serial.read());
    }
  
//  HC12.write(mouseX);
//  delay(100);

}
