#ifndef _TACTJAM_ESPCONFIG_
#define _TACTJAM_ESPCONFIG_

#include <WiFi.h>

namespace tactjam {
namespace config {
  void DisableEspRadios() {
    WiFi.mode(WIFI_OFF);
    btStop();
  }

  void MonitorHeapSize() {
    auto free_heap_size = ESP.getFreeHeap();
    auto heap_size = ESP.getHeapSize();
    Serial.printf("heap size: %u bytes (%.2f kB)\tfree: %u bytes (%.2f kB)\n", heap_size, heap_size/1000.f, free_heap_size, free_heap_size/1000.f);
  }
}
}

#endif //_TACTJAM_ESPCONFIG_
