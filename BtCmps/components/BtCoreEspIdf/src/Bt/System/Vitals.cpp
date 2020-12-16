//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::System::Vitals
//
//*************************************************************************************************

#include "Bt/System/Vitals.h"

//#include <esp_heap_caps.h>
#include <esp_system.h>
#include <esp_timer.h>

#include <ArduinoJson.h>

#include <Bt/Core/StringUtilities.h>

namespace Bt {
namespace System {

Vitals::Vitals(Concurrency::I_SchedulingExecutionContext& pExecutionContext, Protocols::I_MqttController& pMqtt)
: mExecutionContext(pExecutionContext)
, mMqtt(pMqtt)
, mOnMqttConnected(mExecutionContext,[this](auto pEvent){onMqttConnected();}) {
    uint8_t mac[6]; 
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    mChipId = Core::stringPrintf("%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    mTopic = Core::stringPrintf("bittailor/home/gateway/%s/vitals",mChipId.c_str());
}

Vitals::~Vitals() {
}

std::string Vitals::vitalsJson() {
    DynamicJsonDocument doc(1024);
    auto heap = doc.createNestedObject("heap");          
    heap["free"].set(esp_get_free_heap_size());
    heap["min"].set(esp_get_minimum_free_heap_size());
    heap["block"].set(heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT));               
    doc["uptime"].set(esp_timer_get_time());
    return doc.as<std::string>();
} 

 void Vitals::onMqttConnected() {
     publishVitals();
     mExecutionContext.callPeriodically(std::chrono::seconds(20),[this](auto&& pTimer){
         publishVitals();
     });
 }

 void Vitals::publishVitals(){
     mMqtt.publish(mTopic.c_str(), vitalsJson());
 }

} // namespace System
} // namespace Bt

