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


#define FADES_DELAY 2
#define FADES_TIMES 1
#define SPEED_BLINK_DELAY 100
#define SPEED_BLINK_SHORT_TIMES 2
#define SPEED_BLINK_LONG_TIMES 20

#define ANALYSIS_DELAY_MIN 10
#define ANALYSIS_DELAY_MAX 60

// The WS2812B RGB Shield pin
#define LED_PIN           D5
#define BUZZ_PIN          D6

const String SERVICE_TYPE = "2";
const String SERVICE_NAME = "AIR CIRCUIT";
char SERVICE_MAINTENANCE_PASSWORD[] = "8xfxa52k";

/* 
 1 = ELECTRICITY CIRCUIT - yqj6b6k9
 2 = AIR CIRCUIT - 8xfxa52k
 3 = WATER CIRCUIT - 4mvj3yxr
 4 = SOLAR ENERGY - 6swurr4n
 5 = BIOMASS ENERGY - 8pz3j2s8
 6 = GEOTYHERMAL ENERGY - bn6h52r2
 7 = SECURE SYSTEM : ARCHOTYPE - 42hm56e6
 8 = DEFENSE SYSTEM : NANO DETECTION - gj9nzjat
 9 = DEFENSE SYSTEM : KALASH - 5n8cjr3m
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
String outageResolutionCode = "";
bool FirstLoop = true;

unsigned long currentMillis = 0;    // stores the value of millis() in each iteration of loop()
unsigned long previousStatusCheck_Millis = 0; // will store last time the Console status was checked

int menuStep = 0;

HTTPClient http; //Declare an object of class HTTPClient   


// the setup function runs once when you press reset or power the board
void setup() {
  neoPixelSetup();
  buzzerSetup();
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

void buzzerSetup() {
  pinMode(BUZZ_PIN, OUTPUT); // Set buzzer - pin 9 as an output
}

// the loop function runs over and over again forever
void loop() {
  currentMillis = millis();   // capture the latest value of millis()

  StatusCheck();    // Connect the first time then check status every x millis

  ChangeLedStatus(ServiceStatus);
  serialConnexionLoop();
  
  delay(MAIN_FREQUENCY); 
  FirstLoop = false; 
}

void ChangeLedStatus(String ServiceStatus) {
  if (ServiceStatus != "") {
    //Traite le message reçu et change le statut des leds.
    switch (ServiceStatus[0] - '0') { // conversion en INT
      case 1:
        //Problème mineur
        // 
        // PCF_01.write(dLED1, HIGH); // WITH PCF8574 invert led level
        //digitalWrite(LED3, LOW);          
        tone(BUZZ_PIN, 3000); // Send 1KHz sound signal...
        //PLUSE YELLOW // PulseLed(LED2,30);
        fade(FADES_DELAY, FADES_TIMES, 255, 255, 0);
        noTone(BUZZ_PIN);     // Stop sound...
        fade(FADES_DELAY, FADES_TIMES, 255, 255, 0);
        tone(BUZZ_PIN, 3000); // Send 1KHz sound signal...
        //PLUSE YELLOW // PulseLed(LED2,30);
        fade(FADES_DELAY, FADES_TIMES, 255, 255, 0);
        noTone(BUZZ_PIN);     // Stop sound...
        fade(FADES_DELAY, FADES_TIMES, 255, 255, 0);
        break;
      case 2:
        //Problème majeur
        //digitalWrite(LED2, LOW);
        //digitalWrite(LED3, LOW);          
        tone(BUZZ_PIN, 2000); // Send sound signal...
        //BlinkLed(dLED1,30,1, false);
        BlinkLed(pixels.Color(255, 0, 0),SPEED_BLINK_DELAY,SPEED_BLINK_LONG_TIMES);
        break;
      default:
        // Tout va bien
        noTone(BUZZ_PIN);     // Stop sound...
        //VERT CONSTANT // digitalWrite(LED3, HIGH);   
        pixels.setPixelColor(0, pixels.Color(0, 255, 0));
        pixels.show();   
        //PCF_01.write(dLED1, HIGH); // WITH PCF8574 invert led level
        //digitalWrite(LED2, LOW);
        break;   
      } 
  }
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
  http.begin("http://www.clones.be/console/_backend/iot/Get_service_type_status.asp?ServiceType="+SERVICE_TYPE);    //Specify request destination
  // TODO : Set up the  SERVICE-TYPE by some physical switches on the board
  int httpCode = http.GET();    //Send the request
  BlinkLed(pixels.Color(0, 0, 255),SPEED_BLINK_DELAY,SPEED_BLINK_SHORT_TIMES); //BLUE  // Delay of 900 millis to give the time to receive the data
  
  if (httpCode > 0) {   //Check the returning code
    pixels.setPixelColor(0, pixels.Color(0, 0, 255));
    pixels.show();//BLUE // We have the status  
    ServiceStatus = http.getString();   //Get the request response ServiceStatus (just an integer)
    http.end();   //Close connection
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

void fade(uint8_t wait, uint8_t times, uint8_t pRed, uint8_t pGreen, uint8_t pBlue) {
  for(uint8_t cpt = 0; cpt < times; cpt++) {
    for(int j = 0; j < 256 ; j++){
          pixels.setPixelColor(0, pixels.Color(pRed * j / 255, pGreen  * j / 255, pBlue  * j / 255) );
          delay(wait);
          pixels.show();
        }
    
    for(int j = 255; j >= 0 ; j--){
          pixels.setPixelColor(0, pixels.Color(pRed * j / 255, pGreen * j / 255, pBlue * j / 255) );
          delay(wait);
          pixels.show();
        }
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
      switch (menuStep) {
          case 1 :
            Serial.println("********");
            if(strcmp(receivedChars,SERVICE_MAINTENANCE_PASSWORD)==0) {
              Serial.println("ACCES AUTORISE");
              delay(1000);
              if (ServiceStatus != "") {
              switch (ServiceStatus[0] - '0') { // conversion en INT
                case 1:
                  //Problème mineur
                  Serial.println("APPUYER SUR ENTRER POUR OBTENIR LE CODE DE RESOLUTION DE L'INCIDENT EN COURS");
                  menuStep = 3;
                  break;
                case 2:
                  //Problème majeur
                  Serial.println("APPUYER SUR ENTRER POUR OBTENIR LE CODE DE RESOLUTION DE L'INCIDENT CRITIQUE EN COURS");
                  menuStep = 3;
                  break;
                default:
                  // Tout va bien
                  Serial.println("AUCUNE ACTION DE MAINTENANCE A EFFECTUER");
                  Serial.println("DECONNEXION AU SYSTEME ...");
                  menuStep = 0;
                  break;   
                } 
              }
            } else {
              Serial.println("MOT DE PASSE INCORRECT");
              Serial.print(">");
            }
            break; 
          case 2 :
              
            break; 
          case 3 :
            menuStepThree();
           break; 
          default:
            Serial.println("DNG-72 > SIGMA-7 > SYSTEMES DE MAINTENANCE > " + SERVICE_NAME);
            
            if (ServiceStatus != "") {
              switch (ServiceStatus[0] - '0') { // conversion en INT
                case 1:
                  //Problème mineur
                  Serial.println("STATUS : [INCIDENT EN COURS]");
                  break;
                case 2:
                  //Problème majeur
                  Serial.println("STATUS : [INCIDENT CRITIQUE EN COURS]");
                  break;
                default:
                  // Tout va bien
                  Serial.println("STATUS : [100% OPERATIONEL]");
                  break;   
                } 
            }
            Serial.println("VEUILLEZ ENTRER LE MOT DE PASSE DE MAINTENANCE DU MODULE :");
            Serial.print(">");
            menuStep = 1;
            break;
        }
        newData = false;
    }
}

void menuStepThree() {
  long randNumber = random(ANALYSIS_DELAY_MIN, ANALYSIS_DELAY_MAX);

  for (long cpt = 0 ; cpt < randNumber; cpt++) {
    long percent = cpt * 100 / randNumber;
    Serial.println("ANALYSE EN COURS : " + String(percent) + "% ...");
    delay(1000);
  }
  Serial.println("ANALYSE TERMINEE");
  
  
  http.begin("http://www.clones.be/console/_backend/iot/Get_service_type_error.asp?ServiceType="+SERVICE_TYPE);    //Specify request destination
  // TODO : Set up the  SERVICE-TYPE by some physical switches on the board
  int httpCode = http.GET();    //Send the request
  
  if (httpCode > 0) {   //Check the returning code
    outageResolutionCode = http.getString();   //Get the request response ServiceStatus (just an integer)
    //outageResolutionCode = "0";
    http.end();   //Close connection

    Serial.println("CODE DE RESOLUTION : " + outageResolutionCode);
    
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
}
