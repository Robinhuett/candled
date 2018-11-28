#include <Adafruit_NeoPixel.h>

#define led_pin 2
Adafruit_NeoPixel leds = Adafruit_NeoPixel(4, led_pin, NEO_RGBW + NEO_KHZ800);

void setup() {
  leds.begin();
  leds.show();
}

#define MINUNCALM (5*256)
#define MAXUNCALM (60*256)
#define UNCALMINC 10
#define MAXDEV    100
int centerx = MAXDEV;
int centery = MAXDEV / 2;

int xvel = 0;
int yvel = 0;
int uncalm = MINUNCALM;
int uncalmdir = UNCALMINC;
char cnt = 0;
int retriggercnt = 4;

uint32_t powcolor(int val) {
  if (val > 255)
    val = 255;
  if (val < 0)
    val = 0;

  return leds.Color(val / 4, val, val / 32);
}

void loop() {
  int movx = 0;
  int movy = 0;
  //random trigger brightness oscillation, if at least half uncalm
  if (retriggercnt > 0 && uncalm > (MAXUNCALM / 2)) {
    if (random(2000) < 4) {
      uncalm = MAXUNCALM + (MAXUNCALM / 4); //occasional 'bonus' wind
      retriggercnt--;
    }
  }

  if (retriggercnt != 4 && uncalm < (MAXUNCALM / 2)) {
    retriggercnt = 4;
  }

  //random poke, intensity determined by uncalm value (0 is perfectly calm)
  movx = random(uncalm >> 8) - (uncalm >> 9);
  movy = random(uncalm >> 8) - (uncalm >> 9);
  
  //if reach most calm value, start moving towards uncalm
  if (uncalm < MINUNCALM) {
    uncalmdir = UNCALMINC;
  }

  //if reach most uncalm value, start going towards calm
  if (uncalm > MAXUNCALM) {
    uncalmdir = -UNCALMINC;
  }
  uncalm += uncalmdir;

  //move center of flame by the current velocity
  centerx += movx + (xvel >> 2);
  centery += movy + (yvel >> 2);

  //range limits
  if (centerx < -MAXDEV)
    centerx = -MAXDEV;
  if (centerx > MAXDEV)
    centerx = MAXDEV;
  if (centery < -MAXDEV)
    centery = -MAXDEV;
  if (centery > MAXDEV)
    centery = MAXDEV;

  //counter
  cnt++;
  if (! (cnt & 3)) {
    //attenuate velocity 1/4 clicks
    xvel = (xvel * 999) / 1000;
    yvel = (yvel * 999) / 1000;
  }

  //apply acceleration towards center, proportional to distance from center (spring motion; hooke's law)
  xvel -= centerx;
  yvel -= centery;

  leds.setPixelColor(0, powcolor(128 - centerx - centery));
  leds.setPixelColor(1, powcolor(128 + centerx - centery));
  leds.setPixelColor(2, powcolor(128 + centerx + centery));
  leds.setPixelColor(3, powcolor(128 - centerx + centery));
  leds.show();
  delay(20);
}
