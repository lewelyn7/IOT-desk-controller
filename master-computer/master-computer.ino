#include "FastLED.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
//
// RGB Calibration code
//
// Use this sketch to determine what the RGB ordering for your chipset should be.  Steps for setting up to use:

// * Uncomment the line in setup that corresponds to the LED chipset that you are using.  (Note that they
//   all explicitly specify the RGB order as RGB)
// * Define DATA_PIN to the pin that data is connected to.
// * (Optional) if using software SPI for chipsets that are SPI based, define CLOCK_PIN to the clock pin
// * Compile/upload/run the sketch

// You should see six leds on.  If the RGB ordering is correct, you should see 1 red led, 2 green
// leds, and 3 blue leds.  If you see different colors, the count of each color tells you what the
// position for that color in the rgb orering should be.  So, for example, if you see 1 Blue, and 2
// Red, and 3 Green leds then the rgb ordering should be BRG (Blue, Red, Green).

// You can then test this ordering by setting the RGB ordering in the addLeds line below to the new ordering
// and it should come out correctly, 1 red, 2 green, and 3 blue.
//
//////////////////////////////////////////////////

#define NUM_LEDS 120

// For led chips like WS2812, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
// Clock pin only needed for SPI based chipsets when not using hardware SPI
#define DATA_PIN 5
#define CLOCK_PIN 13

CRGB leds[NUM_LEDS];

void setup() {
    Serial.begin(9600);
    // sanity check delay - allows reprogramming if accidently blowing power w/leds
    delay(2000);

    // Uncomment/edit one of the following lines for your leds arrangement.
    // ## Clockless types ##
    // FastLED.addLeds<SM16703, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<TM1829, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<TM1812, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<TM1809, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<TM1804, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<TM1803, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<UCS1903, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<UCS1903B, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<UCS1904, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<UCS2903, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical
    // FastLED.addLeds<WS2852, DATA_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical
     FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);  // GRB ordering is typical
    // FastLED.addLeds<GS1903, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<SK6812, DATA_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical
    // FastLED.addLeds<SK6822, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<APA106, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<PL9823, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<SK6822, DATA_PIN, RGB>(leds, NUM_LEDS);
//     FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<WS2813, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<APA104, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<WS2811_400, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<GE8822, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<GW6205, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<GW6205_400, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<LPD1886, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<LPD1886_8BIT, DATA_PIN, RGB>(leds, NUM_LEDS);
    // ## Clocked (SPI) types ##
    // FastLED.addLeds<LPD6803, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical
    // FastLED.addLeds<LPD8806, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical
    // FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<WS2803, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<SM16716, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<P9813, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);  // BGR ordering is typical
    // FastLED.addLeds<DOTSTAR, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);  // BGR ordering is typical
    // FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);  // BGR ordering is typical
    // FastLED.addLeds<SK9822, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);  // BGR ordering is typical

//     FastLED.setBrightness(CRGB(255,255,255));
    FastLED.setCorrection(TypicalLEDStrip);
    FastLED.setTemperature(Halogen);
    for(int i = 0; i < NUM_LEDS; i++){
      leds[i] = CRGB(0,0,0);
    }
    FastLED.show();
}
int colorArray[3];
void loop() {
    if(Serial.available() >= 4){
      Serial.println("czytam");
      colorArray[0] = Serial.parseInt();
      colorArray[1] = Serial.parseInt();
      colorArray[2] = Serial.parseInt();

      Serial.read();
      Serial.print(colorArray[0]);
      Serial.print(colorArray[1]);
      Serial.print(colorArray[2]);
      Serial.println("przeczytalem");
      Serial.print(Serial.available());
    for(int i = 0; i < NUM_LEDS; i++){
      leds[i  ] = CRGB(colorArray[0], colorArray[1], colorArray[2]);
    }
    FastLED.show();
    FastLED.setCorrection(TypicalLEDStrip);
    FastLED.setTemperature(Halogen);
    }

}