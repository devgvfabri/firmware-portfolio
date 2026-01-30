// Arduino Code
#include "DHT.h"
#define DHTPIN 5 // what digital pin we're connected to
#define DHTTYPE DHT11 // DHT11
#define SOUND_SPEED 0.034
DHT dht(DHTPIN, DHTTYPE);

const long interval1 = 100;
const long interval2 = 50;

bool ledState = LOW;
bool ledState1 = LOW;

unsigned long previousMillisBlinkled1 = 0;
unsigned long previousMillisBlinkled2 = 0;
unsigned long previousMillisSensors = 0;
const int trigPin = 5;
const int echoPin = 8;

int valorTensaoldr = 0;
float valorADC = 0;

long duration;
float distanceCm; 

void hcsr04() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  
  distanceCm = duration * SOUND_SPEED/2;
  
  if(distanceCm >= 330.0){
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
    Serial.println("Sem obstaculos");
  }else if(distanceCm >= 150){
    digitalWrite(6, LOW);
    digitalWrite(7, HIGH);
    Serial.println("Longe");
  }else if(distanceCm >= 50){
    digitalWrite(6, HIGH);
    digitalWrite(7, LOW);
    Serial.println("Medio");
  }else{
    digitalWrite(6, HIGH);
    digitalWrite(7, HIGH);
    Serial.println("Perto");
  }
  
  Serial.println("Distance (cm): ");
  Serial.println(distanceCm);
  
}

void dht()
{ 
  float humi = dht.readHumidity();
  float tempC = dht.readTemperature();
  Serial.print("Humidity: ");
  Serial.print(humi);Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(tempC);Serial.println(" *C ");
  if(humi >= 70){
    digitalWrite(3, HIGH);
  }else{
    digitalWrite(3, LOW);
}
}

void ldr(){
  valorTensaoldr = analogRead(A1);
  valorADC = valorTensaoldr *(5.0/1023);
  
  if(valorADC <= 1.5){
    Serial.println("Obstaculo acima");
    Serial.println(valorADC);
    digitalWrite(2, HIGH);
  }else{
    Serial.println("Sem obstaculos acima");
    digitalWrite(2, LOW);
  }
}

void idle() {
  digitalWrite(13, LOW);
  digitalWrite(12, LOW);
}

void cleaning() {
  digitalWrite(12, LOW);
  if (millis() - previousMillisBlinkled1 >= interval1) {
    previousMillisBlinkled1 = millis();
    Serial.println(ledState);
    Serial.println(millis());
    ledState = !ledState;
    digitalWrite(13, ledState);
  }
}

void docking() {
  digitalWrite(13, LOW);
  if (millis() - previousMillisBlinkled2 >= interval2) {
    previousMillisBlinkled2 = millis();
    ledState1 = !ledState1;
    digitalWrite(12, ledState1);
  }
}

void charging() {
  if (millis() - previousMillisBlinkled1 >= interval2) {
    previousMillisBlinkled1 = millis();
    ledState = !ledState;
    digitalWrite(12, ledState);
  }

  if (millis() - previousMillisBlinkled2 >= interval1) {
    previousMillisBlinkled2 = millis();
    ledState1 = !ledState1;
    digitalWrite(13, ledState1);
  }
}

void setup() {
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  Serial.begin(115200);
  pinMode(11, INPUT);
  pinMode(10, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT); 
  pinMode(6, OUTPUT); // estado distancia
  pinMode(7, OUTPUT); // estado distancia
  dht.begin();
  pinMode(8, OUTPUT); // estado umidade
  pinMode(2, OUTPUT); // estado luminosidade

}

void loop() {
  int i = digitalRead(11);
  int j = digitalRead(10);
  static int lastStateI = -1, lastStateJ = -1;
  if(millis() - previousMillisSensors >= 100)
  {
    previousMillisSensors = millis();
    hcsr04();
    dht();
    ldr();
  }
  Serial.println(i);
  Serial.println(j);
  Serial.println("----");

  if (i != lastStateI || j != lastStateJ) {
    previousMillisBlinkled1 = millis();
    previousMillisBlinkled2 = millis();
    lastStateI = i;
    lastStateJ = j;
  }

  if (i == 0 && j == 0) idle();
  else if (i == 0 && j == 1) cleaning();
  else if (i == 1 && j == 0) docking();
  else if (i == 1 && j == 1) charging();
}