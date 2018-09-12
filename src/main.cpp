#include <Arduino.h>

#include "config.h"

#include <ProcessScheduler.h>
Scheduler sched;

#include <OneWire.h>
OneWire ow(ONE_WIRE_BUS);
#include <DallasTemperature.h>
DallasTemperature dt(&ow);

#include "ds_process.h"
DSProcess ds(sched, HIGH_PRIORITY, 750, &dt);

#include "ntc.h"
NTCProbe probe(sched, HIGH_PRIORITY, SERVICE_CONSTANTLY, PROBE_NTC);

#include "termostat.h"
TermostatProcess tp(sched, HIGH_PRIORITY, SERVICE_SECONDLY, ds, probe);

#include "log_process.h"
LogProcess lp(sched, LOW_PRIORITY, 10000);

#include <PID_v1.h>
double Setpoint, Input, Output;
double Kp = 2, Ki = 5, Kd = 1;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, P_ON_M, DIRECT);
int WindowSize = 5000;
unsigned long windowStartTime;

/****** Change PWM frecuency ARDUINO UNO***** 
Function:  		setPwmFrequencyUNO(pin,divisor);
Description:
DO NOT MODIFY pin 6 is the one on which rely all time functions in Arduino: i.e., 
if you change the frequency of this pins, function like delay() or millis() 
will continue to work but at a different timescale (quicker or slower!!!)
 
For pins 6 and 5:   (5 and 6 are together, can not modify individually,same timer)
    Divisor     Frequency
   	1 	 	    62500 Hz
	2 	 	    7812.5 Hz
	3 	 	    976.5625 Hz  <--DEFAULT Diecimila bootloader
	4 	 	    244.140625 Hz
	5 	 	    61.03515625 Hz  
For pins 9, 10, 11 and 3: (9 & 10 and 11 & 3 are together, 
can not modify individually,same timer)
    Divisor     Frequency
	1 	        31372.55 Hz
	2	        3921.16  Hz
	3 	        979.527  Hz
 	4 	        490.20   Hz   <--DEFAULT Diecimila bootloader
	5 	        244.882  Hz
	6	        122.55   Hz
	7	        30.610   Hz
*/
void setPwmFrequencyUNO(int pin, int divisor)
{
    byte mode;
    if (pin == 5 || pin == 6 || pin == 9 || pin == 10)
    {
        switch (divisor)
        {
        case 1:
            mode = 0x01;
            break;
        case 2:
            mode = 0x02;
            break;
        case 3:
            mode = 0x03;
            break;
        case 4:
            mode = 0x04;
            break;
        case 5:
            mode = 0x05;
            break;
        default:
            return;
        }
        if (pin == 5 || pin == 6)
        {
            TCCR0B = TCCR0B & 0b11111000 | mode;
        }
        else
        {
            TCCR1B = TCCR1B & 0b11111000 | mode;
        }
    }
    else if (pin == 3 || pin == 11)
    {
        switch (divisor)
        {
        case 1:
            mode = 0x01;
            break;
        case 2:
            mode = 0x02;
            break;
        case 3:
            mode = 0x03;
            break;
        case 4:
            mode = 0x04;
            break;
        case 5:
            mode = 0x05;
            break;
        case 6:
            mode = 0x06;
            break;
        case 7:
            mode = 0x07;
            break;
        default:
            return;
        }
        TCCR2B = TCCR2B & 0b11111000 | mode;
    }
}

