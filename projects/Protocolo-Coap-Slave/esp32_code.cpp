// esp32

#include <Wire.h>

#include <WiFi.h>
#include <WiFiUDP.h>
#include <coap-simple.h>

#include <PubSubClient.h>
//const char *WIFI_SSID = "AndroidAP54EA";  
//const char *WIFI_PASSW = "12345678";    
//const char *WIFI_SSID = "moto g84 5G_5388";
//const char *WIFI_PASSW = "fcnb64axjefe8a9";
//const char *WIFI_SSID = "Странная лягушка";
//const char *WIFI_PASSW = "putini00";
WiFiClient wifiClient;

char opcao;
const long interval1 = 100;
float valorRecebido = 0.0;
int opcaotransmitida = 0;

// --- Coap things ---

WiFiUDP udp;
Coap coap(udp);

// cliente não usa o endpoint light
/*
// CoAP server endpoint URL
void callback_light(CoapPacket &packet, IPAddress ip, int port) {
  char clientPayload[packet.payloadlen + 1];
  memcpy(clientPayload, packet.payload, packet.payloadlen);
  clientPayload[packet.payloadlen] = NULL;

  String message(clientPayload);
  Serial.println("Received:light");

  switch(packet.code){
      case COAP_GET:  
      //VERIFICAR -> Se da para mandar em float ou se necessita transformar para String
        if(message == 'a'){  //temp
         transmiteOpcao(1);
         recebeFloat();
         char buffer[16];
		 dtostrf(valorRecebido, 1, 4, buffer);
		 coap.sendResponse(ip, port, packet.messageid, buffer);
        } else if(message == 'b'){  //temp
         transmiteOpcao(2);
         recebeFloat();
         char buffer[16];
		 dtostrf(valorRecebido, 1, 4, buffer);
		 coap.sendResponse(ip, port, packet.messageid, buffer);
        } else if(message == 'c'){  //temp
         transmiteOpcao(3);
         recebeFloat();
		 char buffer[16];
		 dtostrf(valorRecebido, 1, 4, buffer);
		 coap.sendResponse(ip, port, packet.messageid, buffer);        }
      break;
      
      case COAP_PUT:// acho que não precisa de PUT
          coap.sendResponse(ip, port, packet.messageid, "1");        
      break;
      
      default:         
          coap.sendResponse(ip, port, packet.messageid, "Not Supported");
      break;
  }
  
}
*/
void endpoint_dist(CoapPacket &packet, IPAddress ip, int port) {
  Serial.println("----Temp-----");
  Serial.println("PacketPayloadlen");
  Serial.println(packet.payloadlen);
  Serial.println("Packet.messageID");
  Serial.println(packet.messageid);
  Serial.println("Packet");
  Serial.println(packet);
  Serial.println("Ip");
  Serial.println(ip);
  Serial.println("Port");
  Serial.println(port);

  transmiteOpcao(1);         
  recebeFloat();
  Serial.println(valorRecebido);
  char buffer[16];
  dtostrf(valorRecebido, 1, 4, buffer);
  coap.sendResponse(ip, port, packet.messageid, buffer);
}

void endpoint_temp(CoapPacket &packet, IPAddress ip, int port) {
  Serial.println("----Temp-----");
  Serial.println("PacketPayloadlen");
  Serial.println(packet.payloadlen);
  Serial.println("Packet.messageID");
  Serial.println(packet.messageid);
  Serial.println("Packet");
  Serial.println(packet);
  Serial.println("Ip");
  Serial.println(ip);
  Serial.println("Port");
  Serial.println(port);


  transmiteOpcao(2);         
  recebeFloat();
  Serial.println(valorRecebido);
  char buffer[16];
  dtostrf(valorRecebido, 1, 4, buffer);
  coap.sendResponse(ip, port, packet.messageid, buffer);
}

void endpoint_led(CoapPacket &packet, IPAddress ip, int port) {
  Serial.println("----LED-----");
  Serial.println("PacketPayloadlen");
  Serial.println(packet.payloadlen);
  Serial.println("Packet");
  Serial.println(packet);
  Serial.println("Ip");
  Serial.println(ip);
  Serial.println("Port");
  Serial.println(port);

  char msg[packet.payloadlen + 1];
  memcpy(msg, packet.payload, packet.payloadlen);
  msg[packet.payloadlen] = 0;

  Serial.println(msg[0]);

  if (msg[0] == '1') {
    digitalWrite(2, HIGH); 
  } else {
    digitalWrite(2, LOW);
  }

  coap.sendResponse(ip, port, packet.messageid, "OK");
}


// --- Others ---

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

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  Serial.println("Menu: ");
  Serial.println("a) Distância Grupo 1");
  Serial.println("b) Temperatura Grupo 1");
  Serial.println("c) LED Grupo 1");
  ConnectToWiFi();

  //coap.server = endpoints do server
  //coap.server(callback_light, "light"); cliente não usa o endpoint light
  coap.server(endpoint_dist, "dist");
  coap.server(endpoint_temp, "temp");
  coap.server(endpoint_led,  "led_server");
  coap.start();
}

void loop()
{
  coap.loop();
}
  
  