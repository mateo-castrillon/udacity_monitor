#include <string>

#include "format.h"

using std::string;

// using implementation from
//https://stackoverflow.com/questions/58695875/how-to-convert-seconds-to-hhmmss-millisecond-format-c
string Format::ElapsedTime(long seconds) {

  std::string h = std::to_string(seconds / 3600);
  std::string m = std::to_string((seconds % 3600) / 60);
  std::string s = std::to_string(seconds % 60);

  std::string hh = std::string(2 - h.length(), '0') + h;
  std::string mm = std::string(2 - m.length(), '0') + m;
  std::string ss = std::string(2 - s.length(), '0') + s;

  return hh + ':' + mm + ":" + ss;
}