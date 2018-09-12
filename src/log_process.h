#ifndef __LOG_PROCESS_H__
#define __LOG_PROCESS_H__

#include <ProcessScheduler.h>

class LogProcess : public Process
{
public:
  LogProcess(Scheduler &manager, ProcPriority pr, unsigned int period)
      : Process(manager, pr, period)
  {}

  void setup(void);
  void service(void);

};

#endif