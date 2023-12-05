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
IoTTimer pixelTimer;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details. 
Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY); 

/****************************** Feeds ***************************************/ 
// Setup Feeds to publish or subscribe 
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname> 
Adafruit_MQTT_Subscribe buttonFeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/buttononoff"); 
Adafruit_MQTT_Subscribe scentFeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/scentbutton"); 
// Let Device OS manage the connection to the Particle Cloud

// int start;
// int end;
// int i;
// int x;
// const int PIXELCOUNT = 10;
// const int LEDDELAY = 5000;
int subValue,subValue1;
// const int BUTTON=D6;// button
// const int BUTTON1=D5;
bool buttonState;
const int SERVPIN = D13;
// int inputValue, inputValue1;
const int COUNT=5;
int counter;
// Button myButton(BUTTON);
// Button myButton1(BUTTON1);
Servo myServo;
IoTTimer scentTimer;
IoTTimer scentTimer1, scentTimer2;
Button startButton(D5);

Adafruit_NeoPixel pixel(10, SPI1, WS2812B);

int randomPixel();
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
//start pixels
  pixel.begin();
  pixel.setBrightness(255);
  pixel.show();

  pixelTimer.startTimer(1000);
  counter=COUNT;

  WiFi.on();
  WiFi.connect();
  while(WiFi.connecting()) {
    Serial.printf(".");
  }
  Serial.printf("\n\n");

  mqtt.subscribe(&buttonFeed);
  mqtt.subscribe(&scentFeed);
//servo
  myServo.attach(SERVPIN);
  // pinMode(SERVPIN, INPUT_PULLDOWN);
  //Buttons
  // pinMode(BUTTON,INPUT);
  // pinMode(BUTTON1,INPUT);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  MQTT_connect();
  MQTT_ping();

  if(startButton.isClicked()){
  Serial.printf("button %i \n",startButton);
    counter = 0;
    pixel.clear();
    pixel.show();
  }

  if(counter<COUNT){
    counter=counter+randomPixel();
  }

  // inputValue=digitalRead(startButton);
  // inputValue1=digitalRead(BUTTON1);

// if(startButton==1){//turn on lights
//     counter=0;
    // for(i=0; i<5; i++){
    //   randomPixel();
    //   Serial.printf("BUTTON on %i\n", BUTTON);
      // delay(LEDDELAY); //Brian approved the use of this delay
    // }
      // pixel.clear();
      // pixel.show();     
// }
// else{
//       pixel.clear();
//       pixel.show();
// }

// if (inputValue1==1){
//   Serial.printf("BUTTON #2 %i\n", BUTTON1);
//   scentTimer.startTimer(5000);
//   myServo.write(200);
// }
//   if (scentTimer.isTimerReady()) {
//   myServo.write(0);    
//   scentTimer1.startTimer(5000);
//   }
//   if (scentTimer1.isTimerReady()) {
//   myServo.write(100);
//   }  
  
  // for(x=0; x<1; x++){
  //   myServo.write(90);
  //   delay(1000);
  // }
    // myServo.write(90);
    // delay(1000);
    // myServo.write(180);
    // delay(1000);
    // myServo.write(0);
    // delay(1000);
    // myServo.write(180);
    // delay(1000);
    

Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(10000))) {
    if (subscription == &buttonFeed) {
      subValue = atoi((char *)buttonFeed.lastread);
      Serial.printf("Button Subscription %i \n", subValue);
    }
    if(subValue==1){
      counter=0;
      // for(i=0; i<5; i++){
      //   randomPixel();
        // delay(LEDDELAY); 
      pixel.clear();
      pixel.show();
      }

    // }
    // else{
    //   pixel.clear();
    //   pixel.show();
    }
    // if (subscription == &scentFeed) {
    //   subValue1 = atoi((char *)scentFeed.lastread);
    //   Serial.printf("scent Subscription %i \n", subValue1);
    // }
    // if(subValue1==2){
    //   scentTimer.startTimer(5000);
    //   myServo.write(150);
    // }
    // if (scentTimer.isTimerReady()) {
    //     scentTimer1.startTimer(5000);
    //     myServo.write(100); 
    // }
    // if (scentTimer1.isTimerReady()) {
    //     scentTimer2.startTimer(5000);
    //     myServo.write(125);
    // }  
    // if (scentTimer2.isTimerReady()) {
    //    myServo.write(0);    
    // }
      //  myServo.write(180);
      //  delay(5000);
      //  myServo.write(0);
      //  delay(5000);
      //  myServo.write(180);
      //  delay(5000);
      //  myServo1.write(15);
      //  delay(1000);
      //  myServo1.write(180);
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
int randomPixel(){
  int whichPix;
  int randColor;
  int value;

  value=0;
  if(pixelTimer.isTimerReady()){
      pixel.clear();
      whichPix=random(10);
      randColor=random(7);
      Serial.printf("Pixel %i and Color %i \n", whichPix, randColor);
      pixel.setPixelColor(whichPix, rainbow[randColor]);
      pixel.show();
      pixelTimer.startTimer(1000);
      // pixel.clear();
      // pixel.show();
      value=1;
  } 

  return value;
}