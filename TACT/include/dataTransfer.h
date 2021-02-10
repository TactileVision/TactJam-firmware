#ifndef _TACT_DATATRANSFER_
#define _TACT_DATATRANSFER_

#include "peripherals.h"
#include "sampler.h"


namespace tact {

class DataTransfer {
  public:
    enum class DataState {
      idle,
      send,
      receive
    };

    DataTransfer(Peripherals* peripherals, Sampler* tacton_recorder_player);

    void SetState(DataState state, std::string line_2, bool force_display_update = false);
    void Reset(void);
    void ReceiveButtonPressed(uint8_t slot);
    void Receive(void);
    void ReceiveIdleMode(void);
    std::string GetDataAsString(std::vector<uint8_t> &vector_data, int index, int length/*, uint8_t char_stop*/);
    std::string ProcessReceivedData(void);
    void SendButtonPressed(uint8_t slot);

  private:
    DataState state = DataState::idle; // use method SetState to change the state
    std::vector<uint8_t> vector_in;
    unsigned long time_last_receive;
    std::string string_received;
    uint8_t receive_slot = 0;

    Sampler* sampler_;
    Peripherals* peripherals_;

};
   

}  //namespace tact

#endif //_TACT_DATATRANSFER_