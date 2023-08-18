const int min_mqtt=5; // tiempo en minutos para enviar el mensaje MQTT
int cont_mqtt = min_mqtt * 60 / refresh;
bool time_sinc_ok = false;

void get_data (void *pvParameter);

void get_data(void *pvParameter)
{
    while(1) {
        if (!time_sinc_ok)
            obtain_time();
        time_t now = time(NULL);
        timeinfo = localtime(&now);
        strftime(pant_time, sizeof(pant_time), "%H:%M %d-%m-%Y", timeinfo);

        if(level==0)
            pant_main();
        esp_wifi_sta_get_ap_info(&ap_info);
        net_con = (ap_info.authmode != WIFI_AUTH_OPEN);
        if(cont_mqtt==60){
            if (!net_con)
            	esp_wifi_connect();
            cont_mqtt=0;
            mqtt_send_info();
            }
        cont_mqtt++;
        vTaskDelay(pdMS_TO_TICKS(1000*refresh));
   	}
   vTaskDelete(NULL);
}