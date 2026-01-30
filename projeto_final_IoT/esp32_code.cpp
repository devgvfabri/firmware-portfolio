// esp32

#include <Wire.h>

// C++ code

//dataloger remoter

#include <PubSubClient.h>
#include <WiFi.h>
#include <math.h>

const char *WIFI_SSID = "moto g84 5G_5388";
const char *WIFI_PASSW = "fcnb64axjefe8a9";
const char *MQTT_SERVER = "broker.hivemq.com";
const uint16_t MQTT_PORT = 1883;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

const char *MQTT_MONITORADOR_SONO_GIROX = "/publishESP32/IoT5/giroscópioX";
const char *MQTT_MONITORADOR_SONO_GIROY = "/publishESP32/IoT5/giroscópioY";
const char *MQTT_MONITORADOR_SONO_GIROZ = "/publishESP32/IoT5/giroscópioZ";
const char *MQTT_MONITORADOR_SONO_TEMP = "/publishESP32/IoT5/temp";
const char *MQTT_MONITORADOR_SONO_LUZ = "/publishESP32/IoT5/luz";
const char *MQTT_MONITORADOR_SONO_UMIDADE = "/publishESP32/IoT5/umidade";
const char *MQTT_MONITORADOR_SONO_CONTROL = "/publishESP32/IoT5/control";

unsigned long previousMillisSensor = 0;
unsigned long previuosMillisResponse = 0;
unsigned long previousMillislastI2CRequest = 0;
unsigned long previousMillislastMQTTPublish = 0;
float valorRecebido;
const int buttonPin = 4;
bool start = true;
bool lastButton = HIGH;
bool estadoBotao = HIGH;

unsigned long lastdebounceTime = 0;
const unsigned long debounceDelay = 50;
const int ledYellowPin = 19;

float ldrHist[10];
float lm35Hist[10];
float humiHist[10];
float giroXHist[10];
float giroYHist[10];
float giroZHist[10];
float gx_prev = 0, gy_prev = 0, gz_prev = 0;
int totalAmostrasTemp = 0;

int contSensor = 0;

unsigned long lastProcess = 0;
int contadorMovimentos = 0;

float LIMIAR = 200; // ajuste depois
float filtro = 0;

enum EstadoSono {
    SONO_PROFUNDO,
    SONO_LEVE,
    SONO_AGITADO
};

EstadoSono estadoSono = SONO_PROFUNDO;
EstadoSono novoEstado;

int brilho = 0;
int passo = 1;
unsigned long lastFade = 0;

const int BRILHO_MAX = 255;   
const int INTERVALO_FADE = 20; 

#define LED_RED_PIN 18
#define LED_RED_CHANNEL 0
#define LED_FREQ 5000
#define LED_RES 8 

void recebeFloat(){
  if (Wire.requestFrom(4, 4) == 4) {
    byte dados[4];
    for (int i = 0; i < 4; i++) {
        dados[i] = Wire.read();
    }
    memcpy(&valorRecebido, dados, 4);
	}
}

void resetFade() {
    brilho = 0;
    passo = 1;
}

void transmiteOpcao(int opcaotransmitida){
  Wire.beginTransmission(4);
  Wire.write(opcaotransmitida);
  Wire.endTransmission(); 
}

void ConnectToWiFi(){
  Serial.print("Connecting to WiFi ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSW);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nConnected to ");
  Serial.println(WIFI_SSID);
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String mensagem;

  for (int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }

  Serial.print("MQTT recebido: ");
  Serial.println(mensagem);

  if (String(topic) == MQTT_MONITORADOR_SONO_CONTROL) {

    if (mensagem == "TOGGLE") {
      start = !start;
    }
    else if (mensagem == "ON") {
      start = true;
    }
    else if (mensagem == "OFF") {
      start = false;
    }

    Serial.println(start ? "INICIADO (MQTT)" : "PARADO (MQTT)");
  }
}

