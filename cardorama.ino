#include <Adafruit_TCS34725.h>
#include <Bounce2.h> 
#include <Adafruit_MotorShield.h>
#include <FastLED.h>
#include <SPI.h>
#include <SdFat.h>
#include <SFEMP3Shield.h>

#define CAR_STATE 1
#define TURTLE_STATE 2
#define NEUTRAL_STATE 0

#define NUM_LEDS 4
#define LED_PIN 5

unsigned long currentTime = 0;
unsigned long prevTime = 0;
unsigned long time_interval = 500;

int tracknumber = 0;

SdFat sd;
SFEMP3Shield MP3player;

#define BUTTON_DEBOUNCE_PERIOD 20
#define button A0
Bounce buttonPress  = Bounce();


uint16_t r, g, b, c, colorTemp;
float red, green, blue;
byte gammatable[256];

int8_t counter = 0;
int8_t prevCounter = 0;
bool counterChanged = false;

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
CRGB leds[NUM_LEDS];
 
Adafruit_DCMotor *left_motor = AFMS.getMotor(1); // create motor #2, 64KHz pwm
Adafruit_DCMotor *right_motor = AFMS.getMotor(2);

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_1X);


/*--------------------------SETUP------------------------------*/
void setup() {
  Serial.begin(115200);           
  Serial.println("Motor test!");

  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
//FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);



  pinMode(button,INPUT_PULLUP);
//  
  buttonPress.attach(button);
  buttonPress.interval(BUTTON_DEBOUNCE_PERIOD);
  
  AFMS.begin();


  if (tcs.begin()) {
    Serial.println("Found sensor"); }

  
  if(!sd.begin(9, SPI_HALF_SPEED)) sd.initErrorHalt();
  if (!sd.chdir("/")) sd.errorHalt("sd.chdir");

  MP3player.begin();
  MP3player.setVolume(10,10);
  
  Serial.println(F("Looking for Buttons to be depressed..."));
  
}


/*-----------------------------LOOP--------------------------*/
 
void loop() {
  
gammatableConversion();


  
  if(red > 90 && red < 180 && green < 50){
    Serial.print("entered");
    counter = 2; tracknumber = 2;
    }

    else if(green > 90 && blue < 70 && red < 100){
      Serial.print("entered");
      counter = 1; tracknumber = 0;
      }
      
      else { counter = NEUTRAL_STATE; tracknumber = 4; }

     Serial.print("\tcounter: ");
     Serial.print(counter);
     
      if(counter != prevCounter){    
        
          MP3player.stopTrack();
          delay(100);
          showLED();
          delay(1000); 
          prevCounter = counter;
          
            }
            
  runWheels();
  MP3player.playTrack(tracknumber);
  

if(counter == CAR_STATE){
  if (buttonPress.update()) { 
      MP3player.stopTrack();
      
    if (buttonPress.read() == HIGH)  {
      Serial.print("button pressed");
      MP3player.playTrack(1);
     }
    }
  }

if(counter == TURTLE_STATE){
  if (buttonPress.update()) { 
      MP3player.stopTrack();
      
    if (buttonPress.read() == HIGH)  {
      MP3player.playTrack(3);
     }
   }
}

  

  
}




/*-----------------------------RUN WHEELS--------------------------*/

void runWheels(){


  
   left_motor->run(BACKWARD);
  right_motor->run(BACKWARD);
  
  if(counter == CAR_STATE){
// turn it on going forward
  left_motor->setSpeed(50);
  right_motor->setSpeed(50);
  }

   else if(counter == TURTLE_STATE){
// turn it on going forward
  left_motor->setSpeed(20);
  right_motor->setSpeed(20);
  }

  else {
  left_motor->setSpeed(0);
  right_motor->setSpeed(0);
    }
 }


/*-----------------------------SHOW LED--------------------------*/
void showLED(){
FastLED.clear();
Serial.println("showing LED");
for(int i=0;i<NUM_LEDS;i++){
    leds[i] = CRGB(gammatable[(int)red],gammatable[(int)green], gammatable[(int)blue]);
    } 
 FastLED.show(); 
}


/*-----------------------------LED COLOR--------------------------*/
void gammatableConversion(){
  for (int i=0; i<256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;
    
      gammatable[i] = x;
    //Serial.println(gammatable[i]);
  }

//  tcs.setInterrupt(false);  // turn on LED
  tcs.getRawData(&r, &g, &b, &c);
  tcs.getRGB(&red,&green,&blue);
  colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
//
  Serial.print("R:\t"); Serial.print(int(red)); 
  Serial.print("\tG:\t"); Serial.print(int(green)); 
  Serial.print("\tB:\t"); Serial.print(int(blue));
  Serial.print("\tTemp:\t"); Serial.print(int(colorTemp));
}



  
