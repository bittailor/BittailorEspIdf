//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::System::Vitals
//
//*************************************************************************************************

#include "Bt/System/Vitals.h"

#include <esp_heap_caps.h>
#include <esp_system.h>
#include <esp_timer.h>

#include <ArduinoJson.h>

#include <Bt/Core/StringUtilities.h>

#include "Bt/System/I_System.h"

namespace Bt {
namespace System {

Vitals::Vitals(Concurrency::I_SchedulingExecutionContext& pExecutionContext, Protocols::I_MqttController& pMqtt)
: mExecutionContext(pExecutionContext)
, mMqtt(pMqtt)
, mOnMqttConnected(mExecutionContext,[this](auto pEvent){onMqttConnected();}) {
    mTopic = Core::stringPrintf("bittailor/home/gateway/%s/vitals", System::getId().c_str());
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

