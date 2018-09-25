#ifndef __TERMOSTAT_H__
#define __TERMOSTAT_H__

#include <Arduino.h>
#include <Fsm.h>
#include <ProcessScheduler.h>

#include "config.h"
#include "ntc.h"

#define NOTE    (523)
#define DELAY   (180000)

typedef void (* f_void)(void);

class TermostatProcess: public Process
{
public:

    enum states {
        STOP = 0,
        HEATING,
        WARMING,
        FRYING,
        COOKING,
        END
    };

    TermostatProcess(Scheduler&, ProcPriority, unsigned int, double&, double&, NTCProbe&);
    ~TermostatProcess();

    void service() { fsm->run_machine(); }

    void Off();
    int regim();

    void heaterOn(int8_t temp) { setting = temp; }
    void heaterOff() { setting = 0; digitalWrite(HEATER_PIN, LOW);}
    void heaterHeating() { heaterOn(tempCamWarming - 20); }
    void heaterWarming() { heaterOn(tempCamWarming - 15); }
    void heaterFrying() { heaterOn(tempCamFrying); }
    void heaterCooking() { heaterOn(tempCamCooking); }

    void fanOn() { fan_on=true; digitalWrite(FAN_PIN, LOW);}
    void fanOff() { fan_on=false; digitalWrite(FAN_PIN, HIGH);}
    void steamOn() { /*digitalWrite(STEAM_PIN, HIGH);*/}
    void steamOff() { /*digitalWrite(STEAM_PIN, LOW);*/}
    void smokeOn() { /*digitalWrite(SMOKE_PIN, HIGH);*/}
    void smokeOff() { /*digitalWrite(SMOKE_PIN, LOW);*/}

    bool isTempWarming() { return temp_camera >= (tempCamWarming - 20); }
    bool isWarmed() { return probe.getTempC() >= tempProbeFrying; }
    bool isFried() { return probe.getTempC() >= tempProbeCooking; }
    bool isCooked() { return probe.getTempC() >= tempProbeEnd; }

    int initEEPROM();
    int readEEPROM();
    int writeEEPROM();

    void trigger(int ev);

    enum states cur_state;
    boolean fan_on;
    int8_t heater_power;

    int8_t tempCamWarming;
    int8_t tempCamFrying;
    int8_t tempCamCooking;

    int8_t tempProbeFrying;
    int8_t tempProbeCooking;
    int8_t tempProbeEnd;

private:

    double &temp_camera;
    double &setting;
    NTCProbe &probe;

    State *state_stop;
    State *state_heating;
    State *state_warming;
    State *state_frying;
    State *state_cooking;
    State *state_end;

    Fsm *fsm;
};

#endif