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
MotorBase motor3;
MotorBase motor4;
CommandGCode command;
String commandText="";

void setup()
{
  //ini alle pins auf 0
  orderComands.initializePin(16); //D0
  orderComands.initializePin(5);  //D1
  orderComands.initializePin(4);  //D2
  orderComands.initializePin(0);  //D3

  orderComands.initializePin(2);  //D4
  orderComands.initializePin(14); //D5
  orderComands.initializePin(12); //D6
  orderComands.initializePin(13); //D7
  orderComands.initializePin(15); //D8

  Serial.begin(115200);
  Serial.println("");
  Serial.println("Neu start System ");

  //alter motor pinDirection, uint16_t pinSteps,
  motor1.Initializieren(1, 2,0, -1);
  motor2.Initializieren(2, 5,4, 1);
  motor3.Initializieren(3, 15,13,1);
  motor4.Initializieren(4, 14,12,-1);


  orderComands.initializeSleepPin(16);

  orderComands.AddMotorModus(&motor1.ModusDesMotors);
  orderComands.AddMotorModus(&motor2.ModusDesMotors);
  orderComands.AddMotorModus(&motor3.ModusDesMotors);
  orderComands.AddMotorModus(&motor4.ModusDesMotors);

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
      commandText.replace("\n","");
      Serial.println(commandText);  
    }
    Web::tick(&commandText);
    orderComands.AnalyseOrder(commandText,&command);

    commandText="";
    motor1.CheckComand(&command);
    motor2.CheckComand(&command);
    motor3.CheckComand(&command);
    motor4.CheckComand(&command);

    if(command.Commands.size()!=0)
    {
      Serial.print(" count ");
      Serial.println(command.Commands.size());
      command.Commands.clear();
    }
    //command.whatComand=0;
    orderComands.CheckForAutoPowerON();
  }

  motor1.Tick();
  motor2.Tick();
  motor3.Tick();
  motor4.Tick();

  orderComands.Tick();

  delayMicroseconds(1);
}