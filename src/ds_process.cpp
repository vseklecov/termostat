#include "ds_process.h"

DSProcess::DSProcess(Scheduler &manager, ProcPriority pr, unsigned int period, DallasTemperature *dt)
    : Process(manager, pr, period), ds(dt)
{
    any = true;
}

DSProcess::DSProcess(Scheduler &manager, ProcPriority pr, unsigned int period, DallasTemperature *dt, DeviceAddress ds_addr)
    : Process(manager, pr, period), ds(dt)
{
    if (ds->validAddress(ds_addr))
    {
        for(int i=0;i<8;i++)
            term_addr[i] = ds_addr[i];
        any = false;
    } else
        any = true;
}

void DSProcess::setup()
{
    if (any)
    {
        while (ds->getDS18Count() == 0) {
            Serial.println("DS18B20 not found");
            ds->begin();
            delay(1000);
        }
        ds->getAddress(term_addr, 0);
    }
    if (ds->isConnected(term_addr))
    {
        int resolution = 11;
        ds->setResolution(resolution);
        setPeriod(ds->millisToWaitForConversion(resolution)+10);
    } else
    {
        disable();
        return;
    }

    ds->setWaitForConversion(false);
    ds->requestTemperatures();
    ds->setWaitForConversion(true); 
}

void DSProcess::service()
{
    temp_camera = ds->getTemp(term_addr);
    ds->setWaitForConversion(false);
    ds->requestTemperatures();
    ds->setWaitForConversion(true);
}

float DSProcess::getTempC()
{
    return (float) temp_camera * 0.0078125;
}