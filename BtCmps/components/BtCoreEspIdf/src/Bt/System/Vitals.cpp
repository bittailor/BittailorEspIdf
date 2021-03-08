//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::System::Vitals
//
//*************************************************************************************************

#include "Bt/System/Vitals.h"

#include <string>

#include <esp_heap_caps.h>
#include <esp_system.h>
#include <esp_timer.h>
#include <esp_ota_ops.h>


#include <Bt/Core/StringUtilities.h>
#include <Bt/Core/Logging.h>

#include "Bt/System/I_System.h"

namespace Bt {
namespace System {
namespace {
    constexpr const char* TAG = "Bt::System::Vitals"; 
}

Vitals::Vitals(Concurrency::I_SchedulingExecutionContext& pExecutionContext, Protocols::I_MqttController& pMqtt)
: mExecutionContext(pExecutionContext)
, mMqtt(pMqtt)
, mOnMqttConnected(mExecutionContext,[this](auto pEvent){onMqttConnected();})
, mOnMqttDisconnected(mExecutionContext,[this](auto pEvent){onMqttDisconnected();}) {
    mInfoTopic = Core::stringPrintf("bittailor/home/device/%s/info", System::getId().c_str());
    mVitalsTopic = Core::stringPrintf("bittailor/home/device/%s/vitals", System::getId().c_str());
}

Vitals::~Vitals() {
}

std::string Vitals::vitalsJson() {
    std::string msg = Bt::Core::stringPrintf( 
        R"JSON({"heap":{"free":%u,"min":%u,"block":%u},"uptime":%llu})JSON",
        esp_get_free_heap_size(),
        esp_get_minimum_free_heap_size(),
        heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT),
        esp_timer_get_time()
    );
    return msg;
} 

 void Vitals::onMqttConnected() {
    publishInfo();
    publishVitals();
    ESP_LOGI(TAG, "Start vitals timer on MQTT disconnected");
    mVitalsTimer = mExecutionContext.callPeriodically(std::chrono::seconds(20),[this](auto&& pTimer){
        publishVitals();
    });
 }

 void Vitals::onMqttDisconnected() {
     if(mVitalsTimer) {
         ESP_LOGI(TAG, "Cancel vitals timer on MQTT disconnected");
         mVitalsTimer->cancel();
         mVitalsTimer = nullptr;
     }
 }

void Vitals::publishVitals(){
    mMqtt.publish(mVitalsTopic.c_str(), vitalsJson());
 }

 void Vitals::publishInfo(){
    auto appDescription = esp_ota_get_app_description();
    std::string msg = Bt::Core::stringPrintf(
        R"JSON({"id":"%s","project":"%s","version":"%s","date":"%s","time":"%s","esp-idf":"%s"})JSON",
        System::getId().c_str(),
        appDescription->project_name,
        appDescription->version,
        appDescription->date, 
        appDescription->time,
        appDescription->idf_ver
    );    
    mMqtt.publish(mInfoTopic.c_str(), msg, 2, true);
 }


} // namespace System
} // namespace Bt

