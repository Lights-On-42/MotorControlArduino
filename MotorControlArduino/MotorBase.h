//#include "Comands.h"

class MotorBase {
public:
  void Tick();
  void CheckComand(NewComand* com);
  void Initializieren(uint16_t index, uint16_t pinDirection, uint16_t pinSteps,int multiplication);
  void InitializierenWithEnable(uint16_t index, uint16_t pinDirection, uint16_t pinSteps,uint16_t pinEnable, int multiplication);
  void Stop();
  int getPosition();

  uint16_t ModusDesMotors = 0;
  //Modus in dem der Motor gerade fährt
  //0 Motor steht
  //1 Motor bewegt sich im Test Modus
  //2 Motor bewegt sich auf eine Position
  //3 Motor bewegt sich dauerhaft

private:
  uint16_t motorIndex = 0;
  uint16_t motorPinDirection = 0;
  uint16_t motorPinSteps = 0;
  int motorMultiplication=1;

  bool lastStatus = LOW;



  int counterIntervall = 0;
  int motorSpeed = 200;    //Geschwindikeit
  int newMotorSpeed = 0;  //fals eine neue Geschwindigkeit runter kommet

  int targetCounter = 0;  //ziel wo der Motor hinfahren soll
  bool forward = false;   //true vorwärts false ruckwärts
  int stepCounter = 0;    //Anzahl an vergangen Schritte hierraus kann man die Position berechnen
};

void MotorBase::Initializieren(uint16_t index, uint16_t pinDirection, uint16_t pinSteps,int multiplication) 
{
  motorIndex = index;
  motorPinDirection = pinDirection;
  motorPinSteps = pinSteps;
  motorMultiplication=multiplication;

  pinMode(motorPinDirection, OUTPUT);
  pinMode(motorPinSteps, OUTPUT);
}

void MotorBase::InitializierenWithEnable(uint16_t index, uint16_t pinDirection, uint16_t pinSteps,uint16_t pinEnable, int multiplication) 
{
  motorIndex = index;
  motorPinDirection = pinDirection;
  motorPinSteps = pinSteps;
  motorMultiplication=multiplication;

  pinMode(motorPinDirection, OUTPUT);
  pinMode(motorPinSteps, OUTPUT);

  pinMode(pinEnable, OUTPUT);
  digitalWrite(pinEnable, LOW);
}

void MotorBase::CheckComand(NewComand* com) 
{
  int speedzw = 0;
  if (com->whatComand != 0)  //es ist ein Beweg dich comando
  {
    if (com->whatComand == 3) 
    {
      ModusDesMotors = 0;
      Serial.print("Motor Stop  ");
      Serial.println(motorIndex);
    } 
    else if (com->motorNumber == motorIndex || com->motorNumber2 == motorIndex)  //der befehl geht zu meinem motor
    {
      switch (com->whatComand) 
      {
        case 1:
          ModusDesMotors = 2;
          motorSpeed = 200;
          counterIntervall = 0;
          targetCounter = targetCounter + com->motorTarget;
          if (targetCounter > stepCounter) 
          {
            forward = true;
            digitalWrite(motorPinDirection, true);
          } 
          else if (targetCounter < stepCounter) 
          {
            forward = false;
            digitalWrite(motorPinDirection, false);
          } 
          else
          {
            Serial.println("Motor am ziel");
          }
          break;
        case 2:
          ModusDesMotors = 3;
          counterIntervall = 0;

          if (com->motorNumber == motorIndex) 
          {
            speedzw = motorMultiplication * com->motorSpeed;
          }
          if (com->motorNumber2 == motorIndex) 
          {
            speedzw = motorMultiplication * com->motorSpeed2;
          }
          if (speedzw == 0) 
          {
            ModusDesMotors = 0;
            Serial.print("Motor Stop  by speed 0");
            break;
          }
          if (speedzw < 0) 
          {
            digitalWrite(motorPinDirection, true);
            speedzw = speedzw * -1;
            Serial.print("backwarts ");
          } 
          else 
          {
            Serial.print("forward ");
            digitalWrite(motorPinDirection, false);
          }

          newMotorSpeed=speedzw;
          //newMotorSpeed = (short)(1000 - (speedzw * 6));
          Serial.print("new speed :  ");
          Serial.println(newMotorSpeed);
          break;
        case 4:
          Serial.print("ModusDesMotors :  ");
          Serial.println(ModusDesMotors);
          Serial.print("Motor steps :  ");
          Serial.println(stepCounter);
          break;
      }
    }
  }
}

int MotorBase::getPosition() {
  return stepCounter;
}

void MotorBase::Stop() {
  ModusDesMotors = 0;
}

///Muss zeitlich immer exakt abgearbeitet werden
void MotorBase::Tick() 
{
  if (ModusDesMotors == 0) 
  {
    return;  //Motor steht nix machen
  }

  counterIntervall++;
  if (counterIntervall == motorSpeed)  // es ist zeit zum schalten
  {
    digitalWrite(motorPinSteps, lastStatus);

    //Serial.println(motorSpeed);
    //Serial.println(stepCounter);
    //Serial.println(targetCounter);
    //Serial.println(ModusDesMotors);

    counterIntervall = 0;

    if (lastStatus == LOW){
      lastStatus = HIGH;
    } else {
      lastStatus = LOW;
    }

    if (forward){
      stepCounter++;
    } else {
      stepCounter--;
    }

    if (stepCounter == targetCounter && ModusDesMotors == 2) 
    {
      Serial.println("Motor am ziel bewegung");
      ModusDesMotors = 0;
    }

    if (ModusDesMotors == 3 && newMotorSpeed != 0) 
    {
      Serial.print("new Motorspeed  ");
      Serial.println(newMotorSpeed);
      motorSpeed = newMotorSpeed;
      newMotorSpeed = 0;
    }
  }
}
