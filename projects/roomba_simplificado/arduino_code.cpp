// C++ code

const long interval1 = 100;
const long interval2 = 50;
bool ledState = LOW;
bool ledState1 = LOW;
unsigned long previousMillisBlinkled1 = 0;
unsigned long previousMillisBlinkled2 = 0;

void idle(){
    digitalWrite(13, LOW);
    digitalWrite(12, LOW);
}
void cleaning(){
  if (millis() - previousMillisBlinkled1 >= interval1)
  {
              previousMillisBlinkled1 = millis();
          	  Serial.println(ledState);
    		  Serial.println(millis());
              ledState = !ledState;
    		  digitalWrite(13, ledState);
   }
}
void docking(){
  digitalWrite(13, LOW);
  if(millis() - previousMillisBlinkled2 >= interval2)
        {
          previousMillisBlinkled2 = millis(); 
          ledState1 = !ledState1;
    	  digitalWrite(12, ledState1);
        }
}
void charging(){
  if(millis() - previousMillisBlinkled1 >= interval2)
        {
          previousMillisBlinkled1 = millis(); 
          ledState = !ledState; 
          digitalWrite(12, ledState);
        }
      if(millis() - previousMillisBlinkled2 >= interval1)
        {
          previousMillisBlinkled2 = millis(); 
          ledState1 = !ledState1; 
          digitalWrite(13, ledState1);
        }
}
void setup() {
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  Serial.begin(115200);
  pinMode(11, INPUT);
  pinMode(10, INPUT);
}

void loop() {
  int i, j;
  i = digitalRead(11);
  j = digitalRead(10);

 
  if(i == 0 && j ==0) idle();
  else if(i == 0 && j ==1) cleaning();
  else if(i == 1 && j ==0) docking();
  else if(i == 1 && j ==1) charging();  
}