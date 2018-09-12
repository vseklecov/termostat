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

#endif