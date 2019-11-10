#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// TODO: Return this process's ID
int Process::Pid() { return m_pid; }

// TODO: Return this process's CPU utilization
void Process::CpuUtilization(long active_jiffies, long sys_jiffies) {
  m_cpu = static_cast<float>(active_jiffies - m_previous_active_jiffies) /
      (sys_jiffies - m_previous_sys_jiffies);
  m_previous_active_jiffies = active_jiffies;
  m_previous_sys_jiffies = sys_jiffies;
}
float Process::CpuUtilization() const { return m_cpu; }

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(m_pid); }

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(m_pid); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(m_pid); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(m_pid); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
  return CpuUtilization() < a.CpuUtilization();
}
bool Process::operator>(Process const& a) const {
  return CpuUtilization() > a.CpuUtilization();
}