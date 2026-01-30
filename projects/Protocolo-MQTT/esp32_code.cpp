// esp32

#include <Wire.h>

// C++ code
//

#include <PubSubClient.h>
#include <WiFi.h>
const char *WIFI_SSID = "AndroidAP54EA";  //mudar
const char *MQTT_SERVER = "broker.hivemq.com";  //mudar
const char *WIFI_PASSW = "12345678";    //mudar
//const uint16_t MQTT_PORT = 8884;
const uint16_t MQTT_PORT = 1883;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

//publishESP32/ICT-UNIFESP/IoT5/PedidoGrupo1    //eles mandam d e f
//publishESP32/ICT-UNIFESP/IoT5/RespostaGrupo1  //nós recebemos os valores de d e f
//subscribeESP32/ICT-UNIFESP/IoT5/PedidoGrupo2  //a gente mandam d e f
//subscribeESP32/ICT-UNIFESP/IoT5/RespostaGrupo2  //eles recebem nossos valores de d e f

const char *MQTT_PUBLISH_PEDIDO_GRUPO1 = "/publishESP32/ICT-UNIFESP/IoT5/PedidoGrupo1";
const char *MQTT_PUBLISH_RESPOSTA_GRUPO1 = "/publishESP32/ICT-UNIFESP/IoT5/RespostaGrupo1";
const char *MQTT_SUBSCRIBE_PEDIDO_GRUPO2 = "/subscribeESP32/ICT-UNIFESP/IoT5/PedidoGrupo2";
const char *MQTT_SUBSCRIBE_RESPOSTA_GRUPO2 = "/subscribeESP32/ICT-UNIFESP/IoT5/RespostaGrupo2";

unsigned long previousMillisMenu = 0;
unsigned long previuosMillisResponse = 0;
bool grupo2 = false;
char opcao;
const long interval1 = 100;
float valorRecebido;
int opcaotransmitida = 0;

void recebeFloat(){
  Wire.requestFrom(8, 4);
  byte dados[4];
  int i = 0;
  while (Wire.available() && i < 4) {
    dados[i++] = Wire.read();
  }
  memcpy(&valorRecebido, dados, 4);
}

void transmiteOpcao(int opcaotransmitida){
  Wire.beginTransmission(8);
  Wire.write(opcaotransmitida);
  Wire.endTransmission();
  
  previuosMillisResponse = millis();
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

void ConnectToMqtt(){
 Serial.println("Connecting to MQTT Broker...");
 while (!mqttClient.connected()){
  char clientId[100] = "\0";
  sprintf(clientId, "ESP32Client-", random(0xffff));   //variavel = clientID
  Serial.println(clientId);
  if (mqttClient.connect(clientId)){
    Serial.println("Connected to MQTT broker."); 
		mqttClient.subscribe(MQTT_PUBLISH_PEDIDO_GRUPO1);
    mqttClient.subscribe(MQTT_PUBLISH_RESPOSTA_GRUPO1);
  }
 }
}

void CallbackMqtt(char* topic, byte* payload, unsigned int length){
    
    String topicoStr = String(topic);


    //  1 Se for comando ('d','e','f') 
    if (topicoStr == MQTT_PUBLISH_PEDIDO_GRUPO1) {
        
        char comando = (char)payload[0];
        Serial.print("Comando recebido via MQTT: ");
        Serial.println(comando);

        if (comando == 'd') opcaotransmitida = 1;
        else if (comando == 'e') opcaotransmitida = 2;
        else if (comando == 'f') opcaotransmitida = 3;
        else return;

        transmiteOpcao(opcaotransmitida);
        grupo2 = true;
        return;
    }

    //  2 Se for RESPOSTA do outro grupo float 
    if (topicoStr == MQTT_PUBLISH_RESPOSTA_GRUPO1) {

        char buf[20];
        memcpy(buf, payload, length);
        buf[length] = '\0';

        float valor = atof(buf);
        
        if(opcaotransmitida == 4){
          Serial.print("Distância: ");
          Serial.println(valor, 3);
        } else if(opcaotransmitida == 5){
          Serial.print("Tempeatura: ");
          Serial.println(valor, 3);
        }

        return;
    }
}



void SetupMqtt(){
 mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
 mqttClient.setCallback(CallbackMqtt);
}

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  Serial.println("Menu: ");
  Serial.println("a) Distância Grupo 1");
  Serial.println("b) Temperatura Grupo 1");
  Serial.println("c) LED Grupo 1");
  Serial.println("d) Distância Grupo 2");
  Serial.println("e) Temperatura Grupo 2");
  Serial.println("f) LED Grupo 2");
  ConnectToWiFi();
  SetupMqtt();
}

void loop()
{
   if (!mqttClient.connected()){
      ConnectToMqtt();
   }
   mqttClient.loop();
  if (Serial.available()) {
    opcao = Serial.read();
    if(opcao == 'a'){
      Serial.println("opcao 1");
      opcaotransmitida = 1;
      transmiteOpcao(opcaotransmitida);
    }else if(opcao == 'b'){
      Serial.println("opcao 2");
      opcaotransmitida = 2;
      transmiteOpcao(opcaotransmitida);
    }else if(opcao == 'c'){
      Serial.println("opcao 3");
      opcaotransmitida = 3;
      transmiteOpcao(opcaotransmitida);
    }else if(opcao == 'd'){
      Serial.println("opcao 4");
      mqttClient.publish(MQTT_SUBSCRIBE_PEDIDO_GRUPO2, "d");
      opcaotransmitida = 4;
    }else if(opcao == 'e'){
      Serial.println("opcao 5");
      mqttClient.publish(MQTT_SUBSCRIBE_PEDIDO_GRUPO2, "e");
      opcaotransmitida = 5;
    }else if(opcao == 'f'){
      Serial.println("opcao 6");
      mqttClient.publish(MQTT_SUBSCRIBE_PEDIDO_GRUPO2, "f");
    }
 }
 
  
  if((opcaotransmitida == 1 || opcaotransmitida == 2) && millis() - previuosMillisResponse >= 100)
  {
    recebeFloat();
    
      if(opcaotransmitida == 1)
      {
      	Serial.print("Ditância: ");
      	Serial.println(valorRecebido, 3);
        if(grupo2 == true)
        {
          char msg[20];
          dtostrf(valorRecebido, 1, 3, msg);
          mqttClient.publish(MQTT_SUBSCRIBE_RESPOSTA_GRUPO2, msg);
          grupo2 = false;
        }
      }else if(opcaotransmitida == 2)
      {
      	Serial.print("Temperatura: ");
      	Serial.println(valorRecebido, 3);
         if(grupo2 == true)
    	 {
      		char msg[20];
      		dtostrf(valorRecebido, 1, 3, msg);
      		mqttClient.publish(MQTT_SUBSCRIBE_RESPOSTA_GRUPO2, msg);
      		grupo2 = false;
    	 }
       }
    opcaotransmitida = -1;
  }
}