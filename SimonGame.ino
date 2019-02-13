#include <Adafruit_NeoPixel.h>
//#include <sbrk.h> //For use with XMC1.

#define BRed        2         // Digital IO pin connected to the button.  This will be
#define BBlue       3         // driven with a pull-up resistor so the switch should
#define BGreen      4         // pull the pin to ground momentarily.  On a high -> low
#define BYellow     5         // transition the button press logic will execute.
                      
#define PIXEL_PIN   6         // Digital IO pin connected to the NeoPixels.
#define PIXEL_COUNT 36        // Number of leds.

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

int   s;
bool  stateR;
bool  stateB;
bool  stateG;
bool  stateY;

int       level = 5;        //Lenght of the secuence.
const int LEVEL_MAX = 50;   //Max lenght of the secuence.
int       list[LEVEL_MAX];  //Secuence.
bool      GameOver = false; 
int       cont = 0;         //Position in the secuence.
int       sp;               //Led speed show.

extern "C" void *_sbrk(int incr);
void dummy_sbrk_caller() __attribute__((__used__));
void dummy_sbrk_caller()
{
  _sbrk(0);
} 

void setup() {
  pinMode(BRed,     INPUT_PULLUP);
  pinMode(BBlue,    INPUT_PULLUP);
  pinMode(BGreen,   INPUT_PULLUP);
  pinMode(BYellow,  INPUT_PULLUP);
  strip.begin();
  strip.show();     // Initialize all pixels to 'off'

  Serial.begin(9600);
  randomSeed(analogRead(0));
}

void loop() {
  game();
}

void game(){
  GameOver = false;
  randList();
  delay(1000);
  
  while (GameOver == false){
    if (cont == level){
      startShow(5); //Win
      cont = 0;
      GameOver = true;
    }
    
    else {
      showRandList(cont);
      //state();
      for (int i = 0 ; i <= cont ; i++){
        state();
        if (s == list[i]){
          delay(100);
        }
        else{
          startShow(4); //GameOver
          cont = 0;
          GameOver = true;
        }
      }
      if (GameOver == false){
        cont ++;
        startShow(6); //Good
        delay(1000);
      }
    }
  }

  if (GameOver == true){
    state();
    level = (s+1)*10;
  }
 
}

void state(){
  s = -1;

  while (s == -1){
    stateR = digitalRead(BRed);
    stateB = digitalRead(BBlue);
    stateG = digitalRead(BGreen);
    stateY = digitalRead(BYellow);
    
    if (stateR == LOW) s = 0;
    else if (stateB == LOW) s = 1;
    else if (stateG == LOW) s = 2;
    else if (stateY == LOW) s = 3;

    if (s != -1) startShow(s);
  }
}

void randList(){
  for (int i = 0 ; i < LEVEL_MAX ; i++) list[i] = random(4);
}

void showRandList(int c){
  sp = 10;      //Led speed when show the secuence.
  for (int i = 0 ; i <= c ; i++) startShow(list[i]);
  delay (10);   
  sp = 5;       //Led speed when repeat the secuence.
}

void startShow(int state) {
  switch(state){
    case 0: colorWipe( strip.Color(55,  0,  0), sp); 
            colorWipeR(strip.Color( 0,  0,  0), sp);
            break;    //Red
    case 1: colorWipe (strip.Color( 0,  0, 55), sp);
            colorWipeR(strip.Color( 0,  0,  0), sp);
            break;    //Blue
    case 2: colorWipe (strip.Color( 0, 55,  0), sp);
            colorWipeR(strip.Color( 0,  0,  0), sp);
            break;    //Green
    case 3: colorWipe (strip.Color(55, 55,  0), sp);
            colorWipeR(strip.Color( 0,  0,  0), sp);
            break;    //Yellow
    case 4: colorWipe (strip.Color(55,  0,  0),  1); 
            delay (50);
            colorWipeR(strip.Color( 0,  0,  0),  1);
            colorWipe (strip.Color(55,  0,  0),  1); 
            delay (50);
            colorWipeR(strip.Color( 0,  0,  0),  1);
            colorWipe (strip.Color(55,  0,  0),  1); 
            delay (50);
            colorWipeR(strip.Color( 0,  0,  0),  1);
            break;    //Game Over      
    case 5: rainbowCycle(1);    
            colorWipeR(strip.Color( 0,  0,  0),  1);
            break;    //Win  
    case 6: colorWipe (strip.Color(50, 50, 50),  1); 
            delay (50);
            colorWipeR(strip.Color( 0,  0,  0),  1);
            colorWipe (strip.Color(50, 50, 50),  1); 
            delay (50);
            colorWipeR(strip.Color( 0,  0,  0),  1);
            break;    //Good
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
void colorWipeR(uint32_t c, uint8_t wait) {
  for(uint16_t i=strip.numPixels()+1; i>0; i--) {
    strip.setPixelColor(i-1, c);
    strip.show();
    delay(wait);
  }
}

void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
