#ifndef __NTC_H__
#define __NTC_H__

#include <ProcessScheduler.h>

// Значение температуры, возвращаемое если сумма результатов АЦП больше первого значения таблицы
#define TEMPERATURE_UNDER -1
// Значение температуры, возвращаемое если сумма результатов АЦП меньше последнего значения таблицы
#define TEMPERATURE_OVER 1001
// Значение температуры соответствующее первому значению таблицы
#define TEMPERATURE_TABLE_START 0
// Шаг таблицы 
#define TEMPERATURE_TABLE_STEP 50
// Метод доступа к элементу таблицы, должна соответствовать temperature_table_entry_type
#define TEMPERATURE_TABLE_READ(i) pgm_read_word(&termo_table[i])

// Тип каждого элемента в таблице, если сумма выходит в пределах 16 бит - uint16_t, иначе - uint32_t
typedef uint16_t temperature_table_entry_type;
// Тип индекса таблицы. Если в таблице больше 256 элементов, то uint16_t, иначе - uint8_t
typedef uint8_t temperature_table_index_type;

/* Таблица суммарного значения АЦП в зависимости от температуры. От большего значения к меньшему
   Для построения таблицы использованы следующие парамертры:
     R1(T1): 284кОм(18°С)
     R2(T2): 30кОм(70°С)
     Схема включения: A
     Ra: 100кОм
     Напряжения U0/Uref: 5В/5В
*/
const temperature_table_entry_type termo_table[] PROGMEM = {
    28935, 27863, 26607, 25173, 23581, 21863, 20061, 18223,
    16398, 14629, 12953, 11397, 9976, 8698, 7562, 6562,
    5688, 4930, 4274, 3709, 3222
};

class NTCProbe: public Process
{
public:
    NTCProbe(Scheduler&, ProcPriority, unsigned int, byte);

    void setup();
    void service();

    float getTempC();
    
    //int32_t getRaw() { return raw/samples; }

    int16_t    temp;     // Усредненное напряжение на щупе

private:
    byte pin;
//    int16_t    temp;     // Усредненное напряжение на щупе
    uint32_t   raw;
    uint8_t    samples;

    int16_t calc_temperature(temperature_table_entry_type);

    const float BALANCE_RESISTOR = 200000.0;
    const float BETA = 3974.0;
    const float ROOM_TEMP = 298.15;
    const float RESISTOR_ROOM_TEMP = 220000.0;
};

#endif