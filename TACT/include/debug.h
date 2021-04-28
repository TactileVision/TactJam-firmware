#ifndef _TACT_DEBUG_
#define _TACT_DEBUG_

#include <Arduino.h>
#include <iostream>
#include <sstream>


namespace tact {
namespace debug {

enum class DebugLevel {
  none,
  basic,
  verbose
};

#if TACT_DEBUG==0
#undef TACT_DEBUG
const auto kDebugLevel = DebugLevel::none;
#else
const auto kDebugLevel = (TACT_DEBUG == 1) ? DebugLevel::basic : DebugLevel::verbose;
#endif //TACT_DEBUG
const auto kSerialEndLine = "\r\n";

extern std::ostringstream debug_stream;

void println(const std::string& message, const DebugLevel level = DebugLevel::basic);


void println(const std::string& func, const std::string& message, const DebugLevel level = DebugLevel::basic);


void println(const std::string& file, const std::string& func, const std::string& message, const DebugLevel level = DebugLevel::basic);

}
}

#endif //_TACT_DEBUG_
