#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid_) {
  pid = pid_;
}

int Process::Pid() const {
  return pid;
}

// Return this process's CPU utilization
float Process::CpuUtilization() const {

  auto total_time = float(LinuxParser::ActiveJiffies(Pid()));
  auto up = float(LinuxParser::UpTime(Pid()));

  auto seconds = float(LinuxParser::UpTime())  - up;

  return ((total_time) / seconds);
}

// Return the command that generated this process
string Process::Command() const {
  return LinuxParser::Command(Pid());
}

// Return this process's memory utilization
string Process::Ram() const {
  return LinuxParser::Ram(Pid());
}

// Return the user (name) that generated this process
string Process::User() const {
  return LinuxParser::User(Pid());
}

// Return the age of this process (in seconds)
long int Process::UpTime() const {
  return LinuxParser::UpTime(Pid());
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return (CpuUtilization() < a.CpuUtilization());
}

