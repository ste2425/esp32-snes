# Hello World Example

Starts a FreeRTOS task to print "Hello World".

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## How to use example

Follow detailed instructions provided specifically for this example. 

Select the instructions depending on Espressif chip installed on your development board:

- [ESP32 Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/stable/get-started/index.html)
- [ESP32-S2 Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/get-started/index.html)


## Example folder contents

The project **hello_world** contains one source file in C language [hello_world_main.c](main/hello_world_main.c). The file is located in folder [main](main).

ESP-IDF projects are built using CMake. The project build configuration is contained in `CMakeLists.txt` files that provide set of directives and instructions describing the project's source files and targets (executable, library, or both). 

Below is short explanation of remaining files in the project folder.

```
├── CMakeLists.txt
├── example_test.py            Python script used for automated example testing
├── main
│   ├── CMakeLists.txt
│   ├── component.mk           Component make file
│   └── hello_world_main.c
├── Makefile                   Makefile used by legacy GNU Make
└── README.md                  This is the file you are currently reading
```

For more information on structure and contents of ESP-IDF projects, please refer to Section [Build System](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html) of the ESP-IDF Programming Guide.

## Troubleshooting

* Program upload failure

    * Hardware connection is not correct: run `idf.py -p PORT monitor`, and reboot your board to see if there are any output logs.
    * The baud rate for downloading is too high: lower your baud rate in the `menuconfig` menu, and try again.

## Technical support and feedback

Please use the following feedback channels:

* For technical queries, go to the [esp32.com](https://esp32.com/) forum
* For a feature request or bug report, create a [GitHub issue](https://github.com/espressif/esp-idf/issues)

We will get back to you as soon as possible.

WORKING
-------------------------

//
// Created by benjamin and timo on 16.11.19.
//

//
// This program accepts button configurations send over serial port. If connected
// to a snes, it will simulate a snes controller and transmit the button configuration
// that was recieved over serial.
//

#include "Arduino.h"

#define clockpin 4
#define latchpin 17
#define datapin 2


/*!
   Interrupt Routines
*/
bool dataarr[16] = {true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true}; // This holds the current button configuration, 1 means the button is *not* pressed.
byte datafrompi[2]; // this seems to be legacy code
int bitcounter = 0; // a counter variable to handle what button setting to be 
                    // send next.

int letchCounter = 0; // legacy?

// The transmission of the current button configuration will be started after a
// latch signal was detected, according to the timing protocol the first button
// setting will be transmitted here.
void latchdetected() {
  bitcounter = 0;
  digitalWrite(datapin, dataarr[bitcounter]);
}

// On every rising clock signal the next button setting will be transmitted.
// 16 Button setting will be transmitted in total, even though the last four
// are not used by the snes.
void sendrestofdata() {
  bitcounter++; // Increase buttencounter to send the next signal.

  digitalWrite(datapin, dataarr[bitcounter]);

  // after sendig all 16 button settings the counter is reset and the data
  // signal is reset to low.
  if (bitcounter > 15) {
    bitcounter = 0;
    //delayMicroseconds(5);
    digitalWrite(datapin, LOW);
  }
}

// Converts an ascii character recieved over serial to the corresponding int value.
int convertASCIIToInt(int charInt) {
  return charInt - 48;
}

/*!
   Setup
*/
void setup() {
    Serial.println("Setup");
  /*!
     Setting Pinmodes
  */
  pinMode(clockpin, INPUT); // We need to read the snes's clock signal.
  pinMode(latchpin, INPUT); // We also need the read its latch signal.
  pinMode(datapin, OUTPUT); // The data pin is used to transmit the button configuration.

  /*!
     Attaching Interups to clock- and latchpin
  */
  //Time to start the Transmission.
  attachInterrupt(digitalPinToInterrupt(latchpin), latchdetected, RISING);
  //Detecting clocks for the rest of the databits.
  attachInterrupt(digitalPinToInterrupt(clockpin), sendrestofdata, RISING);

  /*!
     Initializing Serial Interface
  */
  Serial.begin(115200);
}

void loop() {
  // The following comment is legacy code and could be removed.
  /*!
     Receiving data from raspberry pi
  */
  /*if (Serial.available() > 0) {
    for (int i = 0; i < 2; i++) {
      int charInt = Serial.read();
      if (!isDigit(charInt)) {
        datafrompi[i] = 0b00000000;
      } else {
        datafrompi[i] = pow(2, convertASCIIToInt(charInt));
      }
    }
    Serial.flush();
    }*/

  /*!
     Decoding data from raspberry pi
    //
    for (int i = 0; i < 2; i++) {
    for (int j = 7; j >= 0; j--) {
      dataarr[j + i * 8] = !bitRead(datafrompi[i], j);
    }
    }*/

  // For the current button configuration we expect a string of 16 characters
  // that tells us which buttons are pressed.
  // Example: "1000000000000000" means that 'b' is pressed.
  // We only expect to recieve valid input over serial, that is a 16 character
  // string consisting of 1 and 0.
  if (Serial.available() > 0) {
    Serial.println("avail");
    for ( int i = 0; i < 16; i++) {
      int charInt = Serial.read();
      int in = convertASCIIToInt(charInt);

      if (in == 1) {
        Serial.println("lol");
        dataarr[i] = false;
      } else {
        dataarr[i] = true;
      }
    }
    Serial.flush(); // we do not want to read more than 16 characters.
                    // Especially \n and such are omitted.
  }
}