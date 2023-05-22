struct NewComand
{
  int whatComand;
  int motorNumber;
  int motorNumber2;

  int motorTarget;
  int motorSpeed;
  int motorSpeed2;
};

// testmode
// testpin-4,1;
// testpin-5,1;
// testto-6,200,500;
// newspeed-1,200;
class Comands
{
    public:
      void AnalyseOrder(String Order,NewComand* com);
      void Tick();
      void CheckForAutoPowerON();
      void Initializieren(uint16_t indexSleepPin);
      void AddMotorModus(uint16_t* modusNewMotor);

    private:
      int tooglePIN=-1;
      long activeSteeps=0;
      int steepsMax=0;
      bool lastStatus=LOW;
      int counterIntervall=0;
      uint16_t interval_us = 1;

      //autoPower on
      bool testMode=false;
      bool lastStatusOfAutoPower=false;

      uint16_t sleepPin = 0;
      std::vector<uint16_t*> motorTreiberModus;
};

void Comands::Initializieren(uint16_t indexSleepPin) 
{
  sleepPin=indexSleepPin;
  pinMode(sleepPin,OUTPUT);
  digitalWrite(sleepPin, LOW);
}

void Comands::AddMotorModus(uint16_t* modusNewMotor) 
{
  motorTreiberModus.push_back(modusNewMotor);
}

void Comands::Tick()
{
  if(tooglePIN!=-1)
  {
    counterIntervall++;
    if(counterIntervall==interval_us)
    {
      counterIntervall=0;
      digitalWrite(tooglePIN, lastStatus);
      activeSteeps++;
      if(lastStatus==LOW)
      {
        lastStatus=HIGH;
      }
      else
      {
        lastStatus=LOW;
      }
      if(steepsMax!=-1)
      {
        if(activeSteeps>steepsMax)
        {
          tooglePIN=-1;
          activeSteeps=0;
          Serial.println("Test Toggle Pin Finshed");
        }
      }
    }
  }
}

