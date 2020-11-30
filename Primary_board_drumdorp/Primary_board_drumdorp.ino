// code created for Soundlab Enschede, component Drumdorp.
// by Miriam Riefel

// chunks of code taken and adjusted from Bare Conductive multi-board-touch-midi
// https://github.com/BareConductive/multi-board-touch-midi
// chunks of code taken and adjusted from Adafruit Neopixel Strandtest
// https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/strandtest/strandtest.ino

// this script is written for 2 Touch Boards in total (24 electrodes) 
// and 4 LED substrips with each 15 LEDs.
// but these settings can be easily adjusted. This is clarified in the script. 
// You can search for 'ADJUST'.

// compiler error handling
#include "Compiler_Errors.h"

// touch includes
#include <MPR121.h>
#include <Wire.h>
#define MPR121_ADDR 0x5C
#define MPR121_INT 4

#include <Keyboard.h>
#define holdKey false 

// ADJUST line 31
// the keys to which the electrodes map, starting E0 on the primary board. 
// this means that the electrodes will serve as surrogate keys in the laptop's keyboard.
// note that if you'd connect more than 1 secondary board, you should add keys and change the length of the array.

const char keyMap[24] = {'A', 'Z', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 'Q', 'S','D', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'W', 'X', 'C', 'V'};

// ADJUST line 34
const int numSecondaryBoards = 1; // number of secondary boards. Change this number if more than 2 touch boards are used.
const int totalNumElectrodes = (numSecondaryBoards+1)*12;

// serial comms definitions
const int serialPacketSize = 13;

// secondary board touch variables
bool thisExternalTouchStatus[numSecondaryBoards][12];
bool lastExternalTouchStatus[numSecondaryBoards][12];

// compound touch variables
bool touchStatusChanged = false;
bool isNewTouch[totalNumElectrodes];
bool isNewRelease[totalNumElectrodes];
int numTouches = 0;




// LED: the following bit of code was copy-pasted from the Strandtest script from Adafruit Neopixel.

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        6

// ADJUST line 64 (if you use more than 100 LEDs in total)
#define NUMPIXELS 100 // Popular NeoPixel ring size

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// end of copy-pasted code

// ADJUST line 74-78 (if you want to use different colours)
//here some default colours are defined that can be used for different sub-strips
//you can create any colour with (RED, GREEN, BLUE) format, with 0 <= value <= 255
uint32_t out = pixels.Color(  0,   0, 0);
uint32_t white = pixels.Color(  255,   255, 255);
uint32_t red = pixels.Color(  255,   20, 50);
uint32_t green = pixels.Color(  12,   255, 52);
uint32_t tule = pixels.Color(  0,   255, 255);
uint32_t blue = pixels.Color(  30,   60, 255);

// ADJUST line 82 (if you want to use different colours)
// map the LEDsub-strips to different colors
uint32_t colorscheme[] = {red, green, tule, blue};

// ADJUST line 92
// fill this list with the number of the first LED of each sub-strip. 
// make sure that the total length of the list is as long as the total number of electrodes. 
// the electrodes that do not have a corresponding LED strip will then turn on some phantom LEDs.
// In this case we had 4 electrodes which triggered LEDs, 
// so 4 sub-strips that all have 15 LEDs. 
// All the numbers after that are for activating LEDs that don't even exist.
 
int first_leds[] = {0, 15, 30, 45, 60, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88}; 

