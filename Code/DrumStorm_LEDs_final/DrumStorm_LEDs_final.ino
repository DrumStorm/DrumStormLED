/*
*@Author Paul Lödige
*@Date 2019-06-03
*
*A script to control LEDs
*/
#include <FastLED.h>


#define LED_PIN                 2     //the pin the LEDs are connected to
#define NUM_LEDS                75    //number of LEDs
#define BTN_PIN                 0     //the pin the modus button is connected to
#define BTN_Bounce              50    //bounce time of the modus button in ms
#define mediumTime              1000  //medium button press time
#define longTime                2000  //long button press time

//other defines and stuff for the FastLED library
#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
#define LED_UPDATES_PER_SECOND  100   

static CRGBPalette16 currentPalette;
static TBlendType    currentBlending;

static unsigned long pressStartMillis=0;  //memory for the time the user starts pressing the button
static unsigned long pressTime=0;         //memory for the amount of time the button has been pressed

static bool inputFinished;   //bool to inform whether the user finished the input
static uint8_t startIndex;       //the start index for the curren palette    

extern CRGBPalette16 myRedWhitePalette;
extern const TProgmemPalette16 myRedWhitePalette_p PROGMEM;


//enums
enum sysState{
  off,
  Color,
  redAndWhite,
  white
};

enum btnState{
  noClick,
  shortClick,
  mediumClick,
  longClick  
};

//enum variables
static sysState systemState=off;
static btnState buttonState=noClick;


void setup() {
  pinMode(BTN_PIN, INPUT_PULLUP);
  delay( 1000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );  
}



void loop() {
  if(digitalRead(BTN_PIN)==HIGH){
      pressStartMillis=millis();
      changeMode();
      buttonState=noClick;
    }

  pressTime=millis()-pressStartMillis;
  if(pressTime>=BTN_Bounce){
      if(pressTime<mediumTime){
          buttonState=shortClick;
        }
       else if(pressTime<longTime){
          buttonState=mediumClick;
          fill_solid( currentPalette, 16, CRGB::Red);
          currentBlending = NOBLEND;
          FillLEDsFromPaletteColors( 0);
          FastLED.show();
          FastLED.delay(1000 / LED_UPDATES_PER_SECOND);
          
        }
       else{
          buttonState=longClick;
          fill_solid( currentPalette, 16, CRGB::Blue);
          currentBlending = NOBLEND;
          FillLEDsFromPaletteColors( 0);
          FastLED.show();
          FastLED.delay(1000 / LED_UPDATES_PER_SECOND);
        }
    }

  statemachine();
}


void changeMode(){
    switch(buttonState){
        case noClick:
          break;

        case shortClick:
          if(systemState==off){
              systemState=white;
            }
          else{
              systemState=off;
            }
          break;

        case mediumClick:
          systemState=redAndWhite; break;

        case longClick:
          systemState=Color; break;
      }
}

void statemachine(){
  switch(systemState){
      case white:
        LED_white();break;

      case off:
        LED_off();break;

      case redAndWhite:
        LED_redAndWhite();break;

      case Color:
        LED_Color();break;
    }  
}

/**
 * @brief turns all LEDs off
**/
void LED_off(){
    for(int i=0;i<NUM_LEDS;i++){
      leds[i]=CRGB::Black;
    }
  FastLED.show();
}

/**
 * @brief switches all LEDs to white
**/
void LED_white(){
  for(int i=0;i<NUM_LEDS;i++){
      leds[i]=CRGB::White;
  }
  FastLED.show();
}

/**
 * @brief switches the LEDs to the red and white sequence
**/
void LED_redAndWhite(){
  currentPalette = myRedWhitePalette_p;
  currentBlending = LINEARBLEND;
  static uint8_t startIndex = 0;
  startIndex = startIndex + 1; /* motion speed */
  FillLEDsFromPaletteColors( startIndex);
  FastLED.show();
  FastLED.delay(1000 / LED_UPDATES_PER_SECOND);
}

/**
 * @brief runs the first Color function
**/
void LED_Color(){
  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
  static uint8_t startIndex = 0;
  startIndex = startIndex + 1; /* motion speed */
  FillLEDsFromPaletteColors( startIndex);
  FastLED.show();
  FastLED.delay(1000 / LED_UPDATES_PER_SECOND);
}

const TProgmemPalette16 myRedWhitePalette_p PROGMEM =
{
  CRGB::Red,
  CRGB::Red,
  CRGB::Red,
  CRGB::Red,

  CRGB::White,
  CRGB::White,
  CRGB::White,
  CRGB::White,

  CRGB::Red,
  CRGB::Red,
  CRGB::Red,
  CRGB::Red,

  CRGB::White,
  CRGB::White,
  CRGB::White,
  CRGB::White,
};

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
  uint8_t brightness = 255;

  for ( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
    colorIndex += 3;
  }
}
