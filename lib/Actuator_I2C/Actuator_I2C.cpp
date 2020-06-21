#include "Arduino.h"
#include "Actuator_I2C.h"

void Actuator_I2C::begin(byte address, char channel){
    mcp.begin(address);

    if(channel == 'A'){
        _extPin = 0;
        _retPin = 1;
        _enablePin = 2;
        _output1Pin = 3;
        _output2Pin = 4;
        _extLED = 5;
        _retLED = 6;
    }
    else if(channel == 'B'){
        _extPin = 15;
        _retPin = 14;
        _enablePin = 13;
        _output1Pin = 12;
        _output2Pin = 11;
        _extLED = 10;
        _retLED = 9;
    }

    mcp.pinMode(_extPin, INPUT);
    mcp.pullUp(_extPin, HIGH);
    mcp.pinMode(_retPin, INPUT);
    mcp.pullUp(_retPin, HIGH);
    mcp.pinMode(_enablePin, OUTPUT);
    mcp.pinMode(_output1Pin, OUTPUT);
    mcp.pinMode(_output2Pin, OUTPUT);
    mcp.pinMode(_extLED, OUTPUT);
    mcp.pinMode(_retLED, OUTPUT);

    setNormallyOpenFB(true);
    setHasFeedback(true);
    setFeedbackTime(3);
    setTimeout(3);

    readInput();
    if(_extFB){
        _state = extended;
        _prevState = extended;
    }
    else if(_retFB){
        _state = retracted;
        _prevState = retracted;
    }
    else{
        _state = relaxed;
        _prevState = relaxed;
    }
    
    cyclic();
}

Actuator_I2C::state Actuator_I2C::getState(){
    return _state;
}

void Actuator_I2C::setTimeout(unsigned int timeout){
    if(timeout >= 0){
        _timeout = timeout * 1000;
    }
}

void Actuator_I2C::setNormallyOpenFB(bool isNormallyOpen){
    _isNO = isNormallyOpen;
}

void Actuator_I2C::setHasFeedback(bool hasFeedback){
    _hasFB = hasFeedback;
}

void Actuator_I2C::setFeedbackTime(unsigned int FBTime){
    if(FBTime >= 0){
        _FBTime = FBTime * 1000;
    }
}

void Actuator_I2C::extend(){
    if(!_extFB){
        _state = extending;
    }
}

void Actuator_I2C::retract(){
    if(!_retFB){
        _state = retracting;
    }
}

void Actuator_I2C::stop(){
    _state = stopped;
}

void Actuator_I2C::relax(){
    _state = relaxed;
}

void Actuator_I2C::readInput(){
    _extFB = (mcp.digitalRead(_extPin) != _isNO);
    _retFB = (mcp.digitalRead(_retPin) != _isNO);
}

void Actuator_I2C::writeOutput(){
    switch(_state){
        case extending:
            mcp.digitalWrite(_output1Pin,HIGH);
            mcp.digitalWrite(_output2Pin,LOW);
            mcp.digitalWrite(_enablePin,HIGH);
            break;
        case retracting:
            mcp.digitalWrite(_output1Pin,LOW);
            mcp.digitalWrite(_output2Pin,HIGH);
            mcp.digitalWrite(_enablePin,HIGH);
            break;
        case stopped:
        case extended:
        case retracted:
        case timedout:
            mcp.digitalWrite(_output1Pin,LOW);
            mcp.digitalWrite(_output2Pin,LOW);
            mcp.digitalWrite(_enablePin,HIGH);
            break;
        case relaxed:
            mcp.digitalWrite(_output1Pin,LOW);
            mcp.digitalWrite(_output2Pin,LOW);
            mcp.digitalWrite(_enablePin,LOW);
            break;
    }
    mcp.digitalWrite(_extLED, _extFB);
    mcp.digitalWrite(_retLED, _retFB);
}

void Actuator_I2C::cyclic(){
    static unsigned long startTime;

    readInput();
    switch(_state){
        case extended:
            if(_state != _prevState){
                _prevState = _state;
            }
            break;

        case extending:
            if(_state != _prevState){
                startTime = millis();
                _prevState = _state;
            }

            if(millis()-startTime >= _timeout && _timeout != 0 && _hasFB){
                _state = timedout;
            }
            
            if((_extFB && !_retFB && _hasFB) || (millis()-startTime >= _FBTime && !_hasFB)){
                _state = extended;
            }
            break;

        case retracted:
            if(_state != _prevState){
                _prevState = _state;
            }
            break;

        case retracting:
            if(_state != _prevState){
                startTime = millis();
                _prevState = _state;
            }

            if(millis()-startTime >= _timeout && _timeout != 0 && _hasFB){
                _state = timedout;
            }
            
            if((!_extFB && _retFB && _hasFB) || (millis()-startTime >= _FBTime && !_hasFB)){
                _state = retracted;
            }
            break;

        case timedout:
            if(_state != _prevState){
                _prevState = _state;
            }
            break;

        case stopped:
            if(_state != _prevState){
                _prevState = _state;
            }
            break;

        case relaxed:
            if(_state != _prevState){
                _prevState = _state;
            }
            break;
    }
    writeOutput();
}