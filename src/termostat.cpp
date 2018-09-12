#include <PID_v1.h>

#include "termostat.h"
#include "config.h"
#include "ds_process.h"

extern TermostatProcess tp;
extern int16_t  temp_probe;
extern void beep(int);
extern double Setpoint;
extern PID myPID;

int regim()
{
    int result = 0;
    result |= digitalRead(SPIN2);
    result |= digitalRead(SPIN1) << 1;
    return result;
}

namespace Termostat {
    
    void on_stop()
    {
        tp.Off();
        Serial.println("Stop");
    }

    void on_heating()
    {
        tp.heaterOn();
        tp.fanOn();
        Serial.println("Heating");
    }

    void on_warming()
    {
        tp.heaterOn();
        tp.fanOn();
        Serial.println("Warming");
    }

    void on_frying()
    {
        tp.heaterOn();
        tp.fanOn();
        tp.smokeOn();
        beep(NOTE);
        Serial.println("Frying");
    }

    void off_frying()
    {
        tp.smokeOff();
    }

    void on_cooking()
    {
        tp.heaterOn();
        tp.fanOn();
        tp.steamOn();
        Serial.println("Cooking");
    }

    void on_end()
    {
        tp.Off();
        Serial.println("End");
    }

    void stop()
    {
        //int button = digitalRead(BUTTON_PIN);
        //if (tp.isHeating() || button > 0)
        if (regim() == 1)
        {
            tp.trigger(TermostatProcess::HEATING);
            tp.cur_state = TermostatProcess::HEATING;
        }
    }

    void heating()
    {
        if (regim() == 0)
        {
            tp.trigger(TermostatProcess::END);
            tp.cur_state = TermostatProcess::END;
        }
        //if (tp.isTempWarming())
        //{
            tp.trigger(TermostatProcess::WARMING);
            tp.cur_state = TermostatProcess::WARMING;
            Setpoint = tp.tempCamWarming>>7;
            myPID.SetMode(AUTOMATIC);
        //}
    }

    void warming()
    {
        tp.warming();
    }

    void frying()
    {
        tp.frying();
    }

    void cooking()
    {
        tp.cooking();
    }
}

TermostatProcess::TermostatProcess(Scheduler &manager, ProcPriority pr, unsigned int period, DSProcess &d, NTCProbe &p)
        : Process(manager, pr, period), ds(d), probe(p)
{
    state_stop = new State(&Termostat::on_stop, &Termostat::stop, NULL);
    state_heating = new State(&Termostat::on_heating, &Termostat::heating, NULL);
    state_warming = new State(&Termostat::on_warming, &Termostat::warming, NULL);
    state_frying = new State(&Termostat::on_frying, &Termostat::frying, &Termostat::off_frying);
    state_cooking = new State(&Termostat::on_cooking, &Termostat::cooking, NULL);
    state_end = new State(&Termostat::on_end, NULL, NULL);

    fsm = new Fsm(state_stop);

    fsm->add_transition(state_stop, state_heating, HEATING, NULL);
    fsm->add_transition(state_heating, state_warming, WARMING, NULL);
    fsm->add_transition(state_heating, state_end, END, NULL);
    fsm->add_transition(state_warming, state_frying, FRYING, NULL);
    fsm->add_transition(state_warming, state_end, END, NULL);
    fsm->add_transition(state_frying, state_cooking, COOKING, NULL);
    fsm->add_transition(state_frying, state_end, END, NULL);
    fsm->add_transition(state_cooking, state_end, END, NULL);

    temp_cam_begin = ds.getTempCamera();
}

TermostatProcess::~TermostatProcess()
{
    free(state_stop);
    free(state_heating);
    free(state_warming);
    free(state_frying);
    free(state_cooking);
    free(state_end);
    free(fsm);
}

void TermostatProcess::Off()
{
    heaterOff();
    fanOff();
    steamOff();
    smokeOff();
}

void TermostatProcess::warming()
{
    if (regim() == 0)
    {
        fsm->trigger(END);
        cur_state = END;
    }
    if (probe.getTempC() >= tempProbeFrying || regim() == 2)
    {
        fsm->trigger(FRYING);
        cur_state = FRYING;
        Setpoint = tempCamFrying>>7;
    }

    /*if (ds.getTempCamera() >= (tempCamWarming + (2<<7)))
        heaterOff();
    else if (ds.getTempCamera() <= (tempCamWarming - (3<<7)))
        heaterOn();*/
}

void TermostatProcess::frying()
{
    if (regim() == 0)
    {
        fsm->trigger(END);
        cur_state = END;
    }
    if (probe.getTempC() >= tempProbeCooking || regim() == 3)
    {
        fsm->trigger(COOKING);
        cur_state = COOKING;
        Setpoint = tempCamCooking>>7;
    }
    /*if (ds.getTempCamera() >= (tempCamFrying + (2<<7)))
        heaterOff();
    else if (ds.getTempCamera() <= (tp.tempCamFrying - (3<<7)))
        heaterOn();*/
}


void TermostatProcess::cooking()
{
    if (regim() == 0)
    {
        fsm->trigger(END);
        cur_state = END;
    }
    if (probe.getTempC() >= tempProbeEnd || regim() == 0)
    {
        fsm->trigger(END);
        cur_state = END;
    }
    /*if (ds.getTempCamera() >= (tempCamCooking + (2<<7)))
    {
        heaterOff();
        steamOff();
    } else if (ds.getTempCamera() <= (tempCamCooking - (3<<7)))
    {
        heaterOn();
        steamOn();
    }*/

}

