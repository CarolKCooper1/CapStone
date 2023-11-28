/* 
 * Project Capstone
 * Author: CKCooper
 * Date: 11/28/2023
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include <neopixel.h>
#include "Colors.h"
#include <Adafruit_MQTT.h>
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
#include "Adafruit_MQTT/Adafruit_MQTT.h"
#include "IoTTimer.h"
#include "Button.h"
#include "credentials.h"


TCPClient TheClient; 
IoTTimer lightTimer;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details. 
Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY); 

/****************************** Feeds ***************************************/ 
// Setup Feeds to publish or subscribe 
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname> 
Adafruit_MQTT_Subscribe buttonFeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/buttononoff"); 
// Let Device OS manage the connection to the Particle Cloud

int start;
int end;
int i;
const int PIXELCOUNT = 10;
const int LEDDELAY = 5000;
unsigned int last, lastTime;
int subValue,pubValue;

const int BUTTON=D3;// button
bool buttonState;
Button myButton(BUTTON);

Adafruit_NeoPixel pixel(PIXELCOUNT, SPI1, WS2812B);

void randomPixel();
void MQTT_connect();//this is important for next project
bool MQTT_ping();


SYSTEM_MODE(SEMI_AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

// setup() runs once, when the device is first turned on
void setup() {
  Serial.begin(9600);
  waitFor(Serial.isConnected,10000);

  pixel.begin();
  pixel.setBrightness(255);
  pixel.show();

  WiFi.on();
  WiFi.connect();
  while(WiFi.connecting()) {
    Serial.printf(".");
  }
  Serial.printf("\n\n");

  mqtt.subscribe(&buttonFeed);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  MQTT_connect();
  MQTT_ping();

if(myButton.isClicked()){//button toggle on off
   buttonState=!buttonState;
}
if(buttonState){
    for(i=0; i<5; i++){
      randomPixel();
      delay(LEDDELAY); 
      Serial.printf("BUTTON on %i\n", BUTTON);
    }
      pixel.clear();
      pixel.show();
}
else{
    pixel.clear();
    pixel.show();
}

Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(10000))) {
    if (subscription == &buttonFeed) {
      subValue = atoi((char *)buttonFeed.lastread);
      Serial.printf("Button Subscription %i \n", subValue);
    }

  if(subValue==1){
      for(i=0; i<5; i++){
        randomPixel();
        delay(LEDDELAY); 
      }
      pixel.clear();
      pixel.show();

  }
  else{
    pixel.clear();
    pixel.show();
  }
}

}
// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;
 
  // Return if already connected.
  if (mqtt.connected()) {
    return;
  }
 
  Serial.print("Connecting to MQTT... ");
 
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.printf("Error Code %s\n",mqtt.connectErrorString(ret));
       Serial.printf("Retrying MQTT connection in 5 seconds...\n");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds and try again
  }
  Serial.printf("MQTT Connected!\n");
}

bool MQTT_ping() {
  static unsigned int last;
  bool pingStatus;

  if ((millis()-last)>120000) {//this lets adafruit know we're still here
      Serial.printf("Pinging MQTT \n");
      pingStatus = mqtt.ping();
      if(!pingStatus) {
        Serial.printf("Disconnecting \n");
        mqtt.disconnect();
      }
      last = millis();
  }
  return pingStatus;
}
void randomPixel(){
  int whichPix;
  int randColor;
    whichPix=random(10);
    randColor=random(7);
    pixel.setPixelColor(whichPix, rainbow[randColor]);
    pixel.show();
    pixel.clear();
  } 
