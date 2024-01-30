#include "Arduino.h"

#define latchpin 17
#define clockpin 4
#define datapin 2
#define buttonpin 27

void setup() {
  pinMode(buttonpin, INPUT_PULLUP); // We need to read the snes's clock signal.
  pinMode(clockpin, INPUT); // We need to read the snes's clock signal.
  pinMode(latchpin, INPUT); // We also need the read its latch signal.
  pinMode(datapin, OUTPUT); // The data pin is used to transmit the button configuration.
}

bool pressed[16] = {
    true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true
};

bool latched = false;
bool clocked = false;
int count = 0;

void loop() {
    if (!latched) {
        latched = digitalRead(latchpin);

        if (latched) {
          //  printf("latched");
            count = 0;
            digitalWrite(datapin, pressed[count]);
        } else {
            bool btnpressed = digitalRead(buttonpin);

            pressed[5] = btnpressed;
        }
    } else {
        bool currentlyClocked = digitalRead(clockpin);
        if (currentlyClocked != clocked && currentlyClocked) {
            //printf("clocked");
            count++;

            digitalWrite(datapin, pressed[count]);

            if (count > 15) {
                latched = false;
                count = 0;
            }
        }
        clocked = currentlyClocked;
    }
}