void Comands::CheckForAutoPowerON()
{
  
  if(testMode==false)
  {
    bool sleepOn=false;
    for (uint16_t* n : motorTreiberModus)
    {
      if(*n!=0)
      {
        sleepOn=true;
      }
    }
    if(sleepOn==false)
    {
      if(lastStatusOfAutoPower==false)
      {
        //nix zu tun ist alles schon so
      }
      else
      {
        lastStatusOfAutoPower=false;
        Serial.println("Auto Power off");
        digitalWrite(sleepPin, LOW);
      }
    }
    else
    {
      if(lastStatusOfAutoPower==true)
      {
        //nix zu tun ist alles schon so
      }
      else
      {
        lastStatusOfAutoPower=true;
        Serial.println("Auto Power on");
        digitalWrite(sleepPin, HIGH);
        
      }
    }
  }
}
void Comands::AnalyseOrder(String Order,NewComand* com)
{
  if(Order.length()==0)
  {
    return;
  }
  //Serial.print("I received: ");
  Serial.println(Order);
  if(Order.indexOf("testmode") == 0)
  {
    Serial.print("Testmode online");
    testMode=true;
  }
  if(Order.indexOf("testpin") == 0)
  {
    //seperate nachricht
    // "Comando"-"pinnumber","0oder1";
    int cmdPos = Order.indexOf("-");
    int pinNummerPos = Order.indexOf(",");
  
    String cmdText = Order.substring(0,cmdPos);
    String pinNummerText= Order.substring(cmdPos + 1, pinNummerPos);
    String zustandsText = Order.substring(pinNummerPos + 1, Order.length());
    
    int pinNummer = pinNummerText.toInt();
    int highorlow = zustandsText.toInt();
    Serial.println(pinNummer);
    Serial.println(highorlow);
    if(highorlow==0)
    {
      pinMode(pinNummer, OUTPUT);
      digitalWrite(pinNummer, LOW);
    }
    if(highorlow==1)
    {
      pinMode(pinNummer, OUTPUT);
      digitalWrite(pinNummer, HIGH);
    }
    
  }
  if(Order.indexOf("testto") == 0)
  {
    //seperate nachricht
    // "Comando"-"pinnumber","speed","anzahlsteps";
    int cmdPos = Order.indexOf("-");
    int pinNummerPos = Order.indexOf(",");
    int speedNummerPos = Order.indexOf(",",pinNummerPos+1);
  
    String cmdText = Order.substring(0,cmdPos);
    String pinNummerText= Order.substring(cmdPos + 1, pinNummerPos);
    String speedText= Order.substring(pinNummerPos + 1, speedNummerPos);
    String steepsText = Order.substring(speedNummerPos + 1, Order.length()-1);
    
    int pinNummer = pinNummerText.toInt();
    int speedTimer = speedText.toInt();        
    int steepsNumber = steepsText.toInt();
    Serial.println(pinNummerText);
    Serial.println(speedTimer);
    Serial.println(steepsNumber);
    tooglePIN=pinNummer;
    interval_us=speedTimer;    
    steepsMax= steepsNumber;
  }
  //Motorfahren
  if(Order.indexOf("to") == 0)
  {
    //seperate nachricht
    // "Comando"-"motorNumber","motorTarget";
    int cmdPos = Order.indexOf("-");
    int pinMotorPos = Order.indexOf(",");
  
    String pinMotorText= Order.substring(cmdPos + 1, pinMotorPos);
    String targetText = Order.substring(pinMotorPos + 1, Order.length()-1);

    com->whatComand=1;
    com->motorNumber=pinMotorText.toInt();
    com->motorTarget = targetText.toInt(); 
  }

  if(Order.indexOf("newspeed") == 0)
  {
    //seperate nachricht
    // "Comando"-"motorNumber","motorSpeed";
    int cmdPos = Order.indexOf("-");
    int pinMotorPos = Order.indexOf(",");
  
    String pinMotorText= Order.substring(cmdPos + 1, pinMotorPos);
    String speedText = Order.substring(pinMotorPos + 1, Order.length()-1);

    com->whatComand=2;
    com->motorNumber=pinMotorText.toInt();
    com->motorSpeed = speedText.toInt(); 
  }

  if(Order.indexOf("stop") == 0)
  {
    //seperate nachricht
    // "Comando"-"motorNumber";
    int cmdPos = Order.indexOf("-");
  
    String pinMotorText= Order.substring(cmdPos + 1, Order.length()-1);

    com->whatComand=3;
    com->motorNumber=pinMotorText.toInt();
  }

  if(Order.indexOf("getPos") == 0)
  {
    //seperate nachricht
    // "Comando"-"motorNumber";
    int cmdPos = Order.indexOf("-");
  
    String pinMotorText= Order.substring(cmdPos + 1, Order.length()-1);

    com->whatComand=4;
    com->motorNumber=pinMotorText.toInt();
  }

  if(Order.indexOf("syn") == 0)
  {
    //seperate nachricht
    // "Comando"-"motorNumber"-"motorNumber2","motorTarget";
    int cmdPos = Order.indexOf("-");
    int cmdPos2 = Order.indexOf("-",cmdPos+1);
    int pinMotorPos = Order.indexOf(",");
  
    //String cmdText = Order.substring(0,cmdPos);
    String pinMotorText= Order.substring(cmdPos + 1, cmdPos2);
    String pinMotorText2= Order.substring(cmdPos2 + 1, pinMotorPos);
    String targetText = Order.substring(pinMotorPos + 1, Order.length()-1);

    com->whatComand=1;
    com->motorNumber=pinMotorText.toInt();
    com->motorNumber2=pinMotorText2.toInt();
    com->motorTarget = targetText.toInt(); 
  }

  if(Order.indexOf("synnewspeed") == 0)
  {
    //seperate nachricht
    // "Comando"-"motorNumber,motorNumber2,motorSpeed1,motorSpeed2";
    int pos1 = Order.indexOf("-");
    int pos2 = Order.indexOf(",",pos1+1);
    int posMotorSpeed1 = Order.indexOf(",",pos2+1);
    int posMotorSpeed2 = Order.indexOf(",",posMotorSpeed1+1);
  


    String motorText1= Order.substring(pos1 + 1, pos2);
    String motorText2= Order.substring(pos2 + 1, posMotorSpeed1);
    String speedText1 = Order.substring(posMotorSpeed1 + 1, posMotorSpeed2);
    String speedText2 = Order.substring(posMotorSpeed2 + 1, Order.length());

    com->whatComand=2;
    com->motorNumber=motorText1.toInt();
    com->motorNumber2=motorText2.toInt();
    com->motorSpeed = speedText1.toInt();
    com->motorSpeed2 = speedText2.toInt(); 
  }

}
