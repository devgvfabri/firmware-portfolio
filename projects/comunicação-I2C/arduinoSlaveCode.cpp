// Arduino Code// Arduino Code

#include "DHT.h"
#define DHTPIN 5 // what digital pin we're connected to
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
double humi;
double tempC;

void hcsr04() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  
  distanceCm = duration * SOUND_SPEED/2;
  
  //Serial.println("Distance (cm): ");
  //Serial.println(distanceCm, 5);
  
}

void dhtCheck(){ 
  humi = dht.readHumidity();
  tempC = dht.readTemperature();
  //Serial.print("Humidity: ");
  //Serial.print(humi);Serial.print(" %\t");
  //Serial.print("Temperature: "); 
  //Serial.print(tempC);Serial.println(" *C ");
}

void receberRequisicao(int numBytes) {
  //Serial.println("entrando receberRequisicao");
  if (Wire.available()) {
    //Serial.println("Entrando no if da func");
    requisicao = Wire.read(); 
  }
}

void enviarFloat() {
  float valorEnviar;

  //hcsr04();
  //dhtCheck();

  //Serial.println("Entrando no EnviaFloat(0)");
  if (requisicao == 1) valorEnviar = tempC;
  else if (requisicao == 2) valorEnviar = humi;
  else if (requisicao == 3) valorEnviar = distanceCm;
  else return; 

  //Serial.println(humi);

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