#ifndef __NTC_H__
#define __NTC_H__

#include <ProcessScheduler.h>

class NTCProbe: public Process
{
public:
    NTCProbe(Scheduler&, ProcPriority, unsigned int, byte);

    void setup();
    void service();

    float getTempC();

    int16_t    temp;     // Усредненное напряжение на щупе

private:
    byte pin;
//    int16_t    temp;     // Усредненное напряжение на щупе
    uint32_t   raw;
    uint8_t    samples;

    const float BALANCE_RESISTOR = 200000.0;
    const float BETA = 3974.0;
    const float ROOM_TEMP = 298.15;
    const float RESISTOR_ROOM_TEMP = 220000.0;
};

#endif