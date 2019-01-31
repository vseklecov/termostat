
#include <Arduino.h>
#include <EEPROM.h>

#include <SerialCommand.h>
SerialCommand sCmd;

#include "config.h"

#include <MsTimer2.h>

#include <LiquidCrystal_I2C.h>

#include <ProcessScheduler.h>
Scheduler sched;

#include <OneWire.h>
OneWire ow(ONE_WIRE_BUS);
#define REQUIRESNEW false
#define REQUIRESALARMS false
#include <DallasTemperature.h>
DallasTemperature dt(&ow);
DeviceAddress term_addr;

//#include "ds_process.h"
//DSProcess ds(sched, HIGH_PRIORITY, 750, &dt);
double Setpoint, Input, Output;
// Упраление мощностью ТЭНа по алгоритму Брезенхема
volatile int8_t power = 0;
void powerControl();
void powerControl2();

#include "ntc.h"
NTCProbe probe(sched, HIGH_PRIORITY, SERVICE_CONSTANTLY, PROBE_NTC);

#include "termostat.h"
TermostatProcess tp(sched, HIGH_PRIORITY, SERVICE_SECONDLY, Input, Setpoint, probe);

#include "log_process.h"
LogProcess lp(sched, LOW_PRIORITY, 1000);
LiquidCrystal_I2C lcd(0x27, 16, 2);

#include <PID_v1.h>
double Kp, Ki, Kd;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, P_ON_E, DIRECT);

void setKp();
void setKi();
void setKd();
void getParam();
void unrecognized(const char *);

void writeEEPROM()
{
    unsigned int i;
    int8_t sum;

    i = tp.writeEEPROM();
    EEPROM.put(i, Kp);
    i += sizeof(Kp);
    EEPROM.put(i, Ki);
    i += sizeof(Ki);
    EEPROM.put(i, Kd);
    sum = 0;
    for (i = 0; i < LENGTH_PARAM_EEPROM; i++)
        sum += EEPROM.read(i);
    EEPROM.write(i, sum ^ 0xE5);
}

void readEEPROM()
{
    unsigned int i = 0;
    int8_t sum = 0;

    for (; i < LENGTH_PARAM_EEPROM; i++)
        sum += EEPROM.read(i);
    //if ((sum ^ 0xE5) == EEPROM.read(i))
    //{
        i = tp.readEEPROM();
        Kp = EEPROM.get(i, Kp);
        i += sizeof(Kp);
        Ki = EEPROM.get(i, Ki);
        i += sizeof(Ki);
        Kd = EEPROM.get(i, Kd);
    //}
    //else
    //{
    //    i = tp.initEEPROM();
    //    Kp = 20;
    //    EEPROM.put(i, Kp);
    //    i += sizeof(Kp);
    //    Ki = 0.01;
    //    EEPROM.put(i, Ki);
    //    i += sizeof(Ki);
    //    Kd = 10;
    //    EEPROM.put(i, Kd);
    //    i += sizeof(Kd);
    //    sum = 0;
    //    for (i = 0; i < LENGTH_PARAM_EEPROM; i++)
    //        sum += EEPROM.read(i);
    //    EEPROM.write(i, sum ^ 0xE5);
    //}
}

void setup()
{

    Serial.begin(9600);
    
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Termostat v 1.0");

    pinMode(ONE_WIRE_BUS, INPUT);
    dt.begin();
    if (dt.getDS18Count() > 0)
    {
        dt.getAddress(term_addr, 0);
        dt.setResolution(9);
    }

    probe.add(true);
    tp.add(true);
    lp.add(true);

    pinMode(FAN_PIN, OUTPUT);

    pinMode(HEATER_PIN, OUTPUT);
    //setPwmFrequencyUNO(HEATER_PIN, 7);

    //pinMode(STEAM_PIN, OUTPUT);
    //pinMode(SMOKE_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);

    pinMode(SPIN1, INPUT);
    pinMode(SPIN2, INPUT);

    readEEPROM();
    myPID.SetTunings(Kp, Ki, Kp);
    myPID.SetMode(MANUAL);
    myPID.SetOutputLimits(0, 100);

    sCmd.addCommand("Kp", setKp);
    sCmd.addCommand("Ki", setKi);
    sCmd.addCommand("Kd", setKd);
    sCmd.addCommand("W", writeEEPROM);
    sCmd.addCommand("GET", getParam);
    sCmd.setDefaultHandler(unrecognized);

    MsTimer2::set(100, powerControl2);
    MsTimer2::start();
}

