#ifndef __DS_PROCESS_H__
#define __DS_PROCESS_H__

#include <OneWire.h>
#define REQUIRESNEW false
#define REQUIRESALARMS false
#include <DallasTemperature.h>
#include <ProcessScheduler.h>

class DSProcess : public Process
{
public:

    DSProcess(Scheduler&, ProcPriority, unsigned int, DallasTemperature*);
    DSProcess(Scheduler&, ProcPriority, unsigned int, DallasTemperature*, DeviceAddress);

    void setup();
    void service();

    inline int16_t getTempCamera() { return temp_camera; }
    float getTempC();

private:
    //OneWire *ow;
    DallasTemperature *ds;
    DeviceAddress term_addr;
    int16_t temp_camera;
    bool any;
};

#endif