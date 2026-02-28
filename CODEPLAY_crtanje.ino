#include <string>

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

bool draw = false;
int drawMode = 0; //0 je za brojeve, 1 je za slova
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
  {0,1,21,3,4,0,22,0,23},
  {1,0,2,5,6,7,0,24,0},
  {21,2,0,8,9,9,25,0,26},
  {3,5,0,0,10,27,12,13,0},
  {4,6,8,10,0,11,14,15,16},
  {0,7,9,27,11,0,0,17,18},
  {22,0,25,12,14,0,0,19,28},
  {0,24,0,13,15,17,19,0,20},
  {23,0,26,0,16,18,28,20,0}
};

String characterIndexFind[36] = {
  "11100000100100000111", // 0
  "00000100000000100000", // 1 (Centered vertical)
  "11000000111100000011", // 2 
  "11000000111000000111", // 3
  "00100000111000000100", // 4
  "11100000011000000111", // 5
  "11100000011100000111", // 6
  "11000000100000000100", // 7
  "11100000111100000111", // 8
  "11100000111000000111", // 9
  "11100000111100000100", // A
  "11000100111000100111", // B
  "11100000000100000011", // C
  "11000100100000100111", // D
  "11100000011100000011", // E
  "11100000011100000000", // F
  "11100000001100000111", // G
  "00100000111100000100", // H
  "11000100000000100011", // I
  "00000000100100000111", // J
  "00100001010100010000", // K
  "00100000000100000011", // L
  "00110001100100000100", // M
  "00110000100100010100", // N
  "11100000100100000111", // O (Same as 0)
  "11100000111100000000", // P
  "11100000100100010111", // Q
  "11100000111100010000", // R
  "11100000011000000111", // S (Same as 5)
  "11000100000000100000", // T
  "00100000100100000111", // U
  "00100000100010001000", // V
  "00100000100101010100", // W
  "00010001000010001000", // X
  "00010001000000100000", // Y
  "11000001000001000011"  // Z
};

char characterList[36]{'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};

void setup() {
  draw = true;
  drawStart = false;
  drawMode = 0;
  
  pinMode(Butt1, INPUT_PULLUP);
  pinMode(Butt2, INPUT_PULLUP);
  pinMode(Butt3, INPUT_PULLUP);
  pinMode(Butt4, INPUT_PULLUP);

  pinMode(PinX, INPUT);
  pinMode(PinY, INPUT);

  for(int i = 0;i<20;i++){
    screenLines[i] = 0;
  }

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

if (draw){

  if (!drawStart && !digitalRead(Butt1)){
    Serial.println("Pen Down!");
    StartX = ValX;
    StartY = ValY;
    drawStart = true;
  }

  if (drawStart && digitalRead(Butt1)){
    Serial.println("Pen Up!");
    dX = ValX - StartX;
    dY = ValY - StartY;
    drawStart = false;

    Serial.print("DX: ");
    Serial.println(dX);
    Serial.print("DY: ");
    Serial.println(dY);

    startNode = map(StartX,-100,100,1,3) + (map(StartY,-100,100,3,1)-1)*3;
    endNode = map(ValX,-100,100,1,3) + (map(ValY,-100,100,3,1)-1)*3;

    Serial.print("LINE: ");
    Serial.println(screenConnect[startNode-1][endNode-1]);

    if (screenConnect[startNode-1][endNode-1] <= 20 && screenConnect[startNode-1][endNode-1] != 0){
      screenLines[screenConnect[startNode-1][endNode-1]-1] = 1;
      Serial.println("Manje od 20!");
    }

    else if (screenConnect[startNode-1][endNode-1] > 20){
      Serial.println("Veće od 20!");
      int line = screenConnect[startNode-1][endNode-1];

      if (line == 21){
        screenLines[0] = 1;
        screenLines[1] = 1;
      }
      if (line == 22){
        screenLines[2] = 1;
        screenLines[11] = 1;
      }
      if (line == 23){
        screenLines[3] = 1;
        screenLines[15] = 1;
      }
      if (line == 24){
        screenLines[5] = 1;
        screenLines[14] = 1;
      }
      if (line == 25){
        screenLines[7] = 1;
        screenLines[13] = 1;
      }
      if (line == 26){
        screenLines[8] = 1;
        screenLines[17] = 1;
      }
      if (line == 27){
        screenLines[9] = 1;
        screenLines[10] = 1;
      }
      if (line == 28){
        screenLines[18] = 1;
        screenLines[19] = 1;
      }
    }
    
  }

  if (!drawStart && !digitalRead(Butt2)){
    znak = "";
    for(int i=0;i<20;i++){
      znak += screenLines[i];
      screenLines[i] = 0;
    }

    for(int i=0;i<36;i++){
      if (znak == characterIndexFind[i]){
        Serial.print("ZNAK: ");
        Serial.println(characterList[i]);
        break;
      }
    }
    
    Serial.println(znak);
    delay(1000);
  }
 
}

}