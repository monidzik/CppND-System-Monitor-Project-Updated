#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  long active_jiffies = LinuxParser::ActiveJiffies();
  long idle_jiffies = LinuxParser::IdleJiffies();
  long active_utilization{LinuxParser::ActiveJiffies() - m_previous_active_jiffies};
  long idle_utilization{LinuxParser::IdleJiffies() - m_previous_idle_jiffies};
  m_previous_active_jiffies = active_jiffies;
  m_previous_idle_jiffies = idle_jiffies;
  return static_cast<float>(active_utilization) / (active_utilization + idle_utilization);
}