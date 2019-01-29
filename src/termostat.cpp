#include <EEPROM.h>
#include <MsTimer2.h>

#include "termostat.h"
#include "config.h"

extern TermostatProcess tp;
extern double Input;
extern PID myPID;
extern void powerControl();

void beep(int note)
{
    tone(BUZZER_PIN, note, 250);
}

namespace Termostat {
    
    void on_stop()
    {
        tp.Off();
        Serial.println("Stop");
    }

    void set_temp_warming()
    {
        tp.setTemp = tp.tempCamWarming;
    }

    void set_temp_frying()
    {
        tp.setTemp = tp.tempCamFrying;
    }

    void on_heating()
    {
        tp.heaterHeating();
        tp.fanOn();
        Serial.println("Heating");
    }

    void on_warming()
    {
        tp.heaterWarming();
        tp.fanOn();
        Serial.println("Warming");
    }

    void on_frying()
    {
        tp.heaterFrying();
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
        tp.heaterCooking();
        tp.fanOn();
        tp.steamOn();
        MsTimer2::stop();
        MsTimer2::set(100, powerControl);
        MsTimer2::start();
        Serial.println("Cooking");
    }

    void on_end()
    {
        tp.Off();
        beep(NOTE);
        Serial.println("End");
    }

    void stop()
    {
        if (tp.regim() > 0)
            tp.trigger(TermostatProcess::HEATING);
    }

    void wait()
    {
        if (tp.regim() == 0)
            tp.trigger(TermostatProcess::END);
    }

    void heating()
    {
        // Пауза в нагреве
        static unsigned long endPause = 0;

        if (tp.regim() == 0)
            tp.trigger(TermostatProcess::END);
        if (tp.isTempWarming())
            tp.trigger(TermostatProcess::WARMING);
        if (tp.isTempFrying())
            tp.trigger(TermostatProcess::FRYING);
        if (endPause == 0)
        {
            if (tp.isHeated())
            {
                endPause = millis() + DELAY;
        //        tp.trigger(TermostatProcess::WAIT);
            }
        } else
        {
            if (millis() >= endPause)
            {
                tp.heaterHeating();
                endPause = 0;
            }
        }
    }

    void warming()
    {
        if (tp.regim() == 0)
            tp.trigger(TermostatProcess::END);
        if (tp.isWarmed() || tp.regim() == 2)
            tp.trigger(TermostatProcess::HEATING);
    }

    void frying()
    {
        if (tp.regim() == 0)
            tp.trigger(TermostatProcess::END);
        if (tp.isFried() || tp.regim() == 3)
            tp.trigger(TermostatProcess::COOKING);
    }

    void cooking()
    {
        if (tp.isCooked() || tp.regim() == 0)
            tp.trigger(TermostatProcess::END);
    }
}

TermostatProcess::TermostatProcess(Scheduler &manager, ProcPriority pr, unsigned int period, double &t, double &s, NTCProbe &p)
        : Process(manager, pr, period), temp_camera(t), setting(s), probe(p)
{
    state_stop = new State(&Termostat::on_stop, &Termostat::stop, NULL);
    state_heating = new State(&Termostat::on_heating, &Termostat::heating, NULL);
    state_warming = new State(&Termostat::on_warming, &Termostat::warming, NULL);
    state_frying = new State(&Termostat::on_frying, &Termostat::frying, &Termostat::off_frying);
    state_cooking = new State(&Termostat::on_cooking, &Termostat::cooking, NULL);
    state_end = new State(&Termostat::on_end, NULL, NULL);
    state_wait = new State(NULL, &Termostat::wait, NULL);

    fsm = new Fsm(state_stop);

    fsm->add_transition(state_stop, state_heating, HEATING, &Termostat::set_temp_warming);
    fsm->add_transition(state_heating, state_warming, WARMING, NULL);
    fsm->add_transition(state_heating, state_frying, FRYING, NULL);
    fsm->add_transition(state_heating, state_wait, WAIT, NULL);
    //fsm->add_timed_transition(state_wait, state_heating, 60000, NULL);
    fsm->add_transition(state_warming, state_heating, HEATING, &Termostat::set_temp_frying);
    fsm->add_transition(state_frying, state_cooking, COOKING, NULL);

    fsm->add_transition(state_wait, state_end, END, NULL);
    fsm->add_transition(state_heating, state_end, END, NULL);
    fsm->add_transition(state_warming, state_end, END, NULL);
    fsm->add_transition(state_frying, state_end, END, NULL);
    fsm->add_transition(state_cooking, state_end, END, NULL);
}

TermostatProcess::~TermostatProcess()
{
    free(state_stop);
    free(state_heating);
    free(state_warming);
    free(state_frying);
    free(state_cooking);
    free(state_end);
    free(state_wait);
    free(fsm);
}

void TermostatProcess::trigger(int ev)
{
    fsm->trigger(ev);
    tp.cur_state = (enum states) ev;
}

void TermostatProcess::Off()
{
    heaterOff();
    fanOff();
    steamOff();
    smokeOff();
}

bool TermostatProcess::isWarmed()
{
    float temp = probe.getTempC();
    if (temp > 100.0)
        return false;
    return temp >= tempProbeFrying; 
}

bool TermostatProcess::isFried()
{
    float temp = probe.getTempC();
    if (temp > 100.0)
        return false;
    return temp >= tempProbeCooking;
}

bool TermostatProcess::isCooked()
{
    float temp = probe.getTempC();
    if (temp > 100.0)
        return false;
    return temp >= tempProbeEnd;
}

int TermostatProcess::regim()
{
    int result = 0;
    result |= digitalRead(SPIN2);
    result |= digitalRead(SPIN1) << 1;
    return result;
}

int TermostatProcess::readEEPROM()
{
    int i=0;

    tempCamWarming = EEPROM.read(i++);
    tempCamFrying = EEPROM.read(i++);
    tempCamCooking = EEPROM.read(i++);
    tempProbeFrying = EEPROM.read(i++);
    tempProbeCooking = EEPROM.read(i++);
    tempProbeEnd = EEPROM.read(i++);

    return i;
}

int TermostatProcess::initEEPROM()
{
    tempCamWarming = 60;
    tempCamFrying = 85;
    tempCamCooking = 75;

    tempProbeFrying = 37;
    tempProbeCooking = 60;
    tempProbeEnd = 70;
    return writeEEPROM();
}

int TermostatProcess::writeEEPROM()
{
    int i = 0;

    EEPROM.write(i++, tempCamWarming);
    EEPROM.write(i++, tempCamFrying);
    EEPROM.write(i++, tempCamCooking);
    EEPROM.write(i++, tempProbeFrying);
    EEPROM.write(i++, tempProbeCooking);
    EEPROM.write(i++, tempProbeEnd);

    return i;
}

void TermostatProcess::heaterHeating()
{
    double delta = setTemp - temp_camera;
    tp.cur_state = HEATING;
    myPID.SetMode(MANUAL);
    // Добавляем по 5 градусов
    //setting = min(setTemp, temp_camera + 5);
    // Более агрессивный нагрев
    if ((setTemp > temp_camera) && (delta > 5))
        setting = temp_camera + delta/2;
    else
        setting = setTemp;
    myPID.SetMode(AUTOMATIC);
}

String TermostatProcess::getState()
{
    switch (cur_state)
    {
        case STOP:
            return "STOP";
        case HEATING:
            return "HEATING";
        case WARMING:
            return "WARMING";
        case FRYING:
            return "FRYING";
        case COOKING:
            return "COOKING";
        case END:
            return "END";
        case WAIT:
            return "WAIT";
        default:
            return "NAN";
    }
}
