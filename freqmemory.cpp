#include "freqmemory.h"
#include "logcategories.h"

// Copyright 2017-2024 Elliott H. Liggett

freqMemory::freqMemory()
{
    // NOTE: These are also present in the header and
    // the array must be changed if these are changed.
    numPresets = 100;
    maxIndex = numPresets - 1;
    initializePresets();

}

void freqMemory::initializePresets()
{
    // qInfo() << "Initializing " << numPresets << " memory channels";

    for(unsigned int p=0; p < numPresets; p++)
    {
        presets[p].frequency = 14.1 + p/1000.0;
        presets[p].mode = modeUSB;
        presets[p].isSet = false;
    }
}

void freqMemory::setPreset(unsigned int index, double frequency, rigMode_t mode)
{
    if(index <= maxIndex)
    {
        presets[index].frequency = frequency;
        presets[index].mode = mode;
        presets[index].isSet = true;
    }
}

preset_kind freqMemory::getPreset(unsigned int index)
{
    //validate then return

    if(index <= maxIndex)
    {
        return presets[index];
    }

    // else, return something obviously wrong
    preset_kind temp;
    temp.frequency=12.345;
    temp.mode = modeUSB;
    temp.isSet = false;
    return temp;
}

unsigned int freqMemory::getNumPresets()
{
    return numPresets;
}

void freqMemory::dumpMemory()
{
    for(unsigned int p=0; p < numPresets; p++)
    {
        if(presets[p].isSet)
        {
        qInfo(logSystem()) << "Index: " << p << " freq: " << presets[p].frequency << " Mode: " << presets[p].mode << " isSet: " << presets[p].isSet;
        }
    }
}
