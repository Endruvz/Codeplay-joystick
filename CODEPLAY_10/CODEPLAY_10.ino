#include <FastLED.h>
#include <LSM6DS3-SOLDERED.h>
#include <string>

Soldered_LSM6DS3 lsm6ds3;

typedef struct {
  int16_t buttons;
  int16_t joy_x;
  int16_t joy_y;
  int16_t tilt_x;
  int16_t tilt_y;
  int16_t tilt_z;
} joystick_packet_t;

joystick_packet_t joystick = { 0 };
joystick_packet_t last_joystick = { 0 };

#define TIMEOUT 30000
unsigned long lastActivity = 0;


#define Butt1 41
#define Butt2 40
#define Butt3 39
#define Butt4 14
#define debounce 10
int gumbi;
unsigned long lastPress = 0;

#define joystickButton 10
#define PinX 11
#define PinY 12
int deadZone = 5;


bool draw = false;
bool drawStart = false;
int StartX;
int StartY;
int dX = 0;
int dY = 0;
int startNode;
int endNode;
String znak;

int screenLines[20];
int screenConnect[9][9]{
  { 0, 1, 21, 3, 4, 0, 22, 0, 23 },
  { 1, 0, 2, 5, 6, 7, 0, 24, 0 },
  { 21, 2, 0, 8, 9, 9, 25, 0, 26 },
  { 3, 5, 0, 0, 10, 27, 12, 13, 0 },
  { 4, 6, 8, 10, 0, 11, 14, 15, 16 },
  { 0, 7, 9, 27, 11, 0, 0, 17, 18 },
  { 22, 0, 25, 12, 14, 0, 0, 19, 28 },
  { 0, 24, 0, 13, 15, 17, 19, 0, 20 },
  { 23, 0, 26, 0, 16, 18, 28, 20, 0 }
};

String characterIndexFind[36] = {
  //"11100000100100000111", // 0
  "00000100000000100000",  // 1 (Centered vertical)
  "11000000111100000011",  // 2
  "11000000111000000111",  // 3
  "00100000111000000100",  // 4
  //"11100000011000000111", // 5
  "11100000011100000111",  // 6
  "11000000100000000100",  // 7
  "11100000111100000111",  // 8
  "11100000111000000111",  // 9
  "11100000111100000100",  // A
  "11000100111000100111",  // B
  "11100000000100000011",  // C
  "11000100100000100111",  // D
  "11100000011100000011",  // E
  "11100000011100000000",  // F
  "11100000001100000111",  // G
  "00100000111100000100",  // H
  "11000100000000100011",  // I
  "00000000100100000111",  // J
  "00100001010100010000",  // K
  "00100000000100000011",  // L
  "00110001100100000100",  // M
  "00110000100100010100",  // N
  "11100000100100000111",  // O (Same as 0)
  "11100000111100000000",  // P
  "11100000100100010111",  // Q
  "11100000111100010000",  // R
  "11100000011000000111",  // S (Same as 5)
  "11000100000000100000",  // T
  "00100000100100000111",  // U
  "00100000100010001000",  // V
  "00100000100101010100",  // W
  "00010001000010001000",  // X
  "00010001000000100000",  // Y
  "11000001000001000011"   // Z
};

char characterList[36]{ '1', '2', '3', '4', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };


#define PinLed 2
#define NUM_LEDS 1
CRGB led[NUM_LEDS];
static uint8_t hue;

#define buzzer 42
#define vibMotor 21

TaskHandle_t SensTaskHandle = NULL;
TaskHandle_t ReceiveTaskHandle = NULL;