void ConnectToMqtt(){
 Serial.println("Connecting to MQTT Broker...");
 while (!mqttClient.connected()){
  char clientId[100] = "\0";
  sprintf(clientId, "ESP32Client-dI8n9P1-", random(0xffff));   //variavel = clientID
  Serial.println(clientId);
  if (mqttClient.connect(clientId)){
    Serial.println("Connected to MQTT broker.");
    mqttClient.subscribe(MQTT_MONITORADOR_SONO_GIROX);
    mqttClient.subscribe(MQTT_MONITORADOR_SONO_GIROY);
    mqttClient.subscribe(MQTT_MONITORADOR_SONO_GIROZ);
    mqttClient.subscribe(MQTT_MONITORADOR_SONO_TEMP);
    mqttClient.subscribe(MQTT_MONITORADOR_SONO_LUZ);
    mqttClient.subscribe(MQTT_MONITORADOR_SONO_UMIDADE);
    mqttClient.subscribe(MQTT_MONITORADOR_SONO_CONTROL);
  }
 }
}

void sendAllData(int dadoAtual) {
    char buffer[32];
	dtostrf(giroXHist[dadoAtual], 1, 2, buffer);
    mqttClient.publish(MQTT_MONITORADOR_SONO_GIROX, buffer);
    dtostrf(giroYHist[dadoAtual], 1, 2, buffer);
    mqttClient.publish(MQTT_MONITORADOR_SONO_GIROY, buffer);
    dtostrf(giroZHist[dadoAtual], 1, 2, buffer);
    mqttClient.publish(MQTT_MONITORADOR_SONO_GIROZ, buffer);
    dtostrf(lm35Hist[dadoAtual], 1, 2, buffer);
    mqttClient.publish(MQTT_MONITORADOR_SONO_TEMP, buffer);
    dtostrf(ldrHist[dadoAtual], 1, 2, buffer);
    mqttClient.publish(MQTT_MONITORADOR_SONO_LUZ, buffer);
    dtostrf(humiHist[dadoAtual], 1, 2, buffer);
  	mqttClient.publish(MQTT_MONITORADOR_SONO_UMIDADE, buffer);
}


void SetupMqtt(){
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setCallback(mqttCallback);

}

void print_data()
{
  Serial.print("Giro em X: ");
  Serial.print(giroXHist[contSensor]);
  Serial.print("  Giro em Y: ");
  Serial.print(giroYHist[contSensor]);
  Serial.print("  Giro em Z: ");
  Serial.println(giroZHist[contSensor]);
  Serial.print("Temperatura: ");
  Serial.print(lm35Hist[contSensor]);
  Serial.print(" \xB0");
  Serial.print("C   Umidade: ");
  Serial.println(humiHist[contSensor]);
  Serial.print("Luminosidade: ");
  Serial.println(ldrHist[contSensor]);

}

void fadeSuave() {
    if (millis() - lastFade >= INTERVALO_FADE) {
        brilho += passo;

        if (brilho >= BRILHO_MAX) {
            brilho = BRILHO_MAX;
            passo = -1;
        } 
        else if (brilho <= 0) {
            brilho = 0;
            passo = 1;
        }

        ledcWrite(LED_RED_CHANNEL, brilho);
        lastFade = millis();
    }
}

void processarMovimento(float gx, float gy, float gz) {

    float movimento = sqrt(gx*gx + gy*gy + gz*gz);

    if (movimento < 2.0f) movimento = 0;

    filtro = 0.85f * filtro + 0.15f * movimento;

    if (filtro > LIMIAR) {
        contadorMovimentos++;
    }

    if (millis() - lastProcess >= 60000) {

        Serial.print("Movimentos no minuto: ");
        Serial.println(contadorMovimentos);

        if (contadorMovimentos < 10) {
            novoEstado = SONO_PROFUNDO;
        }
        else if (contadorMovimentos < 20) {
            novoEstado = SONO_LEVE;
        }
        else {
            novoEstado = SONO_AGITADO;
        }

        if (novoEstado != estadoSono) {
            resetFade();
        }

        estadoSono = novoEstado;
        contadorMovimentos = 0;
        lastProcess = millis();
    }
}


