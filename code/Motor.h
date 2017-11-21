#ifndef __ServoMotor_h__
#define __ServoMotor_h__

#include <Servo.h>

#define MOTOR_MIN_THROTTLE  1000
#define MOTOR_MAX_THROTTLE  2000

class Motor
{
private:
    Servo m_servoObj;
    int m_pin;
    int m_throttle;

public:
    Motor(int pin, int throttle = MOTOR_MIN_THROTTLE);
    ~Motor();
    void connect();
    void disconnect();
    void up();
    void down();
    bool connected();
    void operator+=(int throttle);
    void operator-=(int throttle);
    int getThrottle();
};

#endif /** __ServoMotor_h__ **/
