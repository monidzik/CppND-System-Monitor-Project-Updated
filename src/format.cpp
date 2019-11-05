#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  string hours = std::to_string(seconds / 3600);
  string minutes = std::to_string((seconds / 60) % 60);
  string seconds_s = std::to_string(seconds % 60);
  if(hours.size() < 2) hours += '0';
  if(minutes.size() < 2) minutes += '0';
  if(seconds_s.size() < 2) seconds_s += '0';
  return hours+":"+minutes+":"+seconds_s;
}
