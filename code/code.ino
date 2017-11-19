/**
  Description : To control 2 brushless dc motors with ESCs using Arduino Uno R3
  Date        : 2017-11-19
  Creator     : Cy
  
  Change History
  --------------
  2017-11-19  Cy  Created file
 **/
#include <Servo.h>
 
Servo esc1;           // control motor 1
Servo esc2;           // control motor 2
int value = 1000;     // default arming mode
int prevState = 0;    // default external event (pin state)
char c;

void ShowMenu()
{
  Serial.println("");
  Serial.println("Menu - Version 1.2-2017-1117");
  Serial.println("----------------------------------");
  Serial.println("1> Turn Off (0ms)");
  Serial.println("2> Stop / Arming (1000ms)");
  Serial.println("3> Step Up (+50ms)");
  Serial.println("4> Step Downn (-50ms)");
  Serial.println("5> Max Out / Config. Mode (2000ms)");
  Serial.println("6> Show this menu");
  Serial.println("");
  Serial.print("Current Value: ");
  Serial.println(value);
}

void setValue(int val) 
{
  esc1.writeMicroseconds(val);
  esc2.writeMicroseconds(val);
}

void setup()
{
  Serial.begin(115200);           // start serial at 115200 baud
  pinMode(LED_BUILTIN, OUTPUT);   // use for external event indicator
  pinMode(7, INPUT_PULLUP);       // configure pin 7 as input with internal pullup motor from external (on / off motors)
  esc1.attach( 9);                // attach Servo object to pin 9 
  esc2.attach(10);                // attach Servo object to pin 10
  setValue(value);                // arming motor 1 & 2
  
  // wait for 5 seconds
  for (int i = 0; i < 10; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
  }
  
  ShowMenu();
}
 
void loop()
{
  int currState;
  
  setValue(value);

  if ((currState = digitalRead(7)) != prevState)
  {
    if (!currState)
    {
      digitalWrite(LED_BUILTIN, HIGH);
      value = 2000;
    }
    else
    {
      digitalWrite(LED_BUILTIN, LOW);
      value = 1000;
    }
    prevState = currState;
  }

  if (Serial.available()) 
  {
    c = Serial.read();
    
    switch (c)
    {
      case '1':
        
        value = 0; 
        break;
        
      case '2':
      
        value = 1000; 
        break;
      
      case '3': // step up by 50ms

        value = ((value + 50) <= 2000) ? value + 50 : 2000; 
        break;
      
      case '4': // step down by 50ms
      
        value = ((value - 50) >= 1000) ? value - 50 : 1000; 
        break;
      
      case '5':
        
        value = 2000; 
        break;

      case '6':
      
        ShowMenu();
        break;
      
      default:

        Serial.print("Invalid options: ");
        Serial.println(c, HEX);
        break;
    }
  }
}

