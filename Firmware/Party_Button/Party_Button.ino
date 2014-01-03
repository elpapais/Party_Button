/* 
 12-20-13
 SparkFun Electronics 2013
 Shawn Hymel
 
 This code is public domain but you buy me a beer if you use this 
 and we meet someday (Beerware license).
 
 Description:
 
 Blasts a horn for all to hear, then gets the party started.
 
 Hardware Connections:
 
 Connect the MP3 Shield to an Arduino, then connect the following:
 
 Uno/MP3 Pin    Peripheral
 Vin            Battery +
 5V             PWR on Mono Audio Amp
 A0             Big Button LED+
 A1             NO pin on Big Button
 A2             G on MOSFET
 Speaker -      IN- on Mono Audio Amp
 Speaker R      IN+ on Mono Audio Amp
 GND            Battery -
 GND            GND on Mono Audio Amp
 GND            COM on Big Button
 
 Also, make the following hardware connections:
 - Connect COM to LED- on Big Button
 - Connect a speaker +/- to the Mono Audio Amp OUT+/-
 - Connect a 10k pulldown resistor between G and S on the MOSFET
 - Connect one terminal of car horn to Battery +
 - Connect other terminal of car horn to D on MOSFET
 */

#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h> 
#include <SFEMP3Shield.h>

// Global variables
SdFat sd;
SFEMP3Shield MP3player;
const int spiCsPin = 10;
const int ledPin = A0;
const int buttonPin = A1;
const int hornPin = A2;
int ledState = 0;
int state = 0;  // 0 = waiting
                // 1 = button down 
                // 2 = playing song
                // 3 = button down after stopping song

void setup() {
  
  // Result variable for SD initialization process
  int result = 1;

  // Initialize serial console for debugging
  Serial.begin(9600);
  Serial.println("Start the PARTY!");
      
  // Configure input and output pins
  pinMode(spiCsPin, OUTPUT); // Needed to start SPI
  pinMode(ledPin, OUTPUT);
  pinMode(hornPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  // Try until SD card is initialized
  while(result) {
    delay(500);
    if(sd.begin(SD_SEL, SPI_HALF_SPEED)) {
      if(sd.chdir("/")) {
        result = 0;
      }
    }
  }
  
  // Initialize MP3 player shield
  MP3player.begin();
  
  // Set volume at max and enable mono mode
  MP3player.setMonoMode(1);
  MP3player.setVolume(0, 0); // Backwards! Min=255; Max=0
  
}

void loop() {
  
  // Print current state to console
  Serial.println(state);
  
  // Determine the state and play song accordingly
  switch (state) {
    
    // Turn on LED and wait for button press
    case 0:
      digitalWrite(ledPin, HIGH);
      if ( digitalRead(buttonPin) == LOW ) {
        state = 1;
      }
      break;
      
    // Turn off LED and wait for button release. Play song.
    case 1:
      digitalWrite(ledPin, LOW);
      if ( digitalRead(buttonPin) == HIGH ) {
        digitalWrite(hornPin, HIGH);
        MP3player.playTrack(1);
        digitalWrite(hornPin, LOW);
        state = 2;
     }
      break;
      
    // Playin song and flash LED. Stop song on button press.
    case 2:
      digitalWrite(ledPin, ledState);
      if ( digitalRead(buttonPin) == LOW ) {
        MP3player.stopTrack();
        state = 3;
      }
      ledState = ~ledState;
      delay(100);
      break;
  
  // Button down after stopping song. Wait for button release.
  case 3:
    digitalWrite(ledPin, LOW);
    if ( digitalRead(buttonPin) == HIGH ) {
        state = 0;
      }
    break;
  }
  
}
