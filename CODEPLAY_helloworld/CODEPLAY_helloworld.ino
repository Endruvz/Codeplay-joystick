#include <FastLED.h>
typedef struct {
  int16_t buttons;
  int16_t joy_x;
  int16_t joy_y;
  int16_t tilt_x;
  int16_t tilt_y;
  int16_t tilt_z;
} joystick_packet_t;

joystick_packet_t joystick;

#define Butt1 41
#define Butt2 40
#define Butt3 39
#define Butt4 14

#define PinX 11
#define PinY 12
int deadZone = 5;

#define PinLed 2
#define NUM_LEDS 1
CRGB led[NUM_LEDS];

void setup() {
  pinMode(Butt1, INPUT_PULLUP);
  pinMode(Butt2, INPUT_PULLUP);
  pinMode(Butt3, INPUT_PULLUP);
  pinMode(Butt4, INPUT_PULLUP);

  pinMode(PinX, INPUT);
  pinMode(PinY, INPUT);

  pinMode(PinLed, OUTPUT);
  
  FastLED.addLeds<WS2812B, PinLed>(led, NUM_LEDS);

Serial.begin(115200);
Serial.println("Serial Start!");
}


void loop() {
delay(10);

int ValX = map(analogRead(PinX), 0, 4095, -100, 100);
int ValY = map(analogRead(PinY), 0, 4095, -100, 100);

if (abs(ValX) <= deadZone){
  ValX = 0;
}

if (abs(ValY) <= deadZone){
  ValY = 0;
}


int gumbi = 0;
gumbi = gumbi | digitalRead(Butt1) | digitalRead(Butt2)*2 | digitalRead(Butt3)*4 | digitalRead(Butt4)*8; 


joystick.buttons = gumbi;

joystick.joy_x = ValX;
joystick.joy_y = ValY;

Serial.write((uint8_t*)&joystick, sizeof(joystick));

}
