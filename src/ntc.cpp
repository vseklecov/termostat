#include <avr/io.h>
#include <avr/pgmspace.h>

#include "ntc.h"

#define MAX_ADC (1023)

NTCProbe::NTCProbe(Scheduler &manager, ProcPriority pr, unsigned int period, byte p)
: Process(manager, pr, period), pin(p)
{
    analogReference(DEFAULT);
    pinMode(pin, INPUT);
}

void NTCProbe::setup()
{
    raw = analogRead(pin);
    samples = 1;
    // Конвертировать значение АЦП в температуру
    temp = calc_temperature(raw<<5);
}

void NTCProbe::service()
{
    raw += analogRead(pin);
    if (samples == 31)
    {
        temp = calc_temperature(raw);
        raw = raw >> 5;
        samples = 0;
    }
    samples++;
}

float NTCProbe::getTempC()
{
    return (float)temp/10.;
/*    float rT = BALANCE_RESISTOR * ((MAX_ADC/temp)-1);
    float tK = (BETA*ROOM_TEMP)/(BETA+(ROOM_TEMP*log(rT/RESISTOR_ROOM_TEMP)));
    float tC = tK-273.15;
    return tC;*/
}

// Функция вычисляет значение температуры в десятых долях градусов Цельсия
// в зависимости от суммарного значения АЦП.
int16_t NTCProbe::calc_temperature(temperature_table_entry_type adcsum) {
  temperature_table_index_type l = 0;
  temperature_table_index_type r = (sizeof(termo_table) / sizeof(termo_table[0])) - 1;
  temperature_table_entry_type thigh = TEMPERATURE_TABLE_READ(r);
  
  // Проверка выхода за пределы и граничных значений
  if (adcsum <= thigh) {
    #ifdef TEMPERATURE_OVER
      if (adcsum < thigh) 
        return TEMPERATURE_OVER;
    #endif
    return TEMPERATURE_TABLE_STEP * r + TEMPERATURE_TABLE_START;
  }
  temperature_table_entry_type tlow = TEMPERATURE_TABLE_READ(0);
  if (adcsum >= tlow) {
    #ifdef TEMPERATURE_UNDER
      if (adcsum > tlow)
        return TEMPERATURE_UNDER;
    #endif
    return TEMPERATURE_TABLE_START;
  }

  // Двоичный поиск по таблице
  while ((r - l) > 1) {
    temperature_table_index_type m = (l + r) >> 1;
    temperature_table_entry_type mid = TEMPERATURE_TABLE_READ(m);
    if (adcsum > mid) {
      r = m;
    } else {
      l = m;
    }
  }
  temperature_table_entry_type vl = TEMPERATURE_TABLE_READ(l);
  if (adcsum >= vl) {
    return l * TEMPERATURE_TABLE_STEP + TEMPERATURE_TABLE_START;
  }
  temperature_table_entry_type vr = TEMPERATURE_TABLE_READ(r);
  temperature_table_entry_type vd = vl - vr;
  int16_t res = TEMPERATURE_TABLE_START + r * TEMPERATURE_TABLE_STEP; 
  if (vd) {
    // Линейная интерполяция
    res -= ((TEMPERATURE_TABLE_STEP * (int32_t)(adcsum - vr) + (vd >> 1)) / vd);
  }
  return res;
}