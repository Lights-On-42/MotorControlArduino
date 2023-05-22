#include <Arduino.h>
#include "Comands.h"
#include "MotorBase.h"

#include "Data.h"
#include "Wifi.h"
#include "Web.h"


int counterserial=0;
Comands orderComands;
MotorBase motor1;
MotorBase motor2;
NewComand command;
String commandText="";

void setup()
{

  Serial.begin(115200);
  Serial.println("");
  Serial.println("Neu start System ");

  command.whatComand=0;
  
  motor1.Initializieren(1, D2,D1, 1);
  motor2.Initializieren(2, D3,D4,-1);

  orderComands.Initializieren(D0);

  orderComands.AddMotorModus(&motor1.ModusDesMotors);
  orderComands.AddMotorModus(&motor2.ModusDesMotors);

  Wifi::login();
  Web::startHTTP();
  Web::startWS();


  Serial.println("Setup finished");

}

void loop()
{
  counterserial++;
  if(counterserial==25)
  {
    counterserial=0;
    if (Serial.available() > 0)
    {
      commandText=Serial.readString();
      Serial.println(commandText);
      commandText.replace("\n","");       
    }
    Web::tick(&commandText);
    orderComands.AnalyseOrder(commandText,&command);

    commandText="";
    motor1.CheckComand(&command);
    motor2.CheckComand(&command);

    command.whatComand=0;
    orderComands.CheckForAutoPowerON();
  }

  motor1.Tick();
  motor2.Tick();

  orderComands.Tick();

  delayMicroseconds(1);
}