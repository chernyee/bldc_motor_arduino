/**
  Description : To control 2 brushless dc motors with ESCs using Arduino Uno R3
  Date        : 2017-11-19
  Creator     : Cy
  
  Change History
  --------------
  2017-11-19  Cy  Created file
  2017-11-21  Cy  Refactor with OO design & allow control of ESC individually (use to program ESC settings)
  2017-11-23  Cy  Added password protection
  2017-11-26  Cy  Added LED heart-beat runs at 500ms frequency
 **/
#include <stdio.h>
#include "Motor.h"

#define MOTOR_COUNT 2
#define BUTTON      7    

Motor motors[MOTOR_COUNT] = { Motor(9), Motor(10) };

int buttonState = HIGH;
int heartBeat = LOW;
unsigned long ticks = 0;
int menuState = 0;
int sessionState = 0;
int writeIndex = 0;
char password[9] = { 0 };

void wait(int seconds)
{
  for (int i = 0; i < seconds; i++) {
    for (int j = 0; j < 8; j++) {
      digitalWrite(LED_BUILTIN, j & 1 ? LOW : HIGH);
      delay(125);
    }
    Serial.print(F("."));
  }
}

void throttleUp()
{
  for (int i = 0; i < MOTOR_COUNT; i++) {
    motors[i].up();
  }
}

void throttleDown()
{
  for (int i = 0; i < MOTOR_COUNT; i++) {
    motors[i].down();
  }
}

void stepUp()
{
  for (int i = 0; i < MOTOR_COUNT; i++) {
    motors[i] += 50;
  }
}

void stepDown()
{
  for (int i = 0; i < MOTOR_COUNT; i++) {
    motors[i] -= 50;
  }
}

void toggleOnOff()
{
  for (int i = 0; i < MOTOR_COUNT; i++) {
    if (motors[i].connected()) {
      motors[i].disconnect();  
    } else {
      motors[i].connect();
    }
  }
}

void showMotorState(int i)
{
  Serial.print(motors[i].getThrottle());
  Serial.print(motors[i].connected() ? F(" On") : F(" Off"));
}

void showMenu()
{
  if (menuState == 1 || menuState == 2) 
  {
    Serial.println();
    Serial.print(F("Configure Motor - "));
    Serial.println(menuState == 1 ? 1 : 2);
    Serial.println(F("------------------------------------"));
    Serial.println(F("1. Throttle Up - Max. Speed"));
    Serial.println(F("2. Throttle Down - Stop"));
    Serial.println(F("3. Step Up"));
    Serial.println(F("4. Step Down"));
    Serial.println(F("5. Turn On"));
    Serial.println(F("6. Turn Off"));
    Serial.println(F("0. Back to Main Menu"));
    Serial.println();
    Serial.print(F("State: "));
    showMotorState(menuState == 1 ? 0 : 1);
    Serial.println();
  }
  else 
  {
    Serial.println();
    Serial.println(F("Main Menu - v2.1 (2017-11-23)"));
    Serial.println(F("------------------------------------"));
    Serial.println(F("1. Throttle Up - Max. Speed"));
    Serial.println(F("2. Throttle Down - Stop"));
    Serial.println(F("3. Step Up"));
    Serial.println(F("4. Step Down"));
    Serial.println(F("5. Turn On/Off Motors"));
    Serial.print(F("6. Configure Motor 1 - "));
    showMotorState(0);
    Serial.println();
    Serial.print(F("7. Configure Motor 2 - "));
    showMotorState(1);
    Serial.println();
    Serial.println(F("8. Logout"));
  }
}

void setup()
{
  // configure serial interface baudrate
  Serial.begin(115200);

  // configure input/output pins mode & direction
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  
  // initialize & connect motor objects
  for (uint8_t i = 0; i < MOTOR_COUNT; i++) {
    motors[i].connect();
  }

  // wait 5 seconds
  wait(5);

  // LED heart beat timming
  ticks = millis();
}

void processButton(int state)
{
  if (buttonState != state) 
  {
    if (state == HIGH) {
      throttleDown();
    } else {
      throttleUp();
    }
    digitalWrite(LED_BUILTIN, buttonState);
    buttonState = state;
  }
}

void processMotorCmd(int index, char cmd)
{
  switch (cmd) {
    case '1':
      motors[index].up();
      break;

    case '2':
      motors[index].down();
      break;

    case '3':
      motors[index] += 10;
      break;

    case '4':
      motors[index] -= 10;
      break;

    case '5':
      motors[index].connect();
      break;

    case '6':
      motors[index].disconnect();
      break;

    case '0':
      menuState = 0;
      showMenu();
      break;
      
    case 13:
      showMenu();
      break;

    default:
      Serial.print(F("!"));
      break;
  }
}

void processCmd()
{
  if (Serial.available())
  {
    char cmd = Serial.read();

    if (menuState == 1 || menuState == 2)
    {
      processMotorCmd(menuState == 1 ? 0 : 1, cmd);
    }
    else 
    {
      switch (cmd) {
        case '1':
          throttleUp();
          break;
        
        case '2':
          throttleDown();
          break;
        
        case '3':
          stepUp();
          break;
        
        case '4':
          stepDown();
          break;

        case '5':
          toggleOnOff();
          break;
            
        case '6':
          menuState = 1;
          showMenu();
          break;

        case '7':
          menuState = 2;
          showMenu();
          break;

        case '8':
          sessionState = 0;
          break;

        case 13:
          showMenu();
          break;
        
        default:
          Serial.print(F("!"));
      }
    }
  }
}

void processSession()
{
  if (sessionState == 1) 
  {
    if (Serial.available()) 
    {
      char c = Serial.read();
      switch (c)
      {
        case 13:
          password[writeIndex] = '\0';
          if (strncmp(password, "fee3eChu", 8) == 0) {
            sessionState = 2;
            Serial.println();
            showMenu();
          } else {
            sessionState = 0;
          }
          writeIndex = 0;
          break;

        case 8:
        case 127:
          if (writeIndex > 0) {
            --writeIndex;
            Serial.print(F("\b \b"));
          }          
          break;
          
        default:
          if (writeIndex < 8) {
            password[writeIndex++] = c;
            Serial.print(F("*"));
          }  
      }
    }    
  }
  else
  {
    sessionState = 1;
    Serial.println();
    Serial.print(F("Password: "));
  }
}

void processHeartBeat()
{
  if (buttonState == HIGH)
  {
    unsigned long t = millis() - ticks;
    if (t >= 500) {
      digitalWrite(LED_BUILTIN, heartBeat = heartBeat == HIGH ? LOW : HIGH);
      ticks += t;
    }
  }
}

void loop()
{
  processButton(digitalRead(BUTTON));
  
  processHeartBeat();

  if (sessionState == 2) {
    processCmd();  
  } else {
    processSession();
  }
}

