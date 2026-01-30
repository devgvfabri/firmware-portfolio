// C++ code
//
//00 = iddle
//01 = cleaning
//10 = docking
//11 = charching

enum State{
  IDLE,
    CLEANING,
    DOCKING,
    CHARGING
};
State CurrentState = IDLE;
unsigned long previousMillisTotal = 0;
void setup()
{
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  Serial.begin(115200);
}

void loop()
{
  switch(CurrentState)
  {
    case IDLE:
      digitalWrite(13, LOW);
      digitalWrite(12, LOW);
        if(Serial.available())
          {
            char a =  Serial.read();
            if(a == 'a')
            {
               Serial.println("Idle");
               CurrentState = CLEANING;
               digitalWrite(13, LOW);
          	   digitalWrite(12, HIGH);
               previousMillisTotal = millis();
            }
          }
      break;
    case CLEANING:
          if(Serial.available())
          {
            char a =  Serial.read();
            if(a == 'b')
            {
               CurrentState = DOCKING;
               previousMillisTotal = millis();
               digitalWrite(13, HIGH);
	  		   digitalWrite(12, LOW);
               break;
            }
          }
         if (millis() - previousMillisTotal >= 2000)
         {
            previousMillisTotal = millis(); 
            CurrentState = DOCKING;
            digitalWrite(13, HIGH);
	  		digitalWrite(12, LOW);
         }
        break;
    case DOCKING:
      if (millis() - previousMillisTotal >= 2000)
         {
        	Serial.println("Docking");
            previousMillisTotal = millis(); 
            CurrentState = CHARGING;
            digitalWrite(13, HIGH);
      		digitalWrite(12, HIGH);
         }
      break;
    case CHARGING:
      if(Serial.available())
          {
            char a =  Serial.read();
            if(a == 'a')
            {
               previousMillisTotal = millis();
               CurrentState = CLEANING;
               digitalWrite(13, LOW);
          	   digitalWrite(12, HIGH);
               break;
            }
          }
      if (millis() - previousMillisTotal >= 2000)
         {
            previousMillisTotal = millis(); 
            CurrentState = IDLE;
         }
      break;
    default:
      Serial.println("Estado Inválido!");
      CurrentState = IDLE;
      break;
  }
}