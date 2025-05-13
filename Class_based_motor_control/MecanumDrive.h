#ifndef MECANUMDRIVE_H
#define MECANUMDRIVE_H

#include <Arduino.h>

class MecanumDrive {
  public:
    MecanumDrive(int in1, int in2, int pwm, int enca, int encb);
    void init();
    void setSpeed(float speed);
    void updateEncoder();
    long getForwardCount();
    long getBackwardCount();
    
  private:
    int _in1, _in2, _pwm;
    int _enca, _encb;
    volatile long _forwardCount;
    volatile long _backwardCount;
};

#endif
