#include <dataTransfer.h>
#include <sstream>

namespace tact {

#define COM_SEND (uint8_t)1
#define COM_RECEIVE (uint8_t)2
#define COM_CONNECT (uint8_t)3
//#define COM_DEBUG (uint8_t)4
#define COM_MESSAGE_FOLLOW_UP "\r\n"

DataTransfer::DataTransfer(Peripherals* peripherals, Sampler* sampler) {
    peripherals_ = peripherals;
    sampler_ = sampler;
}


void DataTransfer::SetState(DataState state, std::string line_2, bool force_display_update) {
  #ifdef TACT_DEBUG
  debug::println(__FILE__, __func__, "", debug::DebugLevel::verbose);
  #endif //TACT_DEBUG

  if ((state != this->state) || (force_display_update == true)) {
    std::string line_1;
    switch(state) {
      case DataState::idle: 
        line_1.assign("idle");
        if ( line_2.empty() == true && sampler_->GetTactonSizeCurrentSlot() == 0) {
          line_2.assign("empty"); 
        }
        break;
      case DataState::send:
        line_1.assign("send");
        break;
      case DataState::receive:
        line_1.assign("receive");
        break;
    }
    peripherals_->display.DrawContentTeaserDoubleLine(line_1.c_str(), line_2.c_str());
  }
  this->state = state;
}


void DataTransfer::Reset() {
  #ifdef TACT_DEBUG
  debug::println(__FILE__, __func__, "", debug::DebugLevel::verbose);
  #endif //TACT_DEBUG
  
  SetState(DataState::idle, "", true);
}


void DataTransfer::ReceiveButtonPressed(uint8_t slot) {
  #ifdef TACT_DEBUG
  debug::println(__FILE__, __func__, "");
  #endif //TACT_DEBUG

  if (connection_confirmed == false) {
      peripherals_->buzzer.PlayFail();
      SetState(DataState::idle, "NO CLIENT", true);
      return;
  }

  if ((slot < 1) || (slot >= TACTONS_COUNT_MAX)) {
    #ifdef TACT_DEBUG
    debug::debug_stream << " ERROR: slot " << unsigned(slot) << " not allowed";
    debug::println(__func__, debug::debug_stream.str());
    #endif //TACT_DEBUG
    peripherals_->buzzer.PlayFail();
    return;
  }

  SetState(DataState::receive, "");

  // request currently selected slot to be sent by client
  Serial.write(COM_RECEIVE);
  Serial.write(slot);
  uint32_t size = 0;
  Serial.write((byte *)&size, 4);
  Serial.write(COM_MESSAGE_FOLLOW_UP);

  peripherals_->buzzer.PlayConfirm();
  vector_in.clear();
  time_last_receive = 0;
  //receive_slot = slot;
}


void DataTransfer::Receive(State &current_state) {
  if (connection_confirmed == false) {
    ReceiveIdleMode();
    return;
  }

  if (current_state.mode != tact::Modes::transfer)
    return;

  #ifdef TACT_DEBUG
  debug::println(__FILE__, __func__, "", debug::DebugLevel::verbose);
  #endif //TACT_DEBUG

  if (state != DataState::receive) {
    //drain pending data
    ReceiveIdleMode();
    return;
  }

  //consider transfer finsihed after short void time 
  if ((time_last_receive > 0) && (millis() - time_last_receive > 50)) {
    std::string string_return = ProcessReceivedData();
    SetState(DataState::idle, string_return.substr(0, 8), true);

    if (string_return.rfind("ERROR", 0) == 0) {
      peripherals_->buzzer.PlayFail();
      #ifdef TACT_DEBUG
      debug::debug_stream << " ERROR data: " << string_return.c_str();
      debug::println(__func__, debug::debug_stream.str());
      #endif //TACT_DEBUG
    }
    else {
      peripherals_->buzzer.PlayConfirm();
      #ifdef TACT_DEBUG
      debug::debug_stream << " done: " << string_return.c_str();
      debug::println(__func__, debug::debug_stream.str());
      #endif //TACT_DEBUG
    }

    vector_in.clear();
    return;
  }
      
  while(Serial.available() > 0) {
    time_last_receive = millis();
    vector_in.push_back(Serial.read());
  } 
}


void DataTransfer::ReceiveIdleMode(void) {
  #ifdef TACT_DEBUG
  debug::println(__FILE__, __func__, "", debug::DebugLevel::verbose);
  #endif //TACT_DEBUG
  
  while(Serial.available() > 0) {
    if (string_received.length() > 30) {
      string_received.erase(0, 1);
    }
    string_received.push_back(Serial.read());
  }

  std::string granted = "000000granted";
  granted[0] = COM_CONNECT;
  granted[1] = 0; // slot nb
  granted[2] = 7; // size
  granted[3] = 0; // size
  granted[4] = 0; // size
  granted[5] = 0; // size
  if (string_received.find(granted) != -1) {
    peripherals_->buzzer.PlayConfirm();
    connection_confirmed = true;
    string_received.clear();
  }
}


std::string DataTransfer::ProcessReceivedData(void) {
  #ifdef TACT_DEBUG
  debug::println(__FILE__, __func__, "", debug::DebugLevel::verbose);
  debug::debug_stream << "vector_in.size()=" << vector_in.size();
  debug::println(__func__, debug::debug_stream.str(), debug::DebugLevel::verbose);
  #endif //TACT_DEBUG

  if (vector_in.size() == 0)
    return "ERROR 0 : vector_in.size() == 0";

  if (vector_in.size() < 6)
    return "ERROR 1 : vector_in.size() < 6";

  if (vector_in.at(0) != COM_RECEIVE)
    return "ERROR 2 : vector_in.at(0) != COM_RECEIVE";

  uint8_t slot = vector_in.at(1);

  if ((slot == 0) || (slot >= TACTONS_COUNT_MAX))
    return "ERROR 3 : (slot == 0) || (slot >= TACTONS_COUNT_MAX)";

  uint32_t size = *((uint32_t*)&vector_in[2]);

  if (size % 4 != 0)
    return "ERROR 4 : size % 4 != 0";

  #ifdef TACT_DEBUG
  debug::debug_stream << "size=" << size;
  debug::println(__func__, debug::debug_stream.str(), debug::DebugLevel::verbose);
  #endif //TACT_DEBUG

  //if (vector_in.size() != 6 + size)
  if (vector_in.size() < 6 + size)
    return "ERROR 5 : vector_in.size() < 6 + size";

  unsigned char buffer[4];
  int index_vtp = 0;
  for (int i = 6; i < 6 + size/*vector_in.size()*/; i+=4) {
    buffer[0] = vector_in.at(i + 0);
    buffer[1] = vector_in.at(i + 1);
    buffer[2] = vector_in.at(i + 2);
    buffer[3] = vector_in.at(i + 3);

    VTPInstructionWord out;
    vtp_read_instruction_words(1, buffer, &out);
    int result = sampler_->FromVTP(slot, &out, index_vtp);
    index_vtp++;
    if (result != 0) {
      std::ostringstream ss_out;
      ss_out << "ERROR "  << result;
      return ss_out.str();
    }
  }

  return "OK";
}


void DataTransfer::SendButtonPressed(uint8_t slot) {
  #ifdef TACT_DEBUG
  debug::println(__FILE__, __func__, "");
  #endif //TACT_DEBUG
  
  //Linux:
  // device: e.g. /dev/ttyUSB0
  // set serial device to default settings (needed after firmware upload, so no reconnect needed): stty -F <device> raw 115200
  // show incoming data: cat device
  // write incoming data to file as is: (stty raw; cat > tacton.out) < device
  // send file to device: cat file > device

  if (connection_confirmed == false) {
      peripherals_->buzzer.PlayFail();
      SetState(DataState::idle, "NO CLIENT", true);
      return;
  }

  SetState(DataState::send, "");
  peripherals_->buzzer.PlayConfirm();

  std::vector<uint8_t> vector_data_out;
  sampler_->ToVTP(slot, vector_data_out);
  uint32_t data_size = vector_data_out.size();

  // data should have at least one instruction (4 bytes)
  if (data_size < 4) {
    peripherals_->buzzer.PlayFail();
    SetState(DataState::idle, "NO DATA");
    return;
  }

  // data should have multiple 4 bytes
  if ((data_size%4) != 0) {
    peripherals_->buzzer.PlayFail();
    SetState(DataState::idle, "DATA NOK");
    return;
  }

  #ifdef TACT_DEBUG
  debug::debug_stream << "sending slot=" << unsigned(slot) << "  vector_data_out.size()=" << vector_data_out.size() <<  "  size/4=" << (vector_data_out.size() / 4);
  debug::println(__func__, debug::debug_stream.str());
  #endif //TACT_DEBUG

  std::ostringstream ss_prefix;

  Serial.write(COM_SEND);
  Serial.write(slot);
  uint32_t size = vector_data_out.size();
  Serial.write((byte *)&size, 4);
  for (int i = 0; i < vector_data_out.size(); i++) {
    Serial.write(vector_data_out.at(i));
  }
  Serial.write(COM_MESSAGE_FOLLOW_UP);

  peripherals_->buzzer.PlayConfirm();
  SetState(DataState::idle, "");
}


void DataTransfer::Send(void) {
  // send periodical life sign
  if (connection_confirmed == false && (millis() - time_last_connection_sent >= 1000)) {
    time_last_connection_sent = millis();
    Serial.write(COM_CONNECT);
    Serial.write(0);
    std::string out("asking");
    uint32_t size = out.size();
    Serial.write((byte *)&size, 4);
    Serial.printf("%s", out.c_str());
    Serial.write(COM_MESSAGE_FOLLOW_UP);
    #ifdef TACT_DEBUG
    debug::println(__FILE__, __func__, "", debug::DebugLevel::verbose);
    #endif //TACT_DEBUG
  }
}


} //namespace tact 
