#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "nvs_flash.h"
#include "esp_err.h"


struct BLE_data_packet{
    int rssiVal;// received signal strength indicator
    char macStr[18];// mac address
    // can store as char since rate of packets is low
};
static QueueHandle_t ble_queue = NULL;

// printer task
void printer_task(void *parameter)
{
    BLE_data_packet packet; 
    //packet variable
    while(1) //run infinitely
    {
        // Blocks indefinitely (0% CPU) until Tier 1 pushes a packet into the queue
        if(xQueueReceive(ble_queue, &packet, portMAX_DELAY) == pdPASS) {
            // portMAX_DELAY : wait indefinitely
            // pdPASS : true
            uint32_t timestamp = (unsigned long)pdTICKS_TO_MS(xTaskGetTickCount());
            // Standard comma-separated output for clean parsing
            printf("%lu,%s,%d\n", timestamp, packet.macStr, packet.rssiVal);
        }
    }
}

static int ble_scan_event_cb(struct ble_gap_event *event, void *arg) {
    // Shape-shift check: Only look inside the envelope if it is a Discovery Event
    if (event->type == BLE_GAP_EVENT_DISC) {
        BLE_data_packet packet;
        packet.rssiVal = event->disc.rssi;
        
        // Unpack raw binary 6-byte address layout into readable string format
        snprintf(packet.macStr, sizeof(packet.macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                 event->disc.addr.val[5], event->disc.addr.val[4], event->disc.addr.val[3],
                 event->disc.addr.val[2], event->disc.addr.val[1], event->disc.addr.val[0]);

        // Microsecond Offload: Push packet envelope to background queue
        xQueueSend(ble_queue, &packet, 0); 
    }
    return 0;
}

extern "C" void app_main(void)
{ // initialize non volatile flash memory(very important)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

 ble_queue = xQueueCreate(100,sizeof(BLE_data_packet));
 xTaskCreatePinnedToCore(printer_task,"Packet Printer Task",4096,NULL,1,NULL,1);
 // pin the printer task to core 1
 // since core 1 is free for us, we can set it to priority 1
 nimble_port_init();
 // start the 2.4GHz radio
 // bluetooth host configure synchronization callback
 // with lambda operator
 ble_hs_cfg.sync_cb = []()
    { // set the discovery parameters using a struct
        struct ble_gap_disc_params disc_params = {};
        disc_params.passive = 0;           // 0 means ACTIVE scanning 
        disc_params.filter_duplicates = 1; // 1 means filter out rapid duplicate packets to save bandwidth
        ble_gap_disc(BLE_OWN_ADDR_PUBLIC, BLE_HS_FOREVER, &disc_params, ble_scan_event_cb, NULL);
        // address to use: BLE_OWN_ADDR_PUBLIC or BLE_OWN_ADDR_RANDOM
        //BLE_HS_FOREVER means time limit = inf
        //ble_scan_event_cb: callback pointer
    };

    nimble_port_freertos_init((void (*)(void*))nimble_port_run);
    //start running
}
