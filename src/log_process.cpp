#include <Arduino.h>
#include <PID_v1.h>

#include "log_process.h"
#include "ntc.h"
#include "termostat.h"

extern NTCProbe probe;
extern TermostatProcess tp;
extern double Setpoint, Input, Output;
extern int8_t power;
extern PID myPID;

void LogProcess::setup()
{
    Serial.begin(9600);
}

void LogProcess::service()
{
    Serial.print("State ");
    Serial.print(tp.getState());
    Serial.print("; Power ");
    Serial.print(power);
//    Serial.print("; Pid mode ");
//    Serial.print(myPID.GetMode());
    Serial.print("; Produkt ");
    Serial.print(probe.temp);
//    Serial.print("; Raw ");
//    Serial.print(probe.getRaw());
//    Serial.print("; Fan ");
//    Serial.print(tp.fan_on);
//    Serial.print("; Heater ");
//    Serial.print(tp.heater_power);
//    Serial.print("; Regim ");
//    Serial.print(tp.regim());
//    Serial.print("; PIN4 ");
//    Serial.print(digitalRead(SPIN1));
//    Serial.print("; PIN7 ");
//    Serial.print(digitalRead(SPIN2));
    Serial.print("; setTemp ");
    Serial.print(tp.setTemp);
    Serial.print("; Setpoint ");
    Serial.print(Setpoint);
    Serial.print("; Input ");
    Serial.print(Input);
    Serial.print("; Output ");
    Serial.print(Output);
    Serial.println(';');
}
