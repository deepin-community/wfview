#ifndef FREQMEMORY_H
#define FREQMEMORY_H
#include <QString>
#include <QDebug>
#include "wfviewtypes.h"

struct preset_kind {
    // QString name;
    // QString comment;
    // unsigned int index; // channel number
    double frequency;
    rigMode_t mode;
    bool isSet;
};

class freqMemory
{
public:
    freqMemory();
    void setPreset(unsigned int index, double frequency, rigMode_t mode);
    void setPreset(unsigned int index, double frequency, rigMode_t mode, QString name);
    void setPreset(unsigned int index, double frequency, rigMode_t mode, QString name, QString comment);
    void dumpMemory();
    unsigned int getNumPresets();
    preset_kind getPreset(unsigned int index);

private:
    void initializePresets();
    unsigned int numPresets;
    unsigned int maxIndex;
    //QVector <preset_kind> presets;
    preset_kind presets[100];

};

#endif // FREQMEMORY_H
