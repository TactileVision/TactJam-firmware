#include <debug.h>

std::ostringstream tact::debug::debug_stream;

void tact::debug::println(const std::string& message, const tact::debug::DebugLevel level) {
  if (tact::debug::kDebugLevel == level) {
    Serial.printf("%s%s", message.c_str(), tact::debug::kSerialEndLine);
  }
  debug_stream.str("");
  debug_stream.clear();
}


void tact::debug::println(const std::string& func, const std::string& message, const tact::debug::DebugLevel level) {
  if (tact::debug::kDebugLevel == level) {
    Serial.printf("%s >>> %s%s", func.c_str(), message.c_str(), tact::debug::kSerialEndLine);
  }
  debug_stream.str("");
  debug_stream.clear();
}


void tact::debug::println(const std::string& file, const std::string& func, const std::string& message, const tact::debug::DebugLevel level) {
  if (tact::debug::kDebugLevel == level) {
    Serial.printf("%s @ %s >>> %s%s", file.c_str(), func.c_str(), message.c_str(), tact::debug::kSerialEndLine);
  }
  debug_stream.str("");
  debug_stream.clear();
}
