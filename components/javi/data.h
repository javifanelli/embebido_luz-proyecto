#define refresh 5 // tiempo en segundos para refrescar medición en display
#define min_mqtt 5 // Tiempo en minutos para enviar el mensaje MQTT

int cont_mqtt = (min_mqtt*60/refresh)-4;
bool time_sinc_ok = false;

void get_data (void *pvParameter);

void get_data(void *pvParameter)
{
    while(1) {
        set_times();
        if (!time_sinc_ok)
            obtain_time();
        time_t now = time(NULL);
        now-=3*3600;
        timeinfo = localtime(&now);
        strftime(pant_time, sizeof(pant_time), "%H:%M %d-%m-%Y", timeinfo);
        now = time(NULL);
        timeinfo = localtime(&now);
        strftime(formatted_time, sizeof(formatted_time), "%Y-%m-%d %H:%M:%S", &timeinfo);
        if(level==0)
            pant_main();
        esp_wifi_sta_get_ap_info(&ap_info);
        net_con = (ap_info.authmode != WIFI_AUTH_OPEN);
        if(cont_mqtt==60){
            if (!net_con)
            	esp_wifi_connect();
            if(mqtt_state)    
                cont_mqtt=0;
            mqtt_send_info();
            }
        cont_mqtt++;
        if(modo==1){
                if(time_func)
                    out_dim=set_point;
                if(!time_func)
                    out_dim=0;
            }
        vTaskDelay(pdMS_TO_TICKS(1000*refresh));
   	}
   vTaskDelete(NULL);
}