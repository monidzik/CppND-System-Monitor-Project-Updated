#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
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
  string name, value;
  float memory_total{1};
  float memory_free{0};
  float memory_buffer{0};
  std::ifstream filestream(LinuxParser::kProcDirectory + LinuxParser::kMeminfoFilename);
  if (filestream.is_open()) {
    while (filestream >> value) {
      if (value == "MemTotal:") {
        if (filestream >> value) {
          memory_total = stof(value);
        }
      }
      else if (value == "MemFree:") {
        if (filestream >> value) {
          memory_free = stof(value);
        }
      }
      else if (value == "Buffers:") {
        if (filestream >> value) {
          memory_buffer = stof(value);
        }
      }
    }
  }
  return 1 - (memory_free / (memory_total - memory_buffer));
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  string line, value;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    if (linestream >> value) {
      return stol(value);
    }
  }
  return 0;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return UpTime() * sysconf(_SC_CLK_TCK);
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  vector<string> stat_inputs;
  string line, value;
  long active_jiffies = 0;
  std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kStatFilename);
  if(stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      stat_inputs.push_back(value);
    }
  }
  if (stat_inputs.size() > 21) {
    active_jiffies = stol(stat_inputs[13]) + stol(stat_inputs[14]) + stol(stat_inputs[15]) +
        stol(stat_inputs[16]);
  }
  return active_jiffies;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> cpu_util = CpuUtilization();
  long active_jiffies = stol(cpu_util[CPUStates::kUser_]) + stol(cpu_util[CPUStates::kNice_]) +
                        stol(cpu_util[CPUStates::kSystem_]) + stol(cpu_util[CPUStates::kIRQ_]) +
                        stol(cpu_util[CPUStates::kSoftIRQ_]) + stol(cpu_util[CPUStates::kSteal_]) +
                        stol(cpu_util[CPUStates::kGuest_]) +
                        stol(cpu_util[CPUStates::kGuestNice_]);
  return active_jiffies;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpu_util = CpuUtilization();
  long active_jiffies = stol(cpu_util[CPUStates::kIdle_]) + stol(cpu_util[CPUStates::kIOwait_]);
  return active_jiffies;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpu_usages;
  std::ifstream stream(kProcDirectory + kStatFilename);
  string line, value;
  if(stream.is_open()){
    while (getline(stream, line)){
      std::istringstream linestream(line);
      while (linestream >> value) {
        if (value == "cpu") {
          while (linestream >> value) {
            cpu_usages.push_back(value);
          }
        }
      }
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
          return stoi(value);
        }
      }
    }
  }
  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  string line;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      string key, value;
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return stoi(value);
        }
      }
    }
  }
  return 0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    string line;
    std::getline(stream, line);
    return line;
  }
  return string("");
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
          return to_string(stoi(value) / 1024);
        }
      }
    }
  }
  return string("0");
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
  return string("");
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
  return string("");
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
  return 0;
}