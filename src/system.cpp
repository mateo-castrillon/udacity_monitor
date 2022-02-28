#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;
/*You need to complete the mentioned TODOs in order to satisfy the rubric
 * criteria "The student will be able to extract and display basic data about the system."
*/
//Return the system's CPU
Processor& System::Cpu() {
  return cpu_;
}

// Return a container composed of the system's processes
vector<Process>& System::Processes() {
  vector<int> pids = LinuxParser::Pids();
  processes_.clear();
  for (auto const& pid: pids){
    Process proc(pid);
    processes_.emplace_back(proc);
  }

  std::sort(processes_.rbegin(), processes_.rend());

  return processes_;
}

std::string System::Kernel() {
  return LinuxParser::Kernel();
}

float System::MemoryUtilization() {
  return LinuxParser::MemoryUtilization();
}

std::string System::OperatingSystem() {
  return LinuxParser::OperatingSystem();
}

int System::RunningProcesses() {
  return LinuxParser::RunningProcesses();
}

int System::TotalProcesses() {
  return LinuxParser::TotalProcesses();
}

long int System::UpTime() {
  return LinuxParser::UpTime();
}
