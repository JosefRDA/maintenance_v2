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

int cpt;
bool serialActive;

const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data

boolean newData = false;
boolean baneerHasBeenShown = false;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  cpt = 0;
  serialConnexionLoopSetup();
}

void serialConnexionLoopSetup() {
  Serial.begin(9600);
  serialActive = false;
}


// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000); // wait for a second
  cpt ++;
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
