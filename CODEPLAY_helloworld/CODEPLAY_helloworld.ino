#define Butt1 41
#define Butt2 39
#define Butt3 40
#define Butt4 14

void setup() {
  
  digitalRead(INPUT_PULLUP)

Serial.begin(115200);
Serial.println("Dobar pocetak za Codeplay!");
}
void loop() {
delay(1000);
Serial.println("Radi");
}