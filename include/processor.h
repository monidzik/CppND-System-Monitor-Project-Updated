#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  long m_previous_active_jiffies{0};
  long m_previous_idle_jiffies{0};
};

#endif