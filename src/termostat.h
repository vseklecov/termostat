#ifndef __TERMOSTAT_H__
#define __TERMOSTAT_H__

#include <Arduino.h>
#include <Fsm.h>
#include <ProcessScheduler.h>

#include "config.h"
#include "ntc.h"

#define NOTE    (523)
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

    void heaterOn() { heater_power = 100; digitalWrite(HEATER_PIN, HIGH);}
    void heaterOff() { heater_power = 0; digitalWrite(HEATER_PIN, LOW);}
    void heaterWarming() { setting = tempCamWarming; }
    void heaterFrying() { setting = tempCamFrying; }
    void heaterCooking() { setting = tempCamCooking; }

    void fanOn() { fan_on=true; digitalWrite(FAN_PIN, LOW);}
    void fanOff() { fan_on=false; digitalWrite(FAN_PIN, HIGH);}
    void steamOn() { /*digitalWrite(STEAM_PIN, HIGH);*/}
    void steamOff() { /*digitalWrite(STEAM_PIN, LOW);*/}
    void smokeOn() { /*digitalWrite(SMOKE_PIN, HIGH);*/}
    void smokeOff() { /*digitalWrite(SMOKE_PIN, LOW);*/}

    bool isTempWarming() { return temp_camera >= (tempCamWarming - 15); }
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

private:

    double &temp_camera;
    double &setting;
    NTCProbe &probe;

    int8_t tempCamWarming;
    int8_t tempCamFrying;
    int8_t tempCamCooking;

    int8_t tempProbeFrying;
    int8_t tempProbeCooking;
    int8_t tempProbeEnd;

    State *state_stop;
    State *state_heating;
    State *state_warming;
    State *state_frying;
    State *state_cooking;
    State *state_end;

    Fsm *fsm;
};

#endif