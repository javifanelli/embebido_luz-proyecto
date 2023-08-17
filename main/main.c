#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "cJSON.h"
#include "../components/javi/variables.h"
#include "../components/esp-idf-lib-master/components/encoder/encoder.h"
#include "../components/javi/init.h"
#include "../components/javi/sntp_time.h"
#include "../components/javi/pantallas.c"
#include "../components/javi/led.c"
#include "../components/javi/wifi_con.c"
#include "../components/javi/mqtt_funcs.c"

#define QUEUE_LENGTH 10 // Define la longitud máxima de la cola
static QueueHandle_t _queue;

void app_main(void)
{
    
	ESP_ERROR_CHECK(nvs_flash_init());
	ESP_ERROR_CHECK(esp_netif_init());
	
	_queue = xQueueCreate(QUEUE_LENGTH, sizeof(rotary_encoder_event_t));
    
	config_dis ();
	pant_bienv ();
	config_led();
	pant_inicio ();
	wifi_init_sta();
    if(net_con)
		mqtt_app_start();
	pwm_init();

	ESP_ERROR_CHECK(rotary_encoder_init(_queue));
	ESP_ERROR_CHECK(rotary_encoder_add(&control));
	
	btn_enc=false;
	ssd1306_clear_screen(&devd, false);
	xTaskCreate(read_enc, "read_enc", 4096*2, NULL, 4, NULL);
}
