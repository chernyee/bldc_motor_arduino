/**
  Description : To control 2 brushless dc motors with ESCs using Arduino Uno R3
  Date        : 2017-11-19
  Creator     : Cy
  
  Change History
  --------------
  2017-11-19  Cy  Created file
  2017-11-19  Cy  Implemented config mode & code refactoring
 **/
#include <Servo.h>
 
#define SIGNAL_PIN    7
#define CONFIG_PIN    8
#define COUNT         2
#define MIN_US        1000
#define MAX_US        2000

typedef enum {
  NORMAL,
  MANUAL
} mode_t;
  
typedef struct { 
  uint8_t pinNumber;
  int throttle_us;
  Servo servo; 
} esc_t;

esc_t list[COUNT] = { 
  { 9 , MIN_US, Servo() },  // motor 1 -> pin 9
  { 10, MIN_US, Servo() }   // motor 2 -> pin 10
};

mode_t mode = NORMAL;
int selected = 0;
int state = LOW;

void showMenu()
{
  Serial.println();
  Serial.println(F("Menu - Version 2.0 (2017-11-19)"));
  Serial.println(F("-------------------------------"));
  Serial.println(F("1. Min. Throttle - 10ms"));
  Serial.println(F("2. Max. Throttle - 20ms"));
  Serial.println(F("3. Step Up"));
  Serial.println(F("4. Step Down"));
  Serial.println(F("5. Turn Off Pulse Signal"));
  Serial.println(F("6. Status"));
  if (mode == MANUAL) {
      Serial.print(F("7. Select Motor - "));
      Serial.print(selected + 1);
      Serial.print(F(" of "));
      Serial.println(COUNT);
  }
  Serial.println(F("0. Show Menu"));
  Serial.println();
}

void wait(int times, int ms)
{
  for (uint8_t i = 0; i < times; i++) {
    digitalWrite(LED_BUILTIN, (i & 1 ? LOW : HIGH));
    delay(ms);
  }  
}

void setValue(int value, esc_t &esc)
{
  esc.servo.writeMicroseconds(esc.throttle_us = value);
}

void setThrottle(int val) 
{
  for (uint8_t i = 0; i < COUNT; i++) {
    setValue(val, list[i]);
  }
}

int calcThrottle(int throttle)
{
  return throttle > MAX_US ? MAX_US : (throttle < MIN_US ? MIN_US : throttle);
}

void processCommand(char cmd)
{
  switch (cmd)
  {
    case '0':
      showMenu();
      break;

    case '1':
      if (mode == MANUAL) {
        setValue(MIN_US, list[selected]);
      } else {
        setThrottle(MIN_US);
      }
      break;

    case '2':
      if (mode == MANUAL) {
        setValue(MAX_US, list[selected]);
      } else {
        setThrottle(MAX_US);
      }
      break;

    case '3':
      if (mode == MANUAL) {
        setValue(calcThrottle(list[selected].throttle_us + 50), list[selected]);
      } else {
        setThrottle(calcThrottle(list[0].throttle_us + 50));
      }
      break;
      
    case '4':
      if (mode == MANUAL) {
        setValue(calcThrottle(list[selected].throttle_us - 50), list[selected]);
      } else {
        setThrottle(calcThrottle(list[0].throttle_us - 50));
      }
      break;

    case '5':
      if (mode == MANUAL) {
        setValue(0, list[selected]);
      } else {
        setThrottle(0);
      }
      break;
      
    case '6':
      Serial.print(F("Mode         : "));
      Serial.println(mode == NORMAL ? F("Normal") : F("Manual"));
      Serial.print(F("Throttle(ms) : "));
      Serial.println(list[mode == NORMAL ? 0 : selected].throttle_us);    
      Serial.println();
      break;
      
    case '7':
      if (mode == MANUAL) {
        selected = ++selected % COUNT;
        Serial.print(F("Selected motor "));
        Serial.print(selected + 1);
        Serial.print(F(" of "));
        Serial.print(COUNT);
        Serial.println(F("!"));
      } else {
        Serial.println(F("Not support in current mode!"));
        Serial.println();
      }
      break;
      
    default:
      Serial.print(F("Invalid command: "));
      Serial.println(cmd);
  }
}

void setup()
{
  // configure serial interface at 115200 baud
  Serial.begin(115200);

  // configure input/output pins
  pinMode(LED_BUILTIN, OUTPUT);         // event indicator LED
  pinMode(SIGNAL_PIN, INPUT_PULLUP);    // input (pullup mode) from external to turn on/off the motors
  pinMode(CONFIG_PIN, INPUT_PULLUP);    // input (pullup mode) to enter config mode

  // read config pin
  if (!digitalRead(CONFIG_PIN)) {
    mode = MANUAL;
  }
  
  // configure pins to the Servo objects
  for (int i = 0; i < COUNT; i++) {
    if (mode == MANUAL) {
      list[i].throttle_us = 0;
    }
    list[i].servo.attach(list[i].pinNumber);
    list[i].servo.writeMicroseconds(list[i].throttle_us);
  }

  // for normal mode, wait for 5 sec and while waiting, blink the led
  if (mode == NORMAL) {
    wait(20, 250);  
  }
  
  // show menu
  showMenu();
}

void processSignal(int value) 
{
  if (state != value) {
    if (value == LOW) {
      digitalWrite(LED_BUILTIN, HIGH);
      setThrottle(MAX_US);
    } else {
      digitalWrite(LED_BUILTIN, LOW);
      setThrottle(MIN_US);
    }
    state = value;
  }
}

void loop()
{
  if (mode == NORMAL) processSignal(digitalRead(SIGNAL_PIN));
  if (Serial.available()) processCommand(Serial.read());
}

