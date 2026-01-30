// esp32

#include <Wire.h>

// C++ code
//

unsigned long previousMillisMenu = 0;
unsigned long previuosMillisResponse = 0;
bool response = false;
char opcao;
const long interval1 = 100;
float valorRecebido;
int opcaotransmitida = 0;

void recebeFloat(){
  //Serial.println("Entrando recebeFloat()");
	Wire.requestFrom(8, 5);
  byte dados[5];
  int i = 0;
  while (Wire.available() && i < 5) {
    dados[i++] = Wire.read();
  }
  memcpy(&valorRecebido, dados, 5);
  //Serial.println(valorRecebido);
}

void transmiteOpcao(int opcaotransmitida){
  Wire.beginTransmission(8);
  Wire.write(opcaotransmitida);
  Wire.endTransmission();
  
  previuosMillisResponse = millis();
  response = true;
}

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  Serial.println("Menu: ");
  Serial.println("a) Temperatura");
  Serial.println("b) Umidade");
  Serial.println("c) Distância");
}

void loop()
{
  if (Serial.available()) {
    opcao = Serial.read();
    //Serial.print(opcao);
    if(opcao == 'a'){
      Serial.println("opcao 1");
      opcaotransmitida = 1;
    }else if(opcao == 'b'){
      Serial.println("opcao 2");
      opcaotransmitida = 2;
    }else if(opcao == 'c'){
      Serial.println("opcao 3");
      opcaotransmitida = 3;
    }
    
    if(opcaotransmitida > 0){
      //Serial.print("opcao transmitida = ");
      //Serial.print(opcaotransmitida);
    	transmiteOpcao(opcaotransmitida);
      //opcaotransmitida = 0;
    }
  }
  
  
  if(response && millis() - previuosMillisResponse >= 10){
  	response = false;
    recebeFloat();
    //Serial.println(opcao);
    //Serial.println(opcaotransmitida);
    if(opcaotransmitida == 1){
      Serial.print("Temperatura: ");
      Serial.println(valorRecebido, 3);
      opcaotransmitida = 0;
    }else if(opcaotransmitida == 2){
      Serial.print("Umidade: ");
      Serial.println(valorRecebido, 3);
      opcaotransmitida = 0;
    }else if(opcaotransmitida == 3){
      Serial.print("Ditância: ");
      Serial.println(valorRecebido, 3);
      opcaotransmitida = 0;
    }
  }
}