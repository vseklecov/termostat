#ifndef __CONFIG_H__
#define __CONFIG_H__

// Датчик-щуп NTC термистор для измерения температуры продукта
#define PROBE_NTC   A0
// Термометр ds18b20 для измерения температуры в камере
#define ONE_WIRE_BUS  1

// Управление ветилятором
#define FAN_PIN     2
// Управление ТЭНом
#define HEATER_PIN  3
// Управление парогенератором
#define STEAM_PIN   4
// Управление дымогенератором
#define SMOKE_PIN   5
// Кнопка "Пуск"
#define BUTTON_PIN  6

#endif