/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/

#include <Adafruit_NeoPixel.h>


// The WS2812B RGB Shield pin
#define LED_PIN           D5

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, LED_PIN, NEO_GRB + NEO_KHZ800);

byte neopix_gamma[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

int cpt;
bool serialActive;

const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data

boolean newData = false;
boolean baneerHasBeenShown = false;

// the setup function runs once when you press reset or power the board
void setup() {
  neoPixelSetup();
  serialConnexionLoopSetup();
}

void serialConnexionLoopSetup() {
  Serial.begin(9600);
  serialActive = false;
}

void neoPixelSetup() {
  pixels.begin();
}


// the loop function runs over and over again forever
void loop() {
  pixels.setPixelColor(0, pixels.Color(0, 0, 255)); 
  pixels.show();                      // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  pixels.setPixelColor(0, pixels.Color(0, 0, 0)); 
  pixels.show();                        // turn the LED off by making the voltage LOW
  delay(1000); // wait for a second
  serialConnexionLoop();
}

void serialConnexionLoop() {
  if (Serial.available()) { //Activate by keyboard
    if(serialActive) {
      recvWithEndMarker();
      showNewData();
    } else {
      //first frame of a new connexion
      if(!baneerHasBeenShown) {
        printBaneer();
        baneerHasBeenShown = true;
      }
      serialActive = true;
    }
  } else {
    serialActive = false;  
  }
}


void recvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = '\r';
    char rc;

    while (Serial.available() > 0 && newData == false) {
      
        rc = Serial.read();

        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        }
        else {
            receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
        }
        delay(100);
    }
}

void showNewData() {
    if (newData == true) {
        Serial.println("This just in ... ");
        Serial.println(receivedChars);
        newData = false;
    }
}

void printBaneer() {
  Serial.println("");
  Serial.println("████████ ███████  ██████ ██   ██ ███    ██  ██████   ██████  ██████  ██████  ██████  ");
  Serial.println("   ██    ██      ██      ██   ██ ████   ██ ██    ██ ██      ██    ██ ██   ██ ██   ██ ");
  Serial.println("   ██    █████   ██      ███████ ██ ██  ██ ██    ██ ██      ██    ██ ██████  ██████  ");
  Serial.println("   ██    ██      ██      ██   ██ ██  ██ ██ ██    ██ ██      ██    ██ ██   ██ ██      ");
  Serial.println("   ██    ███████  ██████ ██   ██ ██   ████  ██████   ██████  ██████  ██   ██ ██        ");
  Serial.println("                                                                 ");
  Serial.println("   ((((((((((((((((((                                       /(((((((((((((((((. ");
  Serial.println("     (((((   ((((((((((((                               ((((((((((((   .((((    ");
  Serial.println("                 (((((((((((((((.                (((((((((((((((*               ");
  Serial.println("                  (((((((((((((((((((((((((((((((((((((((((((((/                ");
  Serial.println("                 (((((((((((((((((((((((((((((((((((((((((((((((                ");
  Serial.println("                  ((((((((          ((((((((((         ((((((((                 ");
  Serial.println("                       ,/           (((((((((            /                      ");
  Serial.println("                                     ((((((((                                   ");
  Serial.println("");
  Serial.println("DNG-72 > SIGMA-7 > POWER SUPPLY ADMINISTRATION ");
  Serial.println("");
}
