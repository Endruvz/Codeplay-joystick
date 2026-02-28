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

#define joystickButton 10
#define PinX 11
#define PinY 12
int deadZone = 5;

#define PinLed 2
#define NUM_LEDS 1
CRGB led[NUM_LEDS];
static uint8_t hue;

#define buzzer 42
#define vibMotor 21

TaskHandle_t SensTaskHandle = NULL;
TaskHandle_t ReceiveTaskHandle = NULL;


void SensTask(void *parameter) {
  for (;;) {
    int ValX = map(analogRead(PinX), 0, 4095, -100, 100);
    int ValY = map(analogRead(PinY), 0, 4095, -100, 100);

    if (abs(ValX) <= deadZone) {
      ValX = 0;
    }

    if (abs(ValY) <= deadZone) {
      ValY = 0;
    }


    int gumbi = 0;
    gumbi = gumbi | digitalRead(Butt1) | digitalRead(Butt2) * 2 | digitalRead(Butt3) * 4 | digitalRead(Butt4) * 8 | digitalRead(joystickButton) * 16;


    joystick.buttons = gumbi;

    joystick.joy_x = ValX;
    joystick.joy_y = ValY;

    Serial.write((uint8_t *)&joystick, sizeof(joystick));
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void ReceiveTask(void *parameter) {
  for (;;) {
    led[0] = CHSV(hue++, 255, 255);
    FastLED.show();
    vTaskDelay(33 / portTICK_PERIOD_MS);
    if (Serial.available()) {
      int16_t indata = Serial.read();
      if (indata == 0x01) {
        //coin sound
        noTone(buzzer);
        tone(buzzer, 1000);
        vTaskDelay(80 / portTICK_PERIOD_MS);
        tone(buzzer, 1500);
        vTaskDelay(120 / portTICK_PERIOD_MS);
        noTone(buzzer);
        

      } else if (indata == 0x02) {
        //heal sound 
        noTone(buzzer);
        tone(buzzer, 300);
        vTaskDelay(60 / portTICK_PERIOD_MS);
        tone(buzzer, 500);
        vTaskDelay(60 / portTICK_PERIOD_MS);
        tone(buzzer, 300);
        vTaskDelay(60 / portTICK_PERIOD_MS);
        noTone(buzzer);

      } else if (indata == 0x03) {
        //kill
        noTone(buzzer);
        tone(buzzer, 100);
        vTaskDelay(200 / portTICK_PERIOD_MS);
        tone(buzzer, 75);
        vTaskDelay(150 / portTICK_PERIOD_MS);
        tone(buzzer, 50);
        vTaskDelay(300 / portTICK_PERIOD_MS);
        noTone(buzzer);
      } else if (indata == 0x04) {
        //damage
        noTone(buzzer);
        digitalWrite(vibMotor, HIGH);
        tone(buzzer, 200);
        vTaskDelay(300 / portTICK_PERIOD_MS);
        digitalWrite(vibMotor, LOW);
        tone(buzzer, 100);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        noTone(buzzer);
        
      } else if (indata == 0x05) {
        //death
        noTone(buzzer);
        digitalWrite(vibMotor, HIGH);
        tone(buzzer, 1100);
        vTaskDelay(300 / portTICK_PERIOD_MS);
        digitalWrite(vibMotor, LOW);
        tone(buzzer, 800);
        vTaskDelay(300 / portTICK_PERIOD_MS);
        noTone(buzzer);
        tone(buzzer, 300);
        vTaskDelay(750 / portTICK_PERIOD_MS);
        noTone(buzzer);
      }
    }
  }
}

void setup() {
  pinMode(Butt1, INPUT_PULLUP);
  pinMode(Butt2, INPUT_PULLUP);
  pinMode(Butt3, INPUT_PULLUP);
  pinMode(Butt4, INPUT_PULLUP);

  pinMode(joystickButton, INPUT);
  pinMode(PinX, INPUT);
  pinMode(PinY, INPUT);

  pinMode(PinLed, OUTPUT);

  pinMode(buzzer, OUTPUT);
  pinMode(vibMotor, OUTPUT);

  FastLED.addLeds<WS2812B, PinLed>(led, NUM_LEDS);

  Serial.begin(115200);

  //startup sound
  tone(buzzer, 300);
  digitalWrite(vibMotor, HIGH);
  delay(200);
  tone(buzzer, 500);
  delay(150);
  tone(buzzer, 1000);
  delay(300);
  digitalWrite(vibMotor, LOW);
  noTone(buzzer);



  xTaskCreatePinnedToCore(
    SensTask,         // Task function
    "SensTask",       // Task name
    10000,            // Stack size (bytes)
    NULL,             // Parameters
    1,                // Priority
    &SensTaskHandle,  // Task handle
    1                 // Core 1
  );


  xTaskCreatePinnedToCore(
    ReceiveTask,         // Task function
    "ReceiveTask",       // Task name
    10000,               // Stack size (bytes)
    NULL,                // Parameters
    1,                   // Priority
    &ReceiveTaskHandle,  // Task handle
    0                    // Core 1
  );
}

void loop() {
}