/******** Change PWM frecuency ARDUINO MEGA 2560***** 
	Function:     setPwmFrequencyMEGA2560(pin,divisior); 
 This pins are together, can not modify frequency individually,same timmer):
 pin 13, 4
 pin 12, 11
 pin 10, 9
 pin 5, 3, 2
 pin 8, 7, 6
For pins 13,4 (DO NOT MODIFY pins 13 & 4 is the one on which rely 
all time functions in Arduino: i.e., if you change the frequency of this pins, 
function like delay() or millis() will continue to work but at a different timescale
quicker or slower!))
    Divisor     Frequency
   	1 	 	    62500 Hz
	2 	 	    7812.5 Hz
	3 	 	    976.5625 Hz  <--DEFAULT Diecimila bootloader
	4 	 	    244.140625 Hz
	5 	 	    61.03515625 Hz

For pins 2 to 13 EXCEPT 13,4:
    Divisor     Frequency
	1 	        31372.55 Hz
	2	        3921.16  Hz
	3	        490.20    Hz   <--DEFAULT Diecimila bootloader
	4	        122.55    Hz
	5	        30.610    Hz
*/
/*void setPwmFrequencyMEGA2560(int pin, int divisor)
{
    byte mode;

    switch (divisor)
    {
    case 1:
        mode = 0x01;
        break;
    case 2:
        mode = 0x02;
        break;
    case 3:
        mode = 0x03;
        break;
    case 4:
        mode = 0x04;
        break;
    case 5:
        mode = 0x05;
        break;
    case 6:
        mode = 0x06;
        break;
    case 7:
        mode = 0x07;
        break;
    default:
        return;
    }

    switch (pin)
    {
    case 2:
        TCCR3B = TCCR3B & 0b11111000 | mode;
        break;
    case 3:
        TCCR3B = TCCR3B & 0b11111000 | mode;
        break;
    case 4:
        TCCR0B = TCCR0B & 0b11111000 | mode;
        break;
    case 5:
        TCCR3B = TCCR3B & 0b11111000 | mode;
        break;
    case 6:
        TCCR4B = TCCR4B & 0b11111000 | mode;
        break;
    case 7:
        TCCR4B = TCCR4B & 0b11111000 | mode;
        break;
    case 8:
        TCCR4B = TCCR4B & 0b11111000 | mode;
        break;
    case 9:
        TCCR2B = TCCR0B & 0b11111000 | mode;
        break;
    case 10:
        TCCR2B = TCCR2B & 0b11111000 | mode;
        break;
    case 11:
        TCCR1B = TCCR1B & 0b11111000 | mode;
        break;
    case 12:
        TCCR1B = TCCR1B & 0b11111000 | mode;
        break;
    case 13:
        TCCR0B = TCCR0B & 0b11111000 | mode;
        break;
    default:
        return;
    }
}*/

void beep(int note)
{
    tone(BUZZER_PIN, note, 250);
}

void setup()
{

    Serial.begin(9600);

    pinMode(ONE_WIRE_BUS, INPUT);
    dt.begin();
    while (dt.getDS18Count() == 0)
    {
        Serial.println("Setup DS18b20 not found");
        delay(1000);
    }
    ds.add(true);
    probe.add(true);
    tp.add(true);
    lp.add(true);

    pinMode(FAN_PIN, OUTPUT);

    setPwmFrequencyUNO(HEATER_PIN, 7);
    pinMode(HEATER_PIN, OUTPUT);

    //pinMode(STEAM_PIN, OUTPUT);
    //pinMode(SMOKE_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);

    //pinMode(BUTTON_PIN, INPUT);
    pinMode(SPIN1, INPUT);
    pinMode(SPIN2, INPUT);

    //myPID.SetOutputLimits(0, WindowSize);
    //windowStartTime = millis();
}

void loop()
{

    //if (millis() - windowStartTime > WindowSize)
    //    windowStartTime += WindowSize;
    switch (tp.cur_state)
    {
    case tp.WARMING:
    case tp.FRYING:
    case tp.COOKING:
        Input = ds.getTempC();
        if (myPID.Compute())
        {
            analogWrite(HEATER_PIN, Output);
            // if (Output < millis() - windowStartTime)
            //    tp.heaterOn();
            //else
            //    tp.heaterOff();
        }
        break;
    default:
        break;
    };
    sched.run();
    delay(10);
}