// Arduino Code// Arduino Code

#include "DHT.h"
#define DHTPIN 5
#include <Wire.h>
#define DHTTYPE DHT11 // DHT11
#define SOUND_SPEED 0.034
DHT dht(DHTPIN, DHTTYPE);

unsigned long previousMillisSensors = 0;
const int trigPin = 9;
const int echoPin = 8;
volatile byte requisicao;
int valorTensaoldr = 0;
float valorADC = 0;

long duration;
double distanceCm;
double tempC;
bool LedInterno = false;

void hcsr04() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  
  distanceCm = duration * SOUND_SPEED/2;
  
  Serial.println("Distance (cm): ");
  Serial.println(distanceCm, 5);
  
}

void dhtCheck(){ 
  tempC = dht.readTemperature();
  Serial.print("Temperature: "); 
  Serial.print(tempC);Serial.println(" *C ");
}

void receberRequisicao(int numBytes) {
  //Serial.println("entrando receberRequisicao");
  if (Wire.available()) {
    //Serial.println("Entrando no if da func");
    requisicao = Wire.read(); 
    if(requisicao == 3) {
      invertLed();
    }
  }
}

void invertLed() {
  LedInterno = !LedInterno;
  if(LedInterno) digitalWrite(LED_BUILTIN, HIGH);
  else digitalWrite(LED_BUILTIN, LOW);
}

void enviarFloat() {
  float valorEnviar;

  //hcsr04();
  //dhtCheck();

  //Serial.println("Entrando no EnviaFloat(0)");
  if (requisicao == 1) valorEnviar = distanceCm;
  else if (requisicao == 2) valorEnviar = tempC;
  else return; 

  Serial.println(valorEnviar);
  byte *ptr = (byte*)&valorEnviar;
  for (int i = 0; i < sizeof(float); i++) {
    Wire.write(ptr[i]);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT); 
  pinMode(LED_BUILTIN, OUTPUT);
  
  dht.begin();
  Wire.begin(8);
  Wire.onReceive(receberRequisicao); 
  Wire.onRequest(enviarFloat);
}

void loop() {
  if(millis() - previousMillisSensors >= 100)
  {
    previousMillisSensors = millis();
    hcsr04();
    dhtCheck();
  }
}