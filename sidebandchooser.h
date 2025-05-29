#ifndef SIDEBANDCHOOSER_H
#define SIDEBANDCHOOSER_H

#include "wfviewtypes.h"

class sidebandChooser
{
public:
    sidebandChooser();

    static inline rigMode_t getMode(freqt f, rigMode_t currentMode = modeUSB) {

        if((currentMode == modeLSB) || (currentMode == modeUSB) )
        {
        if(f.Hz < 5000000)
        {
            return modeLSB;
        }
        if( (f.Hz >= 5000000) && (f.Hz < 5600000) )
        {
            return modeUSB;
        }
        if( (f.Hz >= 5600000) && (f.Hz < 10000000) )
        {
            return modeLSB;
        }

        return modeUSB;
        } else {
            return currentMode;
        }
    }


private:

};

#endif // SIDEBANDCHOOSER_H
