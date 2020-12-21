#ifndef _TACTJAM_DECODE_VTP_
#define _TACTJAM_DECODE_VTP_

#include <vector>
#include <Arduino.h>
#include <vtp_codec.h>


namespace tactjam {
namespace data_format {

class VTPDecoder {
private:
  const std::vector<VTPInstructionWord> example_instructions_{
    0x100000ea, 0x2000007b, 0x10200159, 0x1020c9c8,
    0x10100315, 0x000007d0, 0x200000ea, 0x10200237
  };

  void PrintParametersA(VTPInstructionParamsA* params) {
    Serial.printf("value: %u\n", params->parameter_a);
  }

  void PrintParametersB(VTPInstructionParamsB* params) {
    Serial.printf("actuator: %s\t", (params->channel_select == 0) ? "all" : String(params->channel_select));
    Serial.printf("value: %d\t", params->parameter_a);
    if (params->time_offset > 0) {
      Serial.printf("time offset: +%ums\n", params->time_offset);
    }
    else {
      Serial.println();
    }
  }


public:
  VTPDecoder() = default;

  void DecodeExample() {
    Decode(example_instructions_);
  }

  void Decode(const std::vector<VTPInstructionWord>& instruction_words) {
    VTPInstructionV1 instruction;
    auto idx = 0;
    for (auto instructions_word : instruction_words) {
      Serial.printf("[%d]\t", ++idx);
      VTPError error = vtp_decode_instruction_v1(instructions_word, &instruction);
      if (error != VTP_OK) {
        Serial.print("Error decoding instruction word: Code ");
        Serial.println(error);
        continue;
      }
      switch (instruction.code) {
        case VTP_INST_INCREMENT_TIME:
          Serial.print("Increment Time\t");
          PrintParametersA(&instruction.params.format_a);
          break;
        case VTP_INST_SET_AMPLITUDE:
          Serial.print("Set Amplitude\t");
          PrintParametersB(&instruction.params.format_b);
          break;
        case VTP_INST_SET_FREQUENCY:
          Serial.print("Set Frequency\t");
          PrintParametersB(&instruction.params.format_b);
          break;        
      }
    }
    Serial.println();
  }
};

}
}

#endif //_TACTJAM_DECODE_VTP_
