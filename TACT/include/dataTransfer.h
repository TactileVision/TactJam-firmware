#ifndef _TACT_DATATRANSFER_
#define _TACT_DATATRANSFER_

#include <display.h>
#include <buzzer.h>
#include <tactons.h>
#include <state.h>


namespace tact {

class DataTransfer {
  public:
    enum class State {
      idle,
      send,
      receive
    };

    DataTransfer(tact::State* current_state, tact::Display* display, tact::Buzzer* buzzer, tact::TactonRecorderPlayer* tacton_recorder_player);

    void SetState(State state, std::string line_2, bool force_display_update = false);
    void Reset(void);
    void ReceiveButtonPressed(void);
    void Receive(void);
    std::string GetDataAsString(int index, int length, uint8_t char_stop);
    std::string ProcessReceivedData(void);
    void SendButtonPressed(void);

  private:
    State state = State::idle; // use method SetState to change the state
    std::vector<uint8_t> vector_in;
    unsigned long time_last_receive;

    tact::State* current_state;
    tact::Display* display;
    tact::Buzzer* buzzer;
    tact::TactonRecorderPlayer* tacton_recorder_player;


};
   

}  //namespace tact

#endif //_TACT_DATATRANSFER_