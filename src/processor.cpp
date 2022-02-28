#include "processor.h"
#include <string>
#include <unistd.h>
#include <vector>
#include<iostream>

#include "process.h"
#include "linux_parser.h"

float Processor::Utilization() {
  float totald, idled, result;

  long total = CurrentTotal();
  long idle = CurrentIdle();
  long prevTotal = PrevTotal();
  long prevIdle = PrevIdletotal();

  totald = float(total - prevTotal);
  idled = float(idle - prevIdle);

  result = (totald - idled)/totald;
  //std::cout << result << " " << totald - idled <<"\n";
  Update(total, idle);
  return result;
}

void Processor::Update(long total, long idle){
  oldTotal = total;
  oldIdle = idle;
}

long Processor::PrevTotal(){
  return oldTotal;
}
long Processor::PrevIdletotal(){
  return oldIdle;
}
long Processor::CurrentTotal(){
  return LinuxParser::Jiffies();
}
long Processor::CurrentIdle(){
  return LinuxParser::IdleJiffies();
}

