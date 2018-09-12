#ifndef __TS_PROCESS_H__
#define __TS_PROCESS_H__
/*
#include <Arduino.h>
#include <ProcessScheduler.h>

class Termostat_Process;
typedef void (Termostat_Process::* f_tp_void)();

class Termostat_Process: public Process
{
public:

    struct State
    {
        State(f_tp_void, f_tp_void, f_tp_void);
        f_tp_void on_enter;
        f_tp_void on_state;
        f_tp_void on_exit;
    };

    Termostat_Process(Scheduler&, ProcPriority, unsigned int, State*);
    ~Termostat_Process();
    

    void add_transition(State* state_from, State* state_to, int event,
                        f_tp_void on_transition);
    void add_timed_transition(State* state_from, State* state_to,
                              unsigned long interval, f_tp_void on_transition);
    void check_timed_transitions();
    void trigger(int event);
    void run_machine();

    void setup();
    void service();
    bool recive(char ch);

private:

    struct Transition
    {
        State* state_from;
        State* state_to;
        int event;
        f_tp_void on_transition;
    };

    struct TimedTransition
    {
        Transition transition;
        unsigned long start;
        unsigned long interval;
    };
    
    static Transition create_transition(State* state_from, State* state_to,
                                        int event, f_tp_void on_transition);
    void make_transition(Transition* transition);

    State* m_current_state;
    Transition* m_transitions;
    int m_num_transitions;

    TimedTransition* m_timed_transitions;
    int m_num_timed_transitions;
    bool m_initialized;
};
*/
#endif