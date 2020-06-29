#include <Arduino.h>
#include <Actuator_I2C.h>

Actuator_I2C motorA;

unsigned long prevMillis = 0;

void setup() {
  motorA.begin(5,'B');
  Serial.begin(9600);
  while(!Serial);
  Serial.println("serial began");
}

void loop() {
  if(Serial.available()){
    String serialMsg = Serial.readString();

    if(serialMsg == "get state"){
      switch(motorA.getState()){
        case Actuator_I2C::extended:
          Serial.println("extended");
          break;
        case Actuator_I2C::extending:
          Serial.println("extending");
          break;
        case Actuator_I2C::retracted:
          Serial.println("retracted");
          break;
        case Actuator_I2C::retracting:
          Serial.println("retracting");
          break;
        case Actuator_I2C::stopped:
          Serial.println("stopped");
          break;
        case Actuator_I2C::relaxed:
          Serial.println("relaxed");
          break;
        case Actuator_I2C::timedout:
          Serial.println("timed out");
          break;
      }
    }
    else if(serialMsg.substring(0,11) == "set timeout"){
      motorA.setTimeout(serialMsg.substring(12).toFloat());
    }
    else if(serialMsg.substring(0,17) == "set normally open"){
      motorA.setNormallyOpenFB(serialMsg.substring(18).toInt());
    }
    else if(serialMsg.substring(0,10) == "set has fb"){
      motorA.setHasFeedback(serialMsg.substring(11).toInt());
    }
    else if(serialMsg.substring(0,11) == "set fb time"){
      motorA.setFeedbackTime(serialMsg.substring(12).toInt());
    }
    else if(serialMsg.substring(0,18) == "set motor polarity"){
      motorA.setMotorPolarity(serialMsg.substring(19).toInt());
    }
    else if(serialMsg == "extend"){
      motorA.extend();
    }
    else if(serialMsg == "retract"){
      motorA.retract();
    }
    else if(serialMsg == "stop"){
      motorA.stop();
    }
    else if(serialMsg == "relax"){
      motorA.relax();
    }
  }

  motorA.cyclic();
  
}