#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

  void Update(long total, long idle);
  long PrevTotal();
  long PrevIdletotal();
  long CurrentTotal();
  long CurrentIdle();

 private:
  long oldTotal = 0;
  long oldIdle = 0;
};

#endif