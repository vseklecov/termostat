#ifndef __TERMOSTAT_H__
#define __TERMOSTAT_H__

#include <Arduino.h>
#include <Fsm.h>
#include <ProcessScheduler.h>

#include "config.h"
#include "ds_process.h"
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

    TermostatProcess(Scheduler&, ProcPriority, unsigned int, DSProcess&, NTCProbe&);
    ~TermostatProcess();

    void service() { fsm->run_machine(); }

    uint16_t temp_cam_begin; // Температура камеры в начале 1/128 градуса С

    void Off();

    void heaterOn() { heat_on=true; digitalWrite(HEATER_PIN, HIGH);}
    void heaterOff() { heat_on=false; digitalWrite(HEATER_PIN, LOW);}
    void fanOn() { fan_on=true; digitalWrite(FAN_PIN, LOW);}
    void fanOff() { fan_on=false; digitalWrite(FAN_PIN, HIGH);}
    void steamOn() { /*digitalWrite(STEAM_PIN, HIGH);*/}
    void steamOff() { /*digitalWrite(STEAM_PIN, LOW);*/}
    void smokeOn() { /*digitalWrite(SMOKE_PIN, HIGH);*/}
    void smokeOff() { /*digitalWrite(SMOKE_PIN, LOW);*/}

    bool isTempWarming() { return ds.getTempCamera() >= tempCamWarming;}
    bool isHeating() { return (ds.getTempCamera() - temp_cam_begin) > (5<<7);}
    void warming();
    void frying();
    void cooking();

    void trigger(int ev) { fsm->trigger(ev); }

    int16_t tempCamWarming = 60<<7;
    int16_t tempCamFrying = 85<<7;
    int16_t tempCamCooking = 75<<7;

    int8_t tempProbeFrying = 37;
    int8_t tempProbeCooking = 60;
    int8_t tempProbeEnd = 70;

    enum states cur_state;
    boolean fan_on;
    boolean heat_on;

private:
    DSProcess &ds;
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