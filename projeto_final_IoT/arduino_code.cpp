// Arduino Code

//necessita modificar o código

#include "DHT.h"
#define DHTPIN 5
#include <Wire.h>
#define DHTTYPE DHT11 // DHT11
#define SOUND_SPEED 0.034
DHT dht(DHTPIN, DHTTYPE);

#define ADCPIN A3
#define LDRPIN A2// Pino de leitura do sensor


unsigned long previousMillisSensors = 0;
volatile byte requisicao;
int valorTensaoldr = 0;
float valorADC = 0;
float tempADC = 0;

long duration;
double tempC;

const int MPU = 0x68;


float ldrValor = 0;
float lm35Temp = 0;
float dhtHumi = 0;

int acelX, acelY, acelZ;
int giroX, giroY, giroZ;
float mpuTemp = 0;

void inicializaMPU() {
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);        // PWR_MGMT_1
  Wire.write(0x01);        // Clock PLL com eixo X
  Wire.endTransmission();

  // Configura filtro passa-baixa (DLPF)
  Wire.beginTransmission(MPU);
  Wire.write(0x1A);        // CONFIG
  Wire.write(0x03);        // DLPF ~44Hz (ideal p/ movimentos humanos)
  Wire.endTransmission();

  // Escala do giroscópio: ±250 °/s
  Wire.beginTransmission(MPU);
  Wire.write(0x1B);        // GYRO_CONFIG
  Wire.write(0x00);        // FS_SEL = 0
  Wire.endTransmission();

  // Escala do acelerômetro: ±2g
  Wire.beginTransmission(MPU);
  Wire.write(0x1C);        // ACCEL_CONFIG
  Wire.write(0x00);        // AFS_SEL = 0
  Wire.endTransmission();

  // Taxa de amostragem = 1kHz / (1 + SMPLRT_DIV)
  Wire.beginTransmission(MPU);
  Wire.write(0x19);        // SMPLRT_DIV
  Wire.write(0x09);        // ~100 Hz
  Wire.endTransmission();
}

void giroscopico() {
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 14, true);

  int16_t acelX = Wire.read()<<8 | Wire.read();
  int16_t acelY = Wire.read()<<8 | Wire.read();
  int16_t acelZ = Wire.read()<<8 | Wire.read();
  int16_t tempRaw = Wire.read()<<8 | Wire.read();
  int16_t giroX = Wire.read()<<8 | Wire.read();
  int16_t giroY = Wire.read()<<8 | Wire.read();
  int16_t giroZ = Wire.read()<<8 | Wire.read();

  float gx = giroX / 131.0;
  float gy = giroY / 131.0;
  float gz = giroZ / 131.0;

  float mpuTemp = tempRaw / 340.0 + 36.53;

  Serial.print("Giro (°/s): ");
  Serial.print(gx); Serial.print(" ");
  Serial.print(gy); Serial.print(" ");
  Serial.println(gz);
}

void ldr(){
  valorTensaoldr = analogRead(LDRPIN);
  ldrValor = valorTensaoldr *(5.0/1023);
  Serial.println(valorTensaoldr);
  Serial.println(ldrValor);
  if(ldrValor < 2.6){
    Serial.println("Baixa luminosidade");
    Serial.println(ldrValor);
  }else{
    Serial.println("Alta luminosidade");
  }
}

int analogAvg(int sensorPin) {
  int amostras = 20;
  long soma = 0;
  for (int i = 0; i < amostras; ++i) {
    soma += analogRead(sensorPin);
    delay(3); // pequeno delay para estabilizar entre amostras (opcional)
  }
  return (int)(soma / amostras);
}

void templm35() {
  int leitura = analogAvg(ADCPIN);
  Serial.println(leitura);
   
  lm35Temp = leitura * 5.0 / 1023.0 * 100.0;
  // float temperatura = (leitura * 1.1 / 1023 - 0.5 )* 100 ; tmp 63

  Serial.print("LM35: ");
  Serial.print(lm35Temp);
  Serial.println(" C");
}

void dhtCheck(){
   dhtHumi = dht.readHumidity();
   delay(3);
   Serial.print("Umidade: ");
   Serial.println(dhtHumi);
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

  //Serial.println("Entrando no EnviaFloat(0)");
  switch (requisicao) {
    case 1: valorEnviar = ldrValor; break;
    case 2: valorEnviar = lm35Temp; break;
    case 3: valorEnviar = dhtHumi; break;
    case 4: valorEnviar = (float)giroX; break;
    case 5: valorEnviar = (float)giroY; break;
    case 6: valorEnviar = (float)giroZ; break;
    default: valorEnviar = 202; // erro
  }

  Serial.println(valorEnviar);
  byte *ptr = (byte*)&valorEnviar;
  for (int i = 0; i < sizeof(float); i++) {
    Wire.write(ptr[i]);
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  Wire.begin(4);
  Wire.onReceive(receberRequisicao);
  Wire.onRequest(enviarFloat);
  analogReference(DEFAULT);
  inicializaMPU();

}

void loop() {
  if(millis() - previousMillisSensors >= 500)
  {
    previousMillisSensors = millis();
    ldr();
    templm35();
    dhtCheck();
    gisroscopico();
  }
}
