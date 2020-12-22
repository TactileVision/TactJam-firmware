#ifndef _TACT_CONFIG_
#define _TACT_CONFIG_

namespace tact {
namespace config {

  const auto serial_baud_rate = 115200;

  #ifdef TACT_DEBUG
  enum class DebugLevel {
    basic,
    verbose
  };
  const auto debug_level = (TACT_DEBUG == 1) ? DebugLevel::basic : DebugLevel::verbose;
  #endif //TACT_DEBUG

}
}

#endif //_TACT_CONFIG_
