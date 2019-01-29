#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <Arduino.h>

// ������-��� NTC ��������� ��� ��������� ����������� ��������
#define PROBE_NTC       A0
// ��������� ds18b20 ��� ��������� ����������� � ������
#define ONE_WIRE_BUS    9

// ���������� �����������
#define FAN_PIN         8
// ���������� �����
#define HEATER_PIN      11
#define HEATER_ON       bitSet(PORTB, 3)
#define HEATER_OFF      bitClear(PORTB, 3)
#define HEATER2_PIN     12
#define HEATER2_ON      bitSet(PORTB, 4)
#define HEATER2_OFF     bitClear(PORTB, 4)
#define HEATER12_ON     (PORTB |= B11000)
#define HEATER12_OFF    (PORTB &= B100111)

// ���������� ���������������
//#define STEAM_PIN       4
// ���������� ���������������
//#define SMOKE_PIN       5
// ������ "����"
//#define BUTTON_PIN      6
// �������
#define BUZZER_PIN      3
// ����� ������
#define SPIN1    4
#define SPIN2    7

#define LENGTH_PARAM_PID (sizeof(double)*3)
#define LENGTH_PARAM_TERMOSTAT (sizeof(int8_t)*6)
#define LENGTH_PARAM_EEPROM (LENGTH_PARAM_TERMOSTAT+LENGTH_PARAM_PID)

#endif