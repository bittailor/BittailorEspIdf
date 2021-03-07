//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Ui::LedStrip
//
//*************************************************************************************************

#include "Bt/Ui/LedStrip.h"

#include <inttypes.h>

#include <Bt/Core/Logging.h>

namespace Bt {
namespace Ui {
namespace {
    constexpr const char* TAG = "Bt::Ui::LedStrip"; 

    constexpr uint32_t SK6812_T0H_NS = 300;
    constexpr uint32_t SK6812_T0L_NS = 900;
    constexpr uint32_t SK6812_T1H_NS = 600;
    constexpr uint32_t SK6812_T1L_NS = 600;

    
    constexpr uint8_t GAMMA_TABLE[256] = {
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,
        3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  7,
        7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 11, 12, 12,
        13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20,
        20, 21, 21, 22, 22, 23, 24, 24, 25, 25, 26, 27, 27, 28, 29, 29,
        30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42,
        42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
        58, 59, 60, 61, 62, 63, 64, 65, 66, 68, 69, 70, 71, 72, 73, 75,
        76, 77, 78, 80, 81, 82, 84, 85, 86, 88, 89, 90, 92, 93, 94, 96,
        97, 99,100,102,103,105,106,108,109,111,112,114,115,117,119,120,
        122,124,125,127,129,130,132,134,136,137,139,141,143,145,146,148,
        150,152,154,156,158,160,162,164,166,168,170,172,174,176,178,180,
        182,184,186,188,191,193,195,197,199,202,204,206,209,211,213,215,
        218,220,223,225,227,230,232,235,237,240,242,245,247,250,252,255}
    ;
    
    constexpr Color gammaCorrection(Color pColor) {
        return Color(
            GAMMA_TABLE[pColor.red()],
            GAMMA_TABLE[pColor.green()],
            GAMMA_TABLE[pColor.blue()]
        );
    }    

    rmt_item32_t sk6812_bit0;
    rmt_item32_t sk6812_bit1;


    void IRAM_ATTR bt_rmt_adapter(const void* src, rmt_item32_t* dest, size_t src_size, size_t wanted_num, size_t* translated_size, size_t* item_num, const rmt_item32_t& bit0, const rmt_item32_t& bit1)
    {
        if (src == nullptr || dest == nullptr) {
            *translated_size = 0;
            *item_num = 0;
            return;
        }

        size_t size = 0;
        size_t num = 0;
        uint8_t *psrc = (uint8_t *)src;
        rmt_item32_t *pdest = dest;
        while (size < src_size && num < wanted_num) {
            for (int i = 0; i < 8; i++) {
                // MSB first
                if (*psrc & (1 << (7 - i))) {
                    pdest->val =  bit1.val;
                } else {
                    pdest->val =  bit0.val;
                }
                num++;
                pdest++;
            }
            size++;
            psrc++;
        }
        *translated_size = size;
        *item_num = num;
    }

    void IRAM_ATTR sk6812_rmt_adapter(const void *src, rmt_item32_t *dest, size_t src_size,
            size_t wanted_num, size_t *translated_size, size_t *item_num)
    {
        bt_rmt_adapter(src, dest, src_size, wanted_num, translated_size, item_num, sk6812_bit0, sk6812_bit1);
    }
}

LedStrip::LedStrip(gpio_num_t pPin, rmt_channel_t pRtmChannel, size_t pNumberOfLeds)
: mNumberOfLeds(pNumberOfLeds), mBuffer(pNumberOfLeds * 3,0) {
    {
        rmt_config_t config = RMT_DEFAULT_CONFIG_TX(pPin, pRtmChannel);
        mConfig = config;
    }
    mConfig.clk_div = 2;
    ESP_ERROR_CHECK(rmt_config(&mConfig));
    ESP_ERROR_CHECK(rmt_driver_install(mConfig.channel, 0, 0));


    uint32_t counter_clk_hz = 0;
    ESP_ERROR_CHECK(rmt_get_counter_clock(mConfig.channel, &counter_clk_hz));
    float ratio = (float)counter_clk_hz / 1e9;
    
    sk6812_bit0.duration0 = ratio * SK6812_T0H_NS;
    sk6812_bit0.level0 = 1;
    sk6812_bit0.duration1 = ratio * SK6812_T0L_NS;
    sk6812_bit0.level1 = 0;
    sk6812_bit1.duration0 = ratio * SK6812_T1H_NS;
    sk6812_bit1.level0 = 1;
    sk6812_bit1.duration1 = ratio * SK6812_T1L_NS;
    sk6812_bit1.level1 = 0;

    rmt_translator_init(mConfig.channel, sk6812_rmt_adapter);

}

LedStrip::~LedStrip() {
    ESP_ERROR_CHECK(rmt_driver_uninstall(mConfig.channel));
}

void LedStrip::setPixel(size_t pIndex, const Color& pColor)
{
    if(pIndex >= mNumberOfLeds) {
        ESP_LOGE(TAG, "Index=%d out of bound of NumberOfLeds=[%d]", pIndex, pIndex);    
        return;
    }
    uint32_t start = pIndex * 3;
    auto color = gammaCorrection(pColor);
    //auto color = pColor;
    mBuffer[start + 0] = color.green();
    mBuffer[start + 1] = color.red();
    mBuffer[start + 2] = color.blue();
}

void LedStrip::clear(){
    std::fill(std::begin(mBuffer), std::end(mBuffer), 0);
}

void LedStrip::refresh() {
    //auto start = esp_timer_get_time();
    rmt_write_sample(mConfig.channel, mBuffer.data(), mBuffer.size(), false);
    //auto end = esp_timer_get_time();
    //ESP_LOGI(TAG, "refresh took %" PRIu64 "us", end-start);
}

} // namespace Ui
} // namespace Bt

