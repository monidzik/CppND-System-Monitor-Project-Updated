#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line1, line2;
  string name;
  string memory_total;
  string memory_free;
  std::ifstream filestream(LinuxParser::kProcDirectory + LinuxParser::kMeminfoFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line1);
    std::istringstream linestream1(line1);
    linestream1 >> name >> memory_total;
    std::getline(filestream, line2);
    std::istringstream linestream2(line2);
    linestream2 >> name >> memory_free;
  }
  return (stof(memory_total) - stof(memory_free)) / stof(memory_total) * 100;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  string line, value;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
  }
  return stol(value);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpu_usages{};
  std::ifstream stream(kProcDirectory + kStatFilename);
  string line, name, user, nice, system, idle, iowait, irq, softirq, steal;
  if (stream.is_open()) {
    int i = 0;
    while (std::getline(stream, line)) {
      float total_cpu_time_since_boot = 0;
      float total_cpu_idle_time_since_boot = 0;
      float total_cpu_usage_since_boot = 0;
      std::istringstream linestream(line);
      linestream >> name >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
      if (i > 11){
        break;
      }
      i++;
      total_cpu_time_since_boot = stoi(user) + stoi(nice) + stoi(system) + stoi(idle) + stoi
          (iowait) + stoi(irq) + stoi(softirq) + stoi(steal);
      total_cpu_idle_time_since_boot =stoi(idle) + stoi(iowait);
      total_cpu_usage_since_boot = total_cpu_time_since_boot - total_cpu_idle_time_since_boot;
      string total_cpu_usage =
          to_string(total_cpu_usage_since_boot / total_cpu_time_since_boot * 100) + "%";
      cpu_usages.push_back(total_cpu_usage);
    }
  }
  return cpu_usages;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  string line, key, value;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          std::cout << stoi(value) << std::endl;
          return stoi(value);
        }
      }
    }
  }
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  string line, key, value;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return stoi(value);
        }
      }
    }
  }
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  string line;
  if (stream.is_open()) {
    std::getline(stream, line);
  }
  return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  string line, key, value;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          return to_string(stoi(value)*0.001);
        }
      }
    }
  }
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  string line, key, value;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          return value;
        }
      }
    }
  }
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  std::ifstream stream(kPasswordPath);
  string line, key, x, value, rest;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> x >> value) {
        if (value == LinuxParser::Uid(pid)) {
          return key;
        }
      }
    }
  }
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  string line, value;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::replace(line.begin(), line.end(), ' ', '\n');
    std::istringstream linestream(line);
    for(int i = 0; i < 23; ++i){
      linestream >> value;
    }
    linestream >> value;
    return stol(value)/sysconf(_SC_CLK_TCK);
  }
}