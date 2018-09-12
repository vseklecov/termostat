#include "ts_process.h"
/*
Termostat_Process::State::State(f_tp_void on_enter, f_tp_void on_state, f_tp_void on_exit)
:   on_enter(on_enter),
    on_state(on_state),
    on_exit(on_exit)
{
}

Termostat_Process::Termostat_Process(Scheduler &manager, ProcPriority pr, unsigned int period, State* initial_state)
:   Process(manager, pr, period),
    m_current_state(initial_state),
    m_transitions(NULL),
    m_num_transitions(0),
    m_num_timed_transitions(0),
    m_initialized(false)
{
}

Termostat_Process::~Termostat_Process()
{
    free(m_transitions);
    free(m_timed_transitions);
    m_transitions = NULL;
    m_timed_transitions = NULL;
}

void Termostat_Process::add_transition(State* state_from, State* state_to, int event,
                         void (Termostat_Process::*on_transition)())
{
    if (state_from == NULL || state_to == NULL)
        return;

    Transition transition = Termostat_Process::create_transition(state_from, state_to, event,
                                                on_transition);
    m_transitions = (Transition*) realloc(m_transitions, (m_num_transitions + 1)
                                                        * sizeof(Transition));
    m_transitions[m_num_transitions] = transition;
    m_num_transitions++;
}

void Termostat_Process::add_timed_transition(Termostat_Process::State* state_from, Termostat_Process::State* state_to,
                               unsigned long interval, void (Termostat_Process::*on_transition)())
{
    if (state_from == NULL || state_to == NULL)
        return;

    Transition transition = Termostat_Process::create_transition(state_from, state_to, 0,
                                                    on_transition);

    TimedTransition timed_transition;
    timed_transition.transition = transition;
    timed_transition.start = 0;
    timed_transition.interval = interval;

    m_timed_transitions = (TimedTransition*) realloc(
        m_timed_transitions, (m_num_timed_transitions + 1) * sizeof(TimedTransition));
    m_timed_transitions[m_num_timed_transitions] = timed_transition;
    m_num_timed_transitions++;
}

Termostat_Process::Transition Termostat_Process::create_transition(Termostat_Process::State* state_from, Termostat_Process::State* state_to,
                                       int event, void (Termostat_Process::*on_transition)())
{
    Transition t;
    t.state_from = state_from;
    t.state_to = state_to;
    t.event = event;
    t.on_transition = on_transition;

    return t;
}

void Termostat_Process::trigger(int event)
{
    if (m_initialized)
    {
        // Find the transition with the current state and given event.
        for (int i = 0; i < m_num_transitions; ++i)
        {
            if (m_transitions[i].state_from == m_current_state &&
                m_transitions[i].event == event)
            {
                Termostat_Process::make_transition(&(m_transitions[i]));
                return;
            }
        }
    }
}

void Termostat_Process::check_timed_transitions()
{
    for (int i = 0; i < m_num_timed_transitions; ++i)
    {
        TimedTransition* transition = &m_timed_transitions[i];
        if (transition->transition.state_from == m_current_state)
        {
            if (transition->start == 0)
            {
                transition->start = millis();
            }
            else{
                unsigned long now = millis();
                if (now - transition->start >= transition->interval)
                {
                    Termostat_Process::make_transition(&(transition->transition));
                    transition->start = 0;
                }
            }
        }
    }
}

void Termostat_Process::run_machine()
{
    // first run must exec first state "on_enter"
    if (!m_initialized)
    {
        m_initialized = true;
        if (m_current_state->on_enter != NULL)
            (this->*(m_current_state->on_enter))();
    }
    
    if (m_current_state->on_state != NULL)
        (this->*(m_current_state->on_state))();
        
    Termostat_Process::check_timed_transitions();
}

void Termostat_Process::make_transition(Transition* transition)
{
    // Execute the handlers in the correct order.
    if (transition->state_from->on_exit != NULL)
        (this->*(transition->state_from->on_exit))();

    if (transition->on_transition != NULL)
        (this->*(transition->on_transition))();

    if (transition->state_to->on_enter != NULL)
        (this->*(transition->state_to->on_enter))();
    
    m_current_state = transition->state_to;

    //Initialice all timed transitions from m_current_state
    unsigned long now = millis();
    for (int i = 0; i < m_num_timed_transitions; ++i)
    {
        TimedTransition* ttransition = &m_timed_transitions[i];
        if (ttransition->transition.state_from == m_current_state)
            ttransition->start = now;
    }
}
*/