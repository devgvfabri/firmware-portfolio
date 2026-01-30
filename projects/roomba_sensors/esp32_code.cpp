//
// ESP32 Code
//
// 00 = idle
// 01 = cleaning
// 10 = docking
// 11 = charging

enum State {
  State_IDLE,
  State_CLEANING,
  State_DOCKING,
  State_CHARGING
};

State CurrentState = State_IDLE;

unsigned long previousMillisTotal = 0;
unsigned long previousMillisPrints = 0;
char a;

// dacWrite funciona apenas nos pinos 25 e 26 no ESP32.
const int DAC = 25;

void setup() {
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(DAC, OUTPUT);
  pinMode(10, INPUT); // 1 bit  estado distancia
  pinMode(11, INPUT); // 2 bits estado distancia
  Serial.begin(115200); 
  pinMode(7,INPUT); // sinal umidade
}

void loop() {
  bool stateD0 = digitalRead(10);
  bool stateD1 = digitalRead(11);
  bool stateU = digitalRead(7);
  bool stateL = digitalRead(6);
  if(millis() - previousMillisPrints >= 1000)
  {
    previousMillisPrints = millis();
    switch(CurrentState){
      case State_IDLE:
         Serial.println("Idle");
      break;
      case State_CLEANING:
         Serial.println("Cleaning");
      break;
      case State_DOCKING:
         Serial.println("Docking");
      break;
      case State_CHARGING:
         Serial.println("Charging");
      break;
      default:
       break;
    }
    if(stateD0 == LOW && stateD1 == LOW){
      Serial.println("Sem obstaculos");
    }else if(stateD0 == LOW && stateD1 == HIGH){
      Serial.println("Longe");
    }else if(stateD0 == HIGH && stateD1 == LOW){
      Serial.println("Meia distancia");
    }else if(stateD0 == HIGH && stateD1 == HIGH){
      Serial.println("Perto");
    }
    if(stateU == HIGH){
     Serial.println("Umidade não aceitavel");
    }else{
     Serial.println("Umidade aceitavel");
    }
    if(stateL == HIGH){
     Serial.println("Obstaculos acima");
    }else{
     Serial.println("Sem obstaculos acima");
    }
  }

  switch (CurrentState) {

    case State_IDLE:
      digitalWrite(13, LOW);
      digitalWrite(12, LOW);
      dacWrite(DAC, 255); // brilho máximo

      if (Serial.available()) {
        a = Serial.read();
        if (a == 'a') {
          CurrentState = State_CLEANING;
          digitalWrite(13, LOW);
          digitalWrite(12, HIGH);
          previousMillisTotal = millis();
        }
      }
      break;

    case State_CLEANING:
      // FadeOff do terceiro LED
      unsigned long elapsedTimeCleaning = millis() - previousMillisTotal;
      int brightnessDown = map(elapsedTimeCleaning, 0, 2000, 255, 0);
      dacWrite(DAC, brightnessDown);

      if (Serial.available()) {
        a = Serial.read();
        if (a == 'b') {
          CurrentState = State_DOCKING;
          previousMillisTotal = millis();
          digitalWrite(13, HIGH);
          digitalWrite(12, LOW);
          break;
        }
      }

      if (millis() - previousMillisTotal >= 7000) {
        previousMillisTotal = millis();
        CurrentState = State_DOCKING;
        digitalWrite(13, HIGH);
        digitalWrite(12, LOW);
      }
      break;

    case State_DOCKING:
      dacWrite(DAC, 0); // brilho mínimo (desliga)

      if (millis() - previousMillisTotal >= 7000) {
        previousMillisTotal = millis();
        CurrentState = State_CHARGING;
        digitalWrite(13, HIGH);
        digitalWrite(12, HIGH);
      }
      break;

    case State_CHARGING:
      // FadeIn do terceiro LED
      unsigned long elapsedTimeCharging = millis() - previousMillisTotal;
      int brightnessUp = map(elapsedTimeCharging, 0, 2000, 0, 255);
      dacWrite(DAC, brightnessUp);

      if (Serial.available()) {
        a = Serial.read();
        if (a == 'a') {
          previousMillisTotal = millis();
          CurrentState = State_CLEANING;
          digitalWrite(13, LOW);
          digitalWrite(12, HIGH);
          break;
        }
      }

      if (millis() - previousMillisTotal >= 7000) {
        previousMillisTotal = millis();
        CurrentState = State_IDLE;
      }
      break;

    default:
      Serial.println("Estado Inválido!");
      CurrentState = State_IDLE;
      break;
  }
}