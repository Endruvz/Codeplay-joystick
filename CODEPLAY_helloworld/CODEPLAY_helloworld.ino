#define Butt1 41
#define Butt2 39
#define Butt3 40
#define Butt4 14

void setup() {
  
  pinMode(INPUT_PULLUP, Butt1);
  pinMode(INPUT_PULLUP, Butt2);
  pinMode(INPUT_PULLUP, Butt3);
  pinMode(INPUT_PULLUP, Butt4);

Serial.begin(115200);
Serial.println("Dobar pocetak za Codeplay!");
}
void loop() {
delay(1000);


Serial.println(digitalRead(Butt1));
Serial.println(digitalRead(Butt2));
Serial.println(digitalRead(Butt3));
Serial.println(digitalRead(Butt4));

}