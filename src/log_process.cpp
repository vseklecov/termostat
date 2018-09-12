#include <Arduino.h>

#include "log_process.h"
#include "ds_process.h"
#include "ntc.h"
#include "termostat.h"

extern DSProcess ds;
extern NTCProbe probe;
extern TermostatProcess tp;
extern int regim();

void LogProcess::setup()
{
    Serial.begin(9600);
}

void LogProcess::service()
{
    Serial.print("State ");
    Serial.print(tp.cur_state);
    Serial.print("; Kamera ");
    Serial.print(ds.getTempC());
    Serial.print("; Produkt ");
    Serial.print(probe.temp);
    Serial.print("; Fan ");
    Serial.print(tp.fan_on);
    Serial.print("; Heater ");
    Serial.print(tp.heat_on);
    Serial.print("; Regim ");
    Serial.print(regim());
    Serial.print("; PIN4 ");
    Serial.print(digitalRead(SPIN1));
    Serial.print("; PIN7 ");
    Serial.print(digitalRead(SPIN2));
    Serial.println(';');
}
