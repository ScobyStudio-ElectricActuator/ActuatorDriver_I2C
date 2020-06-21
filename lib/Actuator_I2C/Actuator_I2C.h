////////////////////////////////////
//    Project: Actuator Driver    //
//      Author: Scoby Studio      //
//         Revision: v1.0         //
////////////////////////////////////
/*
Revision history
v1.0
-Remove set, get, min speed
-Copy code from Actuator_IO
-Initial release
*/

#ifndef Actuator_I2C_h
#define Actuator_I2C_h

#include "Arduino.h"
#include "Adafruit_MCP23017.h"

class Actuator_I2C{
    public:
        enum state{
            extended,
            extending,
            retracted,
            retracting,
            stopped,
            relaxed,
            timedout,
        };

        void begin(byte address, char channel);
        void cyclic();

        state getState();
        void setTimeout(unsigned int timeout);
        void setNormallyOpenFB(bool isNormallyOpen);
        void setHasFeedback(bool hasFeedback);
        void setFeedbackTime(unsigned int FBTime);

        void extend();
        void retract();
        void stop();
        void relax();

    private:
        Adafruit_MCP23017 mcp;

        int _extPin;
        int _retPin;
        int _enablePin;
        int _output1Pin;
        int _output2Pin;
        int _extLED;
        int _retLED;

        unsigned long _timeout;
        bool _isNO;
        bool _hasFB;
        unsigned long _FBTime;

        bool _extFB;
        bool _retFB;

        state _state;
        state _prevState;

        void writeOutput();
        void readInput();
};


#endif