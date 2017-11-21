#include <Arduino.h>
#include "Motor.h"

Motor::Motor(int pin, int throttle) : m_pin(pin), m_throttle(throttle)
{
    
}

Motor::~Motor()
{
    
}

void Motor::connect()
{
    m_servoObj.attach(m_pin, MOTOR_MIN_THROTTLE, MOTOR_MAX_THROTTLE);
    m_servoObj.writeMicroseconds(m_throttle);
}

void Motor::disconnect()
{
    m_servoObj.detach();
    
    // make sure the pin is at LOW state
    digitalWrite(m_pin, LOW);
}

void Motor::up()
{
    m_servoObj.writeMicroseconds(m_throttle = MOTOR_MAX_THROTTLE);
}

void Motor::down()
{
    m_servoObj.writeMicroseconds(m_throttle = MOTOR_MIN_THROTTLE);
}

void Motor::operator+=(int offset)
{
    m_throttle = (m_throttle + offset) < MOTOR_MAX_THROTTLE ? m_throttle + offset : MOTOR_MAX_THROTTLE;
    m_servoObj.writeMicroseconds(m_throttle);
}

void Motor::operator-=(int offset)
{
    m_throttle = (m_throttle - offset) > MOTOR_MIN_THROTTLE ? m_throttle - offset : MOTOR_MIN_THROTTLE;
    m_servoObj.writeMicroseconds(m_throttle);
}

bool Motor::connected()
{
    return m_servoObj.attached();
}

int Motor::getThrottle()
{
    return m_throttle;
}
