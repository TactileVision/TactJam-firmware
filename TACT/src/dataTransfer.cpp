#include <dataTransfer.h>
#include <sstream>

namespace tact {

DataTransfer::DataTransfer(tact::State* current_state, tact::Display* display, tact::Buzzer* buzzer, tact::TactonRecorderPlayer* tacton_recorder_player) :
  current_state(current_state), display(display), buzzer(buzzer), tacton_recorder_player(tacton_recorder_player) {
}


void DataTransfer::SetState(State state, std::string line_2, bool force_display_update) {
  if (state != this->state || force_display_update == true) {
    std::string line_1;
    switch(state) {
      case State::idle: 
        line_1.assign("idle");
        break;
      case State::send:
        line_1.assign("send");
        break;
      case State::receive:
        line_1.assign("receive");
        break;
    }
    //if ( loop_playback == true )
    //  line_2.assign("loop on");
    //else
    //  line_2.assign("loop off");
    display->DrawContentTeaserDoubleLine(line_1.c_str(), line_2.c_str());
  }
  this->state = state;
}


void DataTransfer::Reset() {
  SetState(State::idle, "", true);
  #ifdef TACT_DEBUG
  Serial.print("DataTransfer::Reset\n");
  #endif //TACT_DEBUG
}


void DataTransfer::ReceiveButtonPressed(int slot) {
  if ( slot < 1 || slot >= TACTONS_COUNT_MAX) {
    Serial.printf("<Result what=\"ERROR: slot %d not allowed\"/>\n", slot);
    buzzer->PlayFail();
    return;
  }
  SetState(State::receive, "");
  buzzer->PlayConfirm();
  vector_in.clear();
  time_last_receive = 0;
  receive_slot = slot;

  #ifdef TACT_DEBUG
  Serial.print("DataTransfer::ReceiveButtonPressed\n");
  #endif //TACT_DEBUG
}


void DataTransfer::Receive(void) {

  if (state == State::idle ) {
    ReceiveIdleMode();
    return;
  }

  if (state != State::receive )
    return;

  //consider transfer finsihed after short void time 
  if ( time_last_receive > 0 &&
       millis() - time_last_receive > 50 ) {
    std::string string_return = ProcessReceivedData();
    SetState(State::idle, string_return.substr(0, 8), true);

    Serial.printf("<Result what=\"%s\"/>\n", string_return.c_str());

    if (string_return.rfind("ERROR", 0) == 0) {
      buzzer->PlayFail();
      //#ifdef TACT_DEBUG
      //Serial.printf("%s\n", string_return.c_str());
      //#endif //TACT_DEBUG
    }
    else {
      buzzer->PlayConfirm();
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
    if ( string_received.length() > 30)
      string_received.erase(0, 1);
    string_received.push_back(Serial.read());
  }

  if (string_received.find("<GetTactonList/>") != -1 ) {
    std::stringstream ss_out;
    ss_out << "<TactonList slots=\"" << tacton_recorder_player->GetTactonListAsString().c_str() << "\"/>";
    Serial.printf("%s\n", ss_out.str().c_str());
    buzzer->PlayConfirm();
    string_received.clear();
  }

  //<GetTacton slot=”int”/>
  std::string string_slot = "<SendTacton slot=\"";
  int index_slot = string_received.find(string_slot);
  if ( index_slot != -1 &&
       string_received.find("/>", index_slot) != -1) {
    int slot = string_received.at(index_slot + string_slot.size()) - 48;
    #ifdef TACT_DEBUG
    Serial.printf("DataTransfer::ReceiveIdleMode: SendTacton slot %d requested\n", slot);
    #endif //TACT_DEBUG
    SendButtonPressed(slot);
    string_received.clear();
  }

  //<ReceiveTacton slot=”int”/>
  string_slot = "<ReceiveTacton slot=\"";
  index_slot = string_received.find(string_slot);
  if ( index_slot != -1 &&
       string_received.find("/>", index_slot) != -1) {
    int slot = string_received.at(index_slot + string_slot.size()) - 48;
    #ifdef TACT_DEBUG
    Serial.printf("DataTransfer::ReceiveTacton: ReceiveTacton slot %d requested\n", slot);
    #endif //TACT_DEBUG
    ReceiveButtonPressed(slot);
    string_received.clear();
  }
}


std::string DataTransfer::GetDataAsString(std::vector<uint8_t> &vector_data, int index, int length/*, uint8_t char_stop*/) {
  if ( vector_data.size() < index + length)
    return "";

  std::string string_return;
  for (int i = index; i < index + length; i++) {
    string_return.push_back(vector_data.at(i));
    //if ( vector_data.at(i) == char_stop)
    //  break;
  }

  return string_return;
}


std::string DataTransfer::ProcessReceivedData(void) {
  #ifdef TACT_DEBUG
  Serial.printf("DataTransfer::ProcessReceivedData: vector_in.size()=%d\n", vector_in.size());
  #endif //TACT_DEBUG

  if (vector_in.size() == 0 )
    return "ERROR 0 : no input data";
  //remove possilbe trailing new line
  for (int i = 0; i <2; i++)
    if (vector_in.at(vector_in.size()-1) == '\n' ||
      vector_in.at(vector_in.size()-1) == '\r')
      vector_in.pop_back();
  //check XML tags
  std::string string_prefix("<tacton>");
  if (GetDataAsString(vector_in, 0, string_prefix.size()) != string_prefix)
    return "ERROR 1 : missing <tacton> prefix";
  std::string string_suffix("</tacton>");
  //Serial.printf("'%s'\n", GetDataAsString(vector_in, vector_in.size() - string_suffix.size(), string_suffix.size()).c_str());
  if (GetDataAsString(vector_in, vector_in.size() - string_suffix.size(), string_suffix.size()) != string_suffix)
    return "ERROR 2 : missing </tacton> suffix";

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

  unsigned char buffer[4];
  int index_vtp = 0;
  for ( int i = string_prefix.size(); i < vector_in.size() - string_suffix.size(); i+=4) {
    buffer[0] = vector_in.at(i + 0);
    buffer[1] = vector_in.at(i + 1);
    buffer[2] = vector_in.at(i + 2);
    buffer[3] = vector_in.at(i + 3);

    VTPInstructionWord out;
    vtp_read_instruction_words(1, buffer, &out);
    int result = tacton_recorder_player->FromVTP(receive_slot, &out, index_vtp);
    index_vtp++;
    if (result != 0 ) {
      std::ostringstream ss_out;
      ss_out << "ERROR "  << result;
      return ss_out.str();
    }
  }

  //std::ostringstream ss_out;
  //ss_out << (index_vtp * 4) << " B";
  return "OK";
}


void DataTransfer::SendButtonPressed(int slot) {
  //Linux:
  // device: e.g. /dev/ttyUSB0
  // set serial device to default settings (needed after firmware upload, so no reconnect needed): stty -F <device> sane -echo 115200
  // show incoming data: cat device
  // write incoming data to file as is: (stty raw; cat > tacton.out) < device
  // send file to device: cat file > device

  //#ifdef TACT_DEBUG
  //Serial.print("DataTransfer::SendButtonPressed\n");
  //#endif //TACT_DEBUG

  if ( slot < 1 || slot >= TACTONS_COUNT_MAX) {
    Serial.printf("<Result what=\"ERROR: slot %d not allowed\"/>\n", slot);
    buzzer->PlayFail();
    return;
  }

  SetState(State::send, "");
  buzzer->PlayConfirm();

  std::vector<unsigned char> vector_data_out;
  tacton_recorder_player->ToVTP(slot, vector_data_out);

  #ifdef TACT_DEBUG
  Serial.printf("DataTransfer::SendButtonPressed: sending slot=%d  vector_data_out.size()=%d  size/4=%d\n", slot, vector_data_out.size(), vector_data_out.size() / 4);
  #endif //TACT_DEBUG

  std::ostringstream ss_prefix;
  //ss_prefix << "<tacton lengthBytes=\""  << vector_data_out.size() << "\">";
  ss_prefix << "<tacton>";
  Serial.write(ss_prefix.str().c_str());
  for (int i = 0; i < vector_data_out.size(); i++)
    Serial.write(vector_data_out.at(i));
  Serial.write("</tacton>");
  Serial.write("\n");
  //buzzer->PlayConfirm();
  SetState(State::idle, "");
}



} //namespace tact 