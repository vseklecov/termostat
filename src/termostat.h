#ifndef __TERMOSTAT_H__
#define __TERMOSTAT_H__

// #include <Arduino.h>
// #include <Fsm.h>

// #define HEAT        1
// #define COOKING     2
// #define END         3 

// State state_stop(&heater_off, &check_heat, NULL);
// State state_heat(&heater_on, &check_min_temp, NULL);
// State state_cooking(NULL, &check_cooking_temp, NULL);
// State state_end(&heater_off, NULL, NULL);

// Fsm fsm_termostat(&state_stop);

// void heater_off()
// {

// }

// void heater_on()
// {

// }

// void check_heat()
// {
//     if (true)
//         fsm_termostat.trigger(HEAT);
// }

// void check_min_temp()
// {
//     if (true)
//         fsm_termostat.trigger(COOKING);
// }

// void check_cooking_temp()
// {
//     if (true)
//         fsm_termostat.trigger(END);
// }

// void termostat_setup()
// {
//     fsm_termostat.add_transition(&state_stop, &state_heat, HEAT, NULL);
//     fsm_termostat.add_transition(&state_heat, &state_cooking, COOKING, NULL);
//     fsm_termostat.add_transition(&state_cooking, &state_end, END, NULL);
    
// }

// void termostat_service()
// {
//     fsm_termostat.run_machine();
// }

#endif