bool getTempCamera()
{
    float temp;

    if (dt.getDS18Count() == 0)
    {
        dt.begin();
        if (dt.getDS18Count() > 0)
        {
            dt.getAddress(term_addr, 0);
            dt.setResolution(9);
        } else
            return false;
    }
    dt.requestTemperatures();
    temp = dt.getTempC(term_addr);
    if (temp == DEVICE_DISCONNECTED_C)
        return false;
    else
        Input = temp;
    
    return true;
}

void loop()
{
    if (getTempCamera())
    {
        switch (tp.cur_state)
        {
        case tp.HEATING:
        case tp.WARMING:
        case tp.FRYING:
        case tp.WAIT:
        case tp.COOKING:
            if (myPID.GetMode() == MANUAL)
                myPID.SetMode(AUTOMATIC);
            if (myPID.Compute())
            {
                if (Input > Setpoint)
                    power = 0;
                else
                    power = Output;
            }
            break;
        default:
            power = 0;
            break;
        };
    } else
    {
        power = 0;
    }
    
    sched.run();
    sCmd.readSerial();
}

void setKp()
{
    char *arg;

    arg = sCmd.next();
    if (arg != NULL)
        Kp = atof(arg);
    myPID.SetTunings(Kp, Ki, Kp);
}

void setKi()
{
    char *arg;

    arg = sCmd.next();
    if (arg != NULL)
        Ki = atof(arg);
    myPID.SetTunings(Kp, Ki, Kp);
}

void setKd()
{
    char *arg;

    arg = sCmd.next();
    if (arg != NULL)
        Kd = atof(arg);
    myPID.SetTunings(Kp, Ki, Kp);
}

void getParam()
{
    double temp;
    unsigned int i;

    Serial.print("Kp=");
    Serial.print(Kp);
    Serial.print("; Ki=");
    Serial.print(Ki);
    Serial.print("; Kd=");
    Serial.print(Kd);
    Serial.print("; EEPROM=");
    for (i=0; i<LENGTH_PARAM_TERMOSTAT; i+=sizeof(int8_t))
    {
        Serial.print(EEPROM.read(i));
        Serial.print(", ");
    }
    Serial.print(EEPROM.get(i, temp));
    Serial.print(", ");
    i += sizeof(Kp);
    Serial.print(EEPROM.get(i, temp));
    Serial.print(", ");
    i += sizeof(Ki);
    Serial.print(EEPROM.get(i, temp));
    Serial.print(", CRC=");
    i += sizeof(Kd);
    Serial.print(EEPROM.read(i), HEX);
    Serial.println(';');
}

void unrecognized(const char *command)
{
    Serial.println("What?");
}

// Упраление мощностью ТЭНа по алгоритму Брезенхема, прерывание 10 раз в сек.
void powerControl()
{
    // power - заданная мощность
    // error - ошибка
    static int8_t error = 0;
    int8_t reg = power + error;
    if (reg < 50)
    {
        HEATER_OFF;
        error = reg;
    }
    else
    {
        HEATER_ON;
        error = reg - 100;
    }
}

// Упраление мощностью двумя ТЭНами по алгоритму Брезенхема, прерывание 10 раз в сек.
void powerControl2()
{
    // power - заданная мощность
    // error - ошибка
    static int8_t error = 0;
    int8_t reg = power + error;
    if (reg < 50)
    {
        HEATER12_OFF;
        error = reg;
    }
    else
    {
        HEATER12_ON;
        error = reg - 100;
    }
}

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
/*void setPwmFrequencyUNO(int pin, int divisor)
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
            TCCR0B = (TCCR0B & 0b11111000) | mode;
        }
        else
        {
            TCCR1B = (TCCR1B & 0b11111000) | mode;
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
        TCCR2B = (TCCR2B & 0b11111000) | mode;
    }
}*/

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