// ADJUST line 96 (if you use more than 2 touch boards, make sure the array of zeroes has the right number of electrodes.
// save on/off status per electrode. We don't care about releases here. A touch is always single. If the electrode was off before, it turns on, vice versa. 
int led_status[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


// little helper function for quickly printing integers with description, for debugging purposes
void int_printer(char varname[], int var) {
  Serial.print("\n");
  Serial.print(varname);
  Serial.print(": "); 
  Serial.print(var);
  Serial.print("\n");
}

// this is the function that activates LEDs. The TOUCHPLAAT is the electrode that is touched. 
void colorWipe(uint32_t color1, int TOUCHPLAAT) {

  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.
  for(int i=first_leds[TOUCHPLAAT]; i<first_leds[TOUCHPLAAT + 1]; i++) { 
    pixels.setPixelColor(i, color1);
    pixels.show();   // Send the updated pixel colors to the hardware.
  }
}

 



// setup() function -- runs once at startup --------------------------------


void setup() {
  pixels.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear();           // Set all pixel colors to 'off'
  pixels.show();            // Turn OFF all pixels ASAP
  
  // ADJUST line 131 (to change the brightness of the LEDs)
  pixels.setBrightness(150); // Set BRIGHTNESS to about 1/5 (max = 255)

  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  //pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)

  Serial.begin(57600);
  
  pinMode(LED_BUILTIN, OUTPUT);
   
  //while (!Serial) {}; //uncomment when using the serial monitor 
  Serial.println("Bare Conductive Multi Board Touch MP3 player");

  if(!MPR121.begin(MPR121_ADDR)) Serial.println("error setting up MPR121");
  MPR121.setInterruptPin(MPR121_INT);

  for(int i=0; i<12; i++){
    MPR121.setTouchThreshold(i, 12);
    MPR121.setTouchThreshold(i, 0.1);
  }
  
  for(int i=0; i<numSecondaryBoards; i++){
    for(int j=0; j<12; j++){
      thisExternalTouchStatus[i][j] = false;
      lastExternalTouchStatus[i][j] = false;
    }
  }

  for(int i=0; i<totalNumElectrodes; i++){
    isNewTouch[i] = false;
    isNewRelease[i] = false;
  }   

  for(int a=A0; a<=A5; a++){
    pinMode(a, OUTPUT);
    digitalWrite(a, LOW); 
  }

  Serial1.begin(9600);
  delay(100);

}

void loop(){ //constantly read the touch inputs from the electrodes
  
  // reset everything  that we combine from the two boards
  resetCompoundVariables();
  
  readLocalTouchInputs(); // read primary touch board
  
  readRemoteTouchInputs(); // read secondary touch board
  
  processTouchInputs(); // compile + press keys and activate/deactive LEDs
  
}


void readLocalTouchInputs(){

  // update our compound data on the local touch status

  if(MPR121.touchStatusChanged()){
    MPR121.updateTouchData();
    touchStatusChanged = true;
    
    for(int i=0; i<12; i++){
      isNewTouch[i] = MPR121.isNewTouch(i);
      isNewRelease[i] = MPR121.isNewRelease(i);
    }
  }
  numTouches+=MPR121.getNumTouches();

}

void readRemoteTouchInputs(){

  char incoming;

  for(int a=A0; a<A0+numSecondaryBoards; a++){

    digitalWrite(a, HIGH);
    delay(15);

    
    // only process if we have a full packet available
    while(Serial1.available() >= serialPacketSize){
      
      // save last status to detect touch / release edges
      for(int i=0; i<12; i++){
        lastExternalTouchStatus[a-A0][i] = thisExternalTouchStatus[a-A0][i];
      delay(10);
      }
      
      incoming = Serial1.read();
      if(incoming == 'T'){ // ensure we are synced with the packet 'header'
        for(int i=0; i<12; i++){
          if(!Serial1.available()){
            return; // shouldn't get here, but covers us if we run out of data
          } else {
            if(Serial1.read()=='1'){
              thisExternalTouchStatus[a-A0][i] = true;
              Serial.print("works up to here");
            } else {
              thisExternalTouchStatus[a-A0][i] = false;
            }
          }
        }
      } 
    }

    // now that we have read the remote touch data, merge it with the local data
    for(int i=0; i<12; i++){
      if(lastExternalTouchStatus[a-A0][i] != thisExternalTouchStatus[a-A0][i]){
        touchStatusChanged = true;
        if(thisExternalTouchStatus[a-A0][i]){
          // shift remote data up the array by 12 so as not to overwrite local data
          isNewTouch[i+(12*((a-A0)+1))] = true;
        } else {
          isNewRelease[i+(12*((a-A0)+1))] = true;
        }
      }

      // add any new touches to the touch count
      if(thisExternalTouchStatus[a-A0][i]){
        numTouches++;
      }
    }

    digitalWrite(a, LOW);
  }
}

void processTouchInputs(){
  // only make an action if we have one or fewer pins touched
  // ignore multiple touches
  
    for (int i=0; i < totalNumElectrodes; i++){  // Check which electrodes were pressed
      if(isNewTouch[i]){   
        //pin i was just touched
        if(led_status[i] == 0) {                              //if previously off, turn on now and activate the LEDs and sounds!
          led_status[i] = 1;
          colorWipe(colorscheme[i], i);                       //prompt LEDs

          Keyboard.press(keyMap[i]);                          //prompt keyboard input
          Keyboard.release(keyMap[i]);
          digitalWrite(LED_BUILTIN, HIGH);             
        }
        else{
          led_status[i] = 0;                                  //if previously on, turn off now and deactivate the LEDs and sounds
          Keyboard.press(keyMap[i]);
          Keyboard.release(keyMap[i]);
          digitalWrite(LED_BUILTIN, LOW);
          colorWipe(out, i);
        }
        // In drumdorp nothing happens when the electrodes are released. 
      }
    } 
}


void resetCompoundVariables(){

  // simple reset for all coumpound variables

  touchStatusChanged = false;
  numTouches = 0;

  for(int i=0; i<totalNumElectrodes; i++){
    isNewTouch[i] = false;
    isNewRelease[i] = false;
  }  
}
