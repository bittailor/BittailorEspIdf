//*************************************************************************************************
//
//  BITTAILOR.CH  -  Bt::Network::I_WiFiController
//
//*************************************************************************************************

#ifndef INC__Bt_Network_I_WiFiController__h
#define INC__Bt_Network_I_WiFiController__h

namespace Bt {
namespace Network {

class I_WiFiController {
   public:
      enum class State {INITIAL, CONNECTED, CONNECT_FAILED, CONNECTION_LOST, DISCONNECTED};
      struct ConnectionState{State state;};

      virtual ~I_WiFiController() {}
};

} // namespace Network
} // namespace Bt

#endif // INC__Bt_Network_I_WiFiController__h

