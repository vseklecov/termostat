#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <Arduino.h>

// Датчик-щуп NTC термистор для измерения температуры продукта
#define PROBE_NTC       A0
// Термометр ds18b20 для измерения температуры в камере
#define ONE_WIRE_BUS    9

// Управление ветилятором
#define FAN_PIN         8
// Управление ТЭНом
#define HEATER_PIN      11
// Управление парогенератором
//#define STEAM_PIN       4
// Управление дымогенератором
//#define SMOKE_PIN       5
// Кнопка "Пуск"
//#define BUTTON_PIN      6
// Динамик
#define BUZZER_PIN      3
// Выбор режима
#define SPIN1    4
#define SPIN2    7

#endif