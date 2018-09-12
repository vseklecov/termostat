#include "ntc.h"

#define MAX_ADC (1023)

NTCProbe::NTCProbe(Scheduler &manager, ProcPriority pr, unsigned int period, byte p)
: Process(manager, pr, period), pin(p)
{
    analogReference(DEFAULT);
    pinMode(pin, INPUT);
}

void NTCProbe::setup()
{
    raw = analogRead(pin);
    samples = 1;
    // Конвертировать значение АЦП в температуру
    temp = raw;
}

void NTCProbe::service()
{
    raw += analogRead(pin);
    if (samples == 63)
    {
        temp = raw >> 6;
        samples = 0;
        raw = temp;
        // Конвертировать значение АЦП в температуру
    }
    samples++;
}

float NTCProbe::getTempC()
{
    float rT = BALANCE_RESISTOR * ((MAX_ADC/temp)-1);
    float tK = (BETA*ROOM_TEMP)/(BETA+(ROOM_TEMP*log(rT/RESISTOR_ROOM_TEMP)));
    float tC = tK-273.15;
    return tC;
}