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
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>


// The WS2812B RGB Shield pin
#define LED_PIN           D5

const String SERVICE_TYPE = "1";

/* 
 1 = Electricity circuit
 2 = Air circuit
 3 = Water circuit
 4 = Solar Energy
 5 = Biomass Energy
 6 = Geotyhermal Energy
 7 = Secure system : Archotype
 8 = Defense system : Nano detection
 9 = Defense system : Kalash
*/

const int MAIN_FREQUENCY = 100; // in milliseconds
const int STATUSCHECK_FREQUENCY = 10000; // in milliseconds, every time we check on internet the status of the service
const char *Wifi_SSID = "VOO-354601";
const char *Wifi_PWD = "NJC235G6";

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

int StatusTime = 0;
String ServiceStatus = "";
bool FirstLoop = true;

unsigned long currentMillis = 0;    // stores the value of millis() in each iteration of loop()
unsigned long previousStatusCheck_Millis = 0; // will store last time the Console status was checked

HTTPClient http; //Declare an object of class HTTPClient   


// the setup function runs once when you press reset or power the board
void setup() {
  neoPixelSetup();
  wifiSetup();
  serialConnexionLoopSetup();
}

void serialConnexionLoopSetup() {
  Serial.begin(9600);
  serialActive = false;
}

void neoPixelSetup() {
  pixels.begin();
}

void wifiSetup() {
  WiFi.begin(Wifi_SSID, Wifi_PWD);
}


// the loop function runs over and over again forever
void loop() {
  currentMillis = millis();   // capture the latest value of millis()

  StatusCheck();    // Connect the first time then check status every x millis

  
  //serialConnexionLoop();
  
  delay(MAIN_FREQUENCY); 
  FirstLoop = false; 
}

void StatusCheck() {

  if (WiFi.status() != WL_CONNECTED) {
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500); 
    }
  }
  
  pixels.setPixelColor(0, pixels.Color(0, 0, 255));
  pixels.show(); // We're online and connected. Blue led lights
  if ((currentMillis - previousStatusCheck_Millis >= STATUSCHECK_FREQUENCY) || FirstLoop) { // We test the first time and every STATUSCHECK_FREQUENCY
    
    OnlineCheck();
    previousStatusCheck_Millis = currentMillis;
  }
}

void OnlineCheck() {
  Serial.println("DEBUG OnlineCheck");
  http.begin("http://www.clones.be/console/_backend/iot/Get_service_type_status.asp?ServiceType="+SERVICE_TYPE);    //Specify request destination
  // TODO : Set up the  SERVICE-TYPE by some physical switches on the board
  int httpCode = http.GET();    //Send the request
  BlinkLed(pixels.Color(0, 0, 255),100,10); //BLUE  // Delay of 900 millis to give the time to receive the data
    
  if (httpCode > 0) {   //Check the returning code
    Serial.println("DEBUG httpCode :" + String(httpCode));
    pixels.setPixelColor(0, pixels.Color(0, 0, 255));
    pixels.show();//BLUE // We have the status  
    ServiceStatus = http.getString();   //Get the request response ServiceStatus (just an integer)
    Serial.println("Status : " + ServiceStatus);    //Print the response payload
    
    http.end();   //Close connection
    Serial.println("Connection closed.");    
  }

}

void BlinkLed (uint32_t color, int Blinkspeed, int BlinkTimes)  {
  for (int BlinkValue = 1 ; BlinkValue <= BlinkTimes; BlinkValue += 1) {
    pixels.setPixelColor(0, color);
    pixels.show();
    delay(Blinkspeed/2);
    pixels.setPixelColor(0, pixels.Color(0, 0, 0));
    pixels.show();
    delay(Blinkspeed/2);
  }
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
