#ifndef __NTC_H__
#define __NTC_H__

#include <ProcessScheduler.h>

// �������� �����������, ������������ ���� ����� ����������� ��� ������ ������� �������� �������
#define TEMPERATURE_UNDER -1
// �������� �����������, ������������ ���� ����� ����������� ��� ������ ���������� �������� �������
#define TEMPERATURE_OVER 1001
// �������� ����������� ��������������� ������� �������� �������
#define TEMPERATURE_TABLE_START 0
// ��� ������� 
#define TEMPERATURE_TABLE_STEP 50
// ����� ������� � �������� �������, ������ ��������������� temperature_table_entry_type
#define TEMPERATURE_TABLE_READ(i) pgm_read_word(&termo_table[i])

// ��� ������� �������� � �������, ���� ����� ������� � �������� 16 ��� - uint16_t, ����� - uint32_t
typedef uint16_t temperature_table_entry_type;
// ��� ������� �������. ���� � ������� ������ 256 ���������, �� uint16_t, ����� - uint8_t
typedef uint8_t temperature_table_index_type;

/* ������� ���������� �������� ��� � ����������� �� �����������. �� �������� �������� � ��������
   ��� ���������� ������� ������������ ��������� ����������:
     R1(T1): 284���(18��)
     R2(T2): 30���(70��)
     ����� ���������: A
     Ra: 100���
     ���������� U0/Uref: 5�/5�
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

    int16_t    temp;     // ����������� ���������� �� ����

private:
    byte pin;
//    int16_t    temp;     // ����������� ���������� �� ����
    uint32_t   raw;
    uint8_t    samples;

    int16_t calc_temperature(temperature_table_entry_type);

    const float BALANCE_RESISTOR = 200000.0;
    const float BETA = 3974.0;
    const float ROOM_TEMP = 298.15;
    const float RESISTOR_ROOM_TEMP = 220000.0;
};

#endif