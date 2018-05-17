#include <Arduino.h>
#include <OneWire.h>

#define REQUIRESNEW     false
#define REQUIRESALARMS  false
#include <DallasTemperature.h>

#include <Fsm.h>

#include "config.h"

#define HEATING        1
#define WARMING     2
#define FRYING      3
#define COOKING     4
#define END         5 

#define TEMP_CAM_WARMING    (50*128)
#define TEMP_CAM_FRYING     (90*128)
#define TEMP_CAM_COOKING    (80*128)

#define TEMP_PROBE_FRYING   (45)
#define TEMP_PROBE_COOKING  (60)
#define TEMP_PROBE_END      (70)

volatile int16_t    temp_camera;    // Температура камеры 1/128 градуса С
int16_t             temp_cam_begin; // Температура камеры в начале 1/128 градуса С

volatile int16_t    temp_probe;     // Температура щупа в градусах С
uint32_t            temp_probe_raw;
uint8_t             temp_probe_samples;

unsigned long       next_run_termostat;

OneWire ds(ONE_WIRE_BUS);
DallasTemperature termometr(&ds);
DeviceAddress termometr_camera;

void buzzer()
{
    Serial.println("Поджигай дымогенератор");
}

void all_off()
{
    digitalWrite(HEATER_PIN, LOW);
    digitalWrite(FAN_PIN, LOW);
    digitalWrite(STEAM_PIN, LOW);
    digitalWrite(SMOKE_PIN, LOW);
}

void on_stop()
{
    temp_cam_begin = temp_camera;
    all_off();
    Serial.println("Stop");
}

void on_heating()
{
    digitalWrite(HEATER_PIN, HIGH);
    digitalWrite(FAN_PIN, HIGH);
    Serial.println("Heating");
}

void on_warming()
{
    digitalWrite(HEATER_PIN, HIGH);
    digitalWrite(FAN_PIN, HIGH);
    Serial.println("Warming");
}

void on_frying()
{
    digitalWrite(HEATER_PIN, HIGH);
    digitalWrite(FAN_PIN, HIGH);
    digitalWrite(SMOKE_PIN, HIGH);
    buzzer();
    Serial.println("Frying");
}

void off_frying()
{
    digitalWrite(SMOKE_PIN, LOW);
}

void on_cooking()
{
    digitalWrite(HEATER_PIN, HIGH);
    digitalWrite(FAN_PIN, HIGH);
    digitalWrite(STEAM_PIN, HIGH);
    Serial.println("Cooking");
}

void on_end()
{
    all_off();
    Serial.println("End");
}

void check_start();
void check_warming();

void warming();
void frying();
void cooking();

State state_stop(&on_stop, &check_start, NULL);
State state_heating(&on_heating, &check_warming, NULL);
State state_warming(&on_warming, &warming, NULL);
State state_frying(&on_frying, &frying, &off_frying);
State state_cooking(&on_cooking, &cooking, NULL);
State state_end(&on_end, NULL, NULL);

Fsm fsm_termostat(&state_stop);

void check_start()
{
    int button = digitalRead(BUTTON_PIN);
    if ((temp_camera - temp_cam_begin) > (5*128) || button > 0)
        fsm_termostat.trigger(HEATING);
}

void check_warming()
{
    if (temp_camera >= TEMP_CAM_WARMING)
        fsm_termostat.trigger(WARMING);
}

void warming()
{
    if (temp_probe >= TEMP_PROBE_FRYING)
        fsm_termostat.trigger(FRYING);
    if (temp_camera >= TEMP_CAM_WARMING)
        digitalWrite(HEATER_PIN, LOW);
    else if (temp_camera <= (TEMP_CAM_WARMING - (10*128)))
        digitalWrite(HEATER_PIN, HIGH);
}

void frying()
{
    if (temp_probe >= TEMP_PROBE_COOKING)
        fsm_termostat.trigger(COOKING);
    if (temp_camera >= TEMP_CAM_FRYING)
        digitalWrite(HEATER_PIN, LOW);
    else if (temp_camera <= (TEMP_CAM_FRYING - (10*128)))
        digitalWrite(HEATER_PIN, HIGH);
}

void cooking()
{
    if (temp_probe >= TEMP_PROBE_END)
        fsm_termostat.trigger(END);
    if (temp_camera >= TEMP_CAM_COOKING)
    {
        digitalWrite(HEATER_PIN, LOW);
        digitalWrite(STEAM_PIN, LOW);
    } else if (temp_camera <= (TEMP_CAM_COOKING - (10*128)))
    {
        digitalWrite(HEATER_PIN, HIGH);
        digitalWrite(STEAM_PIN, HIGH);
    }
}

void termostat_setup()
{
    fsm_termostat.add_transition(&state_stop, &state_heating, HEATING, NULL);
    fsm_termostat.add_transition(&state_heating, &state_warming, WARMING, NULL);
    fsm_termostat.add_transition(&state_warming, &state_frying, FRYING, NULL);
    fsm_termostat.add_transition(&state_frying, &state_cooking, COOKING, NULL);
    fsm_termostat.add_transition(&state_cooking, &state_end, END, NULL);
}

void termostat_service()
{
    fsm_termostat.run_machine();
}

void setup() {
    Serial.begin(115200);

    termostat_setup();

    termometr.begin();
    if (termometr.getDS18Count() > 0)
        termometr.getAddress(termometr_camera, 0);
    if (termometr.isConnected(termometr_camera))
        termometr.setResolution(12);

    termometr.setWaitForConversion(false);
    termometr.requestTemperatures();
    termometr.setWaitForConversion(true);

    analogReference(DEFAULT);
    pinMode(PROBE_NTC, INPUT);
    temp_probe_raw = analogRead(PROBE_NTC);
    temp_probe_samples = 1;
    // Конвертировать значение АЦП в температуру
    //temp_probe = 0;

    pinMode(FAN_PIN, OUTPUT);
    pinMode(HEATER_PIN, OUTPUT);
    pinMode(STEAM_PIN, OUTPUT);
    pinMode(SMOKE_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);

    next_run_termostat = millis() + 1000;
}

void loop() {

    temp_probe_raw += analogRead(PROBE_NTC);
    if (temp_probe_samples == 127)
    {
        temp_probe = temp_probe_raw >> 7;
        temp_probe_samples = 0;
        temp_probe_raw = temp_probe;
        // Конвертировать значение АЦП в температуру
        // temp_probe = ;
    }
    temp_probe_samples++;
    
    if (millis() >= next_run_termostat)
    {
        temp_camera = termometr.getTemp(termometr_camera);
        termometr.setWaitForConversion(false);
        termometr.requestTemperatures();
        termometr.setWaitForConversion(true);
        
        termostat_service();
        next_run_termostat = millis() + 1000;
    }
    delay(10);
}