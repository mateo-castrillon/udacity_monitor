#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <numeric>

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
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
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

float LinuxParser::MemoryUtilization() {
  float totalMem = 0.0, freeMem = 0.0, buffers = 0.0, cached = 0.0;
  float sreclam = 0.0, shmem = 0.0;
  float value;
  std::string key;
  std::string line;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:")
          totalMem = value;
        else if  (key == "MemFree:")
          freeMem = value;
        else if  (key == "Buffers:")
          buffers = value;
        else if  (key == "Cached:")
          cached = value;
        else if  (key == "SReclaimable:")
          sreclam = value;
        else if  (key == "Shmem:")
          shmem = value;
        else
          continue;
      }
    }
  }
  //Total used memory - (Buffers + Cached memory)
  float usedMem = totalMem - freeMem;
  float cachedMem = cached + sreclam - shmem;

  //std::cout << "used memory: " << usedMem
  //          <<  " buff/cache: " <<  buffers + cachedMem << "\n";
  return (usedMem - (buffers + cachedMem)) / totalMem;
}

long LinuxParser::UpTime() {
  string line;
  long uptime = 0.0, idletime = 0.0;
  std::ifstream stream(kProcDirectory + kUptimeFilename);

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idletime;
    //std::cout << uptime << "\n";
  }
  return uptime - idletime;

}

/*
 *   user    nice   system  idle      iowait irq   softirq  steal  guest  guest_nice
cpu  74608   2520   24433   1117073   6176   4054  0        0      0      0
 */

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  std::vector<long> jiffies = CpuUtilization();
  std::vector<CPUStates> needed{kUser_, kNice_, kSystem_, kIdle_, kIOwait_, kIRQ_, kSoftIRQ_, kSteal_};
  long result = 0;
  for(auto const& state: needed){
    result += jiffies[state];
  }
  return result;
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string value;
  string key;
  string line;
  std::vector<string> jiffies;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key;
    while(linestream >> value)
      jiffies.emplace_back(value);
  }
  // add utime + stime + cutime + cstime
  long total = std::stol(jiffies[13]) + std::stol(jiffies[14])
               + std::stol(jiffies[15]) + std::stol(jiffies[16]);

  //jiffies are clock ticks (Hz), needed to convert to time???
  // this return gets compared with uptime which is not in Hz anymmore.
  return total / sysconf(_SC_CLK_TCK);
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  std::vector<long> jiffies = CpuUtilization();
  std::vector<CPUStates> needed{kUser_, kNice_, kSystem_, kIRQ_, kSoftIRQ_, kSteal_};
  long result = 0;
  for(auto const& state: needed){
    result += jiffies[state];
    //std::cout << state << "-----\n ";
  }
  return result;
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  std::vector<long> jiffies = CpuUtilization();
  std::vector<CPUStates> needed{kIdle_, kIOwait_};
  long result = 0;
  for(auto const& state: needed) {
    result += jiffies[state];
  }
  return result;
}

// Read and return CPU utilization,
// changed the signature to vector<long>, to avoid conversion in multiple cases later

vector<long> LinuxParser::CpuUtilization() {
  long value;
  string key;
  string line;
  std::vector<long> jiffies;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key;
    while(linestream >> value)
      jiffies.emplace_back(value);
  }

  return jiffies;
}

int LinuxParser::TotalProcesses() {
  int value = 0;
  string key;
  string line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") return value;
      }
    }
  }
  return 0;
}

int LinuxParser::RunningProcesses() {
  int value;
  string key;
  string line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") return value;
      }
    }
  }
  return 0;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string result;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open())
    std::getline(stream, result);
  return result;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {

  string line, key, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        // using VmRSS instrad VmSize because it gives the exact physical memory being used as part of physical RAM
        if (key == "VmRSS:")
          return value;
      }
    }
  }
  return value;
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {

  string line, key, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:")
          return value;
      }
    }
  }
  return value;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line, name, x, id;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> name >> x >> id;
      if (id == Uid(pid))
        break;
    }
  }
  return name;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line, key;
  string value;
  vector<string> values;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      values.emplace_back(value);
    }
  }
  //std::cout << values[21] << "----here--\n";

  return std::stol(values[21]) / sysconf(_SC_CLK_TCK);
}