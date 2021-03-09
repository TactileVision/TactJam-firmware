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
  SetState(DataState::idle, "", true);
  #ifdef TACT_DEBUG
  Serial.print("DataTransfer::Reset\n");
  #endif //TACT_DEBUG
}


void DataTransfer::ReceiveButtonPressed(uint8_t slot) {

  if (connection_confirmed == false) {
      peripherals_->buzzer.PlayFail();
      SetState(DataState::idle, "NO CLIENT", true);
      return;
  }

  if ((slot < 1) || (slot >= TACTONS_COUNT_MAX)) {
    Serial.printf("<Result what=\"ERROR: slot %d not allowed\"/>\n", slot);
    peripherals_->buzzer.PlayFail();
    return;
  }

  SetState(DataState::receive, "");
  peripherals_->buzzer.PlayConfirm();
  vector_in.clear();
  time_last_receive = 0;
  //receive_slot = slot;

  #ifdef TACT_DEBUG
  Serial.print("DataTransfer::ReceiveButtonPressed\n");
  #endif //TACT_DEBUG
}


void DataTransfer::Receive(State &current_state) {

  if (connection_confirmed == false) {
    ReceiveIdleMode();
    return;
  }

  if (current_state.mode != tact::Modes::transfer)
    return;

  if (state != DataState::receive) {
    //drain pending data
    ReceiveIdleMode();
    return;
  }

  //consider transfer finsihed after short void time 
  if ((time_last_receive > 0) && (millis() - time_last_receive > 50)) {
    std::string string_return = ProcessReceivedData();
    SetState(DataState::idle, string_return.substr(0, 8), true);
    
    #ifdef TACT_DEBUG
    Serial.printf("<Result what=\"%s\"/>\n", string_return.c_str());
    #endif //TACT_DEBUG


    if (string_return.rfind("ERROR", 0) == 0) {
      peripherals_->buzzer.PlayFail();
      //#ifdef TACT_DEBUG
      //Serial.printf("%s\n", string_return.c_str());
      //#endif //TACT_DEBUG
    }
    else {
      peripherals_->buzzer.PlayConfirm();
      //#ifdef TACT_DEBUG
      //Serial.printf("done: %s\n", string_return.c_str());
      //#endif //TACT_DEBUG
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
  while(Serial.available() > 0) {
    if (string_received.length() > 30) {
      string_received.erase(0, 1);
    }
    string_received.push_back(Serial.read());
  }


  if (string_received.find("granted") != -1) {
    peripherals_->buzzer.PlayConfirm();
    connection_confirmed = true;
    string_received.clear();
  }

  //if (string_received.find("<GetTactonList/>") != -1) {
  //  std::stringstream ss_out;
  //  ss_out << "<TactonList slots=\"" << sampler_->GetTactonListAsString().c_str() << "\"/>";
  //  Serial.printf("%s\n", ss_out.str().c_str());
  //  peripherals_->buzzer.PlayConfirm();
  //  string_received.clear();
  //}

  //<GetTacton slot=”int”/>
  //std::string string_slot = "<SendTacton slot=\"";
  //int index_slot = string_received.find(string_slot);
  //if ((index_slot != -1) && (string_received.find("/>", index_slot) != -1)) {
  //  int slot = string_received.at(index_slot + string_slot.size()) - 48;
  //  #ifdef TACT_DEBUG
  //  Serial.printf("DataTransfer::ReceiveIdleMode: SendTacton slot %d requested\n", slot);
  //  #endif //TACT_DEBUG
  //  SendButtonPressed(slot);
  //  string_received.clear();
  //}

  //<ReceiveTacton slot=”int”/>
  //string_slot = "<ReceiveTacton slot=\"";
  //index_slot = string_received.find(string_slot);
  //if ((index_slot != -1) && (string_received.find("/>", index_slot) != -1)) {
  //  int slot = string_received.at(index_slot + string_slot.size()) - 48;
  //  #ifdef TACT_DEBUG
  //  Serial.printf("DataTransfer::ReceiveTacton: ReceiveTacton slot %d requested\n", slot);
  //  #endif //TACT_DEBUG
  //  ReceiveButtonPressed(slot);
  //  string_received.clear();
  //}
}


//std::string DataTransfer::GetDataAsString(std::vector<uint8_t> &vector_data, int index, int length/*, uint8_t char_stop*/) {
//  if (vector_data.size() < (index + length)) {
//    return "";
//  }
//
//  std::string string_return;
//  for (int i = index; i < index + length; i++) {
//    string_return.push_back(vector_data.at(i));
    //if ( vector_data.at(i) == char_stop)
    //  break;
//  }
//
//  return string_return;
//}


std::string DataTransfer::ProcessReceivedData(void) {
  #ifdef TACT_DEBUG
  Serial.printf("DataTransfer::ProcessReceivedData: vector_in.size()=%d\n", vector_in.size());
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
  Serial.printf("DataTransfer::ProcessReceivedData: size=%d\n", size);
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



  //remove possilbe trailing new line
  //for (int i = 0; i <2; i++) {
  //  if ((vector_in.at(vector_in.size()-1) == '\n') || (vector_in.at(vector_in.size()-1) == '\r')) {
  //    vector_in.pop_back();
  //  }
  //}
  //check XML tags
  //std::string string_prefix("<tacton>");
  //if (GetDataAsString(vector_in, 0, string_prefix.size()) != string_prefix) {
  //  return "ERROR 1 : missing <tacton> prefix";
  //}
  //std::string string_suffix("</tacton>");
  //Serial.printf("'%s'\n", GetDataAsString(vector_in, vector_in.size() - string_suffix.size(), string_suffix.size()).c_str());
  //if (GetDataAsString(vector_in, vector_in.size() - string_suffix.size(), string_suffix.size()) != string_suffix) {
  //  return "ERROR 2 : missing </tacton> suffix";
  //}
  //int length_overhead_start;
  //std::string string_data;
  //string_data = GetDataAsString(0, string_prefix.size(), '>');
  //if ( string_data.empty() == true )
  //  return "ERROR 1";
  //Serial.printf("'%s'\n", string_data.c_str());
  //if (string_data.rfind(string_prefix, 0) != 0 )
  //  return "ERROR 2";
  //if (string_data.at(string_data.size() -1) != '>' )
  //  return "ERROR 3";
  //length_overhead_start=string_data.size();
  //std::string string_suffix("</tacton>");
  //string_data = GetDataAsString(vector_in.size() - string_suffix.size(), string_suffix.size(), '_');
  //Serial.printf("'%s'\n", string_data.c_str());
  //if (string_data.compare(string_suffix) != 0 )
  //  return "ERROR 4";

  //int length_data_calculated = vector_in.size() - length_overhead_start - string_suffix.size();

  //if (length_data_calculated % 4 != 0 )
  //      return "ERROR 5";

  //unsigned char buffer[4];
  //int index_vtp = 0;
  //for (int i = string_prefix.size(); i < vector_in.size() - string_suffix.size(); i+=4) {
  //  buffer[0] = vector_in.at(i + 0);
  //  buffer[1] = vector_in.at(i + 1);
  //  buffer[2] = vector_in.at(i + 2);
  //  buffer[3] = vector_in.at(i + 3);

  //  VTPInstructionWord out;
  //  vtp_read_instruction_words(1, buffer, &out);
  //  int result = sampler_->FromVTP(receive_slot, &out, index_vtp);
  //  index_vtp++;
  //  if (result != 0) {
  //    std::ostringstream ss_out;
  //    ss_out << "ERROR "  << result;
  //    return ss_out.str();
  //  }
  //}

  //std::ostringstream ss_out;
  //ss_out << (index_vtp * 4) << " B";
  return "OK";
}


void DataTransfer::SendButtonPressed(uint8_t slot) {
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
  Serial.printf("DataTransfer::SendButtonPressed: sending slot=%d  vector_data_out.size()=%d  size/4=%d\n", slot, vector_data_out.size(), vector_data_out.size() / 4);
  #endif //TACT_DEBUG

  std::ostringstream ss_prefix;
  //ss_prefix << "<tacton lengthBytes=\""  << vector_data_out.size() << "\">";
  //ss_prefix << "<tacton>";
  //Serial.write(ss_prefix.str().c_str());
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
  
  //if (state != DataState::idle)
  //  return;

  // send periodical life sign
  if (connection_confirmed == false &&
      (millis() - time_last_connection_sent >= 1000)) {
    time_last_connection_sent = millis();
    Serial.write(COM_CONNECT);
    Serial.write(0);
    std::string out("asking");
    uint32_t size = out.size();
    Serial.write((byte *)&size, 4);
    Serial.printf("%s", out.c_str());
    Serial.write(COM_MESSAGE_FOLLOW_UP);
  }
}



} //namespace tact 