void SensTask(void *parameter) {
  lastActivity = millis();

  for (;;) {

    if (!draw) {
      int32_t accelerometer[3];
      lsm6ds3.getAcceleratorAxes(accelerometer);
      joystick.tilt_x = map(accelerometer[0], -550, 550, -100, 100);
      joystick.tilt_y = map(accelerometer[1], -550, 550, -100, 100);
      joystick.tilt_z = map(accelerometer[2], -550, 550, -100, 100);
    }

    int ValX = map(analogRead(PinX), 0, 4095, -100, 100);
    int ValY = map(analogRead(PinY), 0, 4095, -100, 100);

    if (abs(ValX) <= deadZone) {
      ValX = 0;
    }

    if (abs(ValY) <= deadZone) {
      ValY = 0;
    }

    joystick.joy_x = ValX;
    joystick.joy_y = ValY;

    gumbi = 0;
    gumbi = gumbi | digitalRead(Butt1) | digitalRead(Butt2) * 2 | digitalRead(Butt3) * 4 | digitalRead(Butt4) * 8 | digitalRead(joystickButton) * 16;

    if (gumbi != joystick.buttons && millis() - lastPress > debounce){
      joystick.buttons = gumbi;
      lastPress = millis();
    }
    
    if (inputChanged(joystick, last_joystick)) {
      Serial.write((uint8_t *)&joystick, sizeof(joystick));
      last_joystick = joystick;
      lastActivity = millis();
    }

    if (millis() - lastActivity >= TIMEOUT) {
      deepSleep();
    }

    if (draw) {

      if (!drawStart && !digitalRead(Butt1)) {

        StartX = ValX;
        StartY = ValY;
        drawStart = true;
      }

      if (drawStart && digitalRead(Butt1)) {

        dX = ValX - StartX;
        dY = ValY - StartY;
        drawStart = false;

        startNode = map(StartX, -100, 100, 1, 3) + (map(StartY, -100, 100, 3, 1) - 1) * 3;
        endNode = map(ValX, -100, 100, 1, 3) + (map(ValY, -100, 100, 3, 1) - 1) * 3;

        if (screenConnect[startNode - 1][endNode - 1] <= 20 && screenConnect[startNode - 1][endNode - 1] != 0) {
          screenLines[screenConnect[startNode - 1][endNode - 1] - 1] = 1;
        }

        else if (screenConnect[startNode - 1][endNode - 1] > 20) {
          int line = screenConnect[startNode - 1][endNode - 1];

          if (line == 21) {
            screenLines[0] = 1;
            screenLines[1] = 1;
          }
          if (line == 22) {
            screenLines[2] = 1;
            screenLines[11] = 1;
          }
          if (line == 23) {
            screenLines[3] = 1;
            screenLines[15] = 1;
          }
          if (line == 24) {
            screenLines[5] = 1;
            screenLines[14] = 1;
          }
          if (line == 25) {
            screenLines[7] = 1;
            screenLines[13] = 1;
          }
          if (line == 26) {
            screenLines[8] = 1;
            screenLines[17] = 1;
          }
          if (line == 27) {
            screenLines[9] = 1;
            screenLines[10] = 1;
          }
          if (line == 28) {
            screenLines[18] = 1;
            screenLines[19] = 1;
          }
        }
      }

      if (!drawStart && !digitalRead(Butt2)) {
        znak = "";
        for (int i = 0; i < 20; i++) {
          znak += screenLines[i];
          screenLines[i] = 0;
        }

        for (int i = 0; i < 36; i++) {
          if (znak == characterIndexFind[i]) {
            Serial.write((uint8_t *)&characterList[i], sizeof(characterList[i]));
            lastActivity = millis();
            break;
          }
        }
      }
    }

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

        tone(buzzer, 1500);
        vTaskDelay(120 / portTICK_PERIOD_MS);
        noTone(buzzer);
        serialFlush();


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
        serialFlush();


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
        serialFlush();

      } else if (indata == 0x04) {
        //damage
        digitalWrite(vibMotor, HIGH);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        digitalWrite(vibMotor, LOW);
        serialFlush();

      } else if (indata == 0x05) {

        if (SensTaskHandle != NULL) {
          vTaskDelete(SensTaskHandle);
          SensTaskHandle = NULL;
        }

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

        deepSleep();
      }
    }
  }
}

void serialFlush() {
  while (Serial.available() > 0) {
    char t = Serial.read();
  }
}

void deepSleep() {
  Serial.flush();

  if (SensTaskHandle != NULL) {
    vTaskDelete(SensTaskHandle);
    SensTaskHandle = NULL;
  }

  FastLED.clear();
  FastLED.show();
  noTone(buzzer);
  digitalWrite(vibMotor, LOW);

  esp_sleep_enable_ext0_wakeup((gpio_num_t)Butt4, LOW);
  esp_deep_sleep_start();
}

bool inputChanged(const joystick_packet_t &current, const joystick_packet_t &last) {
  if (current.buttons != last.buttons) return true;

  if (abs(current.joy_x - last.joy_x) > 2) return true;
  if (abs(current.joy_y - last.joy_y) > 2) return true;
  if (abs(current.tilt_x - last.tilt_x) > 3) return true;
  if (abs(current.tilt_y - last.tilt_y) > 3) return true;
  if (abs(current.tilt_z - last.tilt_z) > 3) return true;

  return false;
}



void setup() {
  pinMode(Butt1, INPUT_PULLUP);
  pinMode(Butt2, INPUT_PULLUP);
  pinMode(Butt3, INPUT_PULLUP);
  pinMode(Butt4, INPUT_PULLUP);

  pinMode(joystickButton, INPUT);
  pinMode(PinX, INPUT);
  pinMode(PinY, INPUT);

  for (int i = 0; i < 20; i++) {
    screenLines[i] = 0;
  }

  pinMode(PinLed, OUTPUT);

  pinMode(buzzer, OUTPUT);
  pinMode(vibMotor, OUTPUT);

  FastLED.addLeds<WS2812B, PinLed>(led, NUM_LEDS);

  Serial.begin(115200);

  Wire.begin();
  lsm6ds3.begin();
  lsm6ds3.enableAccelerator();

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

  while (digitalRead(Butt1) && digitalRead(Butt4)) {}
  gumbi = gumbi | digitalRead(Butt1) | digitalRead(Butt2) * 2 | digitalRead(Butt3) * 4 | digitalRead(Butt4) * 8 | digitalRead(joystickButton) * 16;
  joystick.buttons = gumbi;

  Serial.write((uint8_t *)&joystick, sizeof(joystick));
  last_joystick = joystick;

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
    0                    // Core 0
  );
}

void loop() {
}