float mediaTemp() {
    float soma = 0;
    for (int i = 0; i < 10; i++) soma += lm35Hist[i];
    return soma / 10.0;
}

void processarTempeatura()
{
  if(lm35Hist[contSensor] >= 17.0 && lm35Hist[contSensor] <= 20.0)
  {
    Serial.println("Temperatura ideal");
  }
  else if(lm35Hist[contSensor] < 14.0 || lm35Hist[contSensor] > 24.0)
  {
    Serial.println("Temperatura ruim");
  }
  else if((lm35Hist[contSensor] >= 14.0 && lm35Hist[contSensor] <= 17.0) ||(lm35Hist[contSensor] >= 20.0 && lm35Hist[contSensor] <= 24.0) )
  {
    Serial.println("Temperatura boa");
  }
  float atual = lm35Hist[contSensor];

  if (totalAmostrasTemp >= 10) {
    float media = mediaTemp();
    float delta = fabs(atual - media);

    if (delta >= 5.0) {
      Serial.println("Temperatura variou muito");
    }
  }

  if(humiHist[contSensor] >= 70.0 || humiHist[contSensor] <= 30.0)
  {
    Serial.println("Umidade péssima");
  }
}

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledYellowPin, OUTPUT);
  ledcSetup(LED_RED_CHANNEL, LED_FREQ, LED_RES);
  ledcAttachPin(LED_RED_PIN, LED_RED_CHANNEL);
  ConnectToWiFi();
  SetupMqtt();
}

void loop()
{
  if (!mqttClient.connected()){
      ConnectToMqtt();
  }
  mqttClient.loop();
  
  bool leitura = digitalRead(buttonPin);
  if (leitura != lastButton) {
    lastdebounceTime = millis();
  }

  if ((millis() - lastdebounceTime) > debounceDelay) {
      if (leitura != estadoBotao) {
          estadoBotao = leitura;
          if (estadoBotao == LOW) {
              start = !start;
              Serial.println(start ? "INICIADO" : "PARADO");
          }
      }
  }

  lastButton = leitura;
 
  if(start){
   
    digitalWrite(ledYellowPin, HIGH);
    
    switch(estadoSono) {

      case SONO_PROFUNDO:
        ledcWrite(LED_RED_CHANNEL, 0);
        break;
      case SONO_LEVE:
        fadeSuave();
        break;
      case SONO_AGITADO:
        ledcWrite(LED_RED_CHANNEL, 255);
        break;
      default:
        estadoSono = SONO_PROFUNDO;  
    }


    
    if(millis() - previousMillisSensor >= 3000)
    {
      previousMillisSensor = millis();
      transmiteOpcao(1);
      delayMicroseconds(500);
      recebeFloat();
      ldrHist[contSensor] = valorRecebido;
      transmiteOpcao(2);
      delayMicroseconds(500);
      recebeFloat();
      lm35Hist[contSensor] = valorRecebido;
      if (totalAmostrasTemp < 10) {
    		totalAmostrasTemp++;
	  }
      transmiteOpcao(3);
      delayMicroseconds(500);
      recebeFloat();
      humiHist[contSensor] = valorRecebido;
      transmiteOpcao(4);
      delayMicroseconds(500);
      recebeFloat();
      giroXHist[contSensor] = valorRecebido;
      transmiteOpcao(5);
      delayMicroseconds(500);
      recebeFloat();
      giroYHist[contSensor] = valorRecebido;
      transmiteOpcao(6);
      delayMicroseconds(500);
      recebeFloat();
      giroZHist[contSensor] = valorRecebido;

      print_data();
      processarTempeatura();
      processarMovimento(giroXHist[contSensor], giroYHist[contSensor] , giroZHist[contSensor]);

      contSensor = (contSensor + 1) % 10;



    }

    if(millis() - previuosMillisResponse >= 1000)
    {
      int idxPub = (contSensor == 0) ? 9 : contSensor - 1;

      sendAllData(idxPub);

      previuosMillisResponse = millis();
    }
  }
  else
  {
      digitalWrite(ledYellowPin, LOW);
  }
}
