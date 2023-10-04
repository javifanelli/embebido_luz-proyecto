// Certificados usados para la conexión segura con el broker
extern const uint8_t client_cert_pem_start[] asm("_binary_client_pem_start");
extern const uint8_t client_cert_pem_end[] asm("_binary_client_pem_end");
extern const uint8_t client_key_pem_start[] asm("_binary_client_key_start");
extern const uint8_t client_key_pem_end[] asm("_binary_client_key_end");
extern const uint8_t server_cert_pem_start[] asm("_binary_ca_pem_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_ca_pem_end");

#include "sntp_time.h"

#define BROKER_URI "mqtts://192.168.0.70" // IP del broker (Raspberry Pi)
#define TAG "sensor"

static char *buffer_mqtt;
static char *buffer_topic;
static char TOPIC_OUT[100]="/home/dimmer/data"; // Topic de MQTT de datos de salida
static char TOPIC_IN[100]="/home/dimmer/settings"; // Topic de MQTT de datos de entrada
static char TOPIC_CONFIG[100]="/home/config";
static char TOPIC_SETUP[100]="/home/setup";
static esp_mqtt_client_handle_t client;

void mqtt_send_info (void);
void mqtt_rcv_info (void);

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
    mqtt_state = true;
    ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
    msg_id = esp_mqtt_client_subscribe(client, TOPIC_IN, 0);
    msg_id = esp_mqtt_client_subscribe(client, TOPIC_SETUP, 0);
    ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
    mqtt_state = true;
    if (!init) {
        cJSON *root = cJSON_CreateObject();
        cJSON_AddStringToObject(root, "ID", ID);
        char *json_string = cJSON_PrintUnformatted(root);
        blink_1();
        esp_mqtt_client_publish(client, TOPIC_CONFIG, json_string, 0, 0, 0);
        free(json_string);
        cJSON_Delete(root);
        init = true;
    }
    break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        mqtt_state = false;
        break;
    
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=");
        for (int i = 0; i < event->topic_len; i++) {
            putchar(event->topic[i]);
        }
        putchar('\n');
        printf("DATA=");
        for (int i = 0; i < event->data_len; i++) {
            putchar(event->data[i]);
        }
        putchar('\n');
        buffer_mqtt = malloc(event->data_len + 1);
        buffer_topic = malloc(event->topic_len + 1);
        if (buffer_mqtt == NULL || buffer_topic == NULL) {
            ESP_LOGE(TAG, "Memory allocation failed");
        break;
        }
        memcpy(buffer_mqtt, event->data, event->data_len);
        buffer_mqtt[event->data_len] = '\0';
        memcpy(buffer_topic, event->topic, event->topic_len);
        buffer_topic[event->topic_len] = '\0';
        if (strcmp(buffer_topic, TOPIC_IN) == 0) {
            ESP_LOGI(TAG, "Received data via MQTT");
            mqtt_rcv_info();
        }
        else if (strcmp(buffer_topic, TOPIC_SETUP) == 0) {
            ESP_LOGI(TAG, "Received initial setup via MQTT");
            mqtt_rcv_info();
        }
        else {
            ESP_LOGW(TAG, "Received data from an unknown MQTT topic");
        }
        free(buffer_mqtt);
        free(buffer_topic);
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

static void mqtt_app_start(void)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = BROKER_URI,
        .client_cert_pem = (const char *)client_cert_pem_start,
        .client_key_pem = (const char *)client_key_pem_start,
        .cert_pem = (const char *)server_cert_pem_start,
    };
    
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);

}

void mqtt_send_info(void)
{   
    blink_2();
    char hon_c[4], mon_c[4], hoff_c[4], moff_c[4];
    memset(out_char, 0, sizeof(out_char));
    memset(sp_char, 0, sizeof(sp_char));
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "ID", ID);
    cJSON_AddStringToObject(root, "MAC", mac_str);
    cJSON_AddStringToObject(root, "tipo", tipo_disp);
    strftime(formatted_time, sizeof(formatted_time), "%Y-%m-%d %H:%M:%S", timeinfo);
    cJSON_AddStringToObject(root, "time", formatted_time);
    sprintf(out_char, "%d", out_dim/102*10);
    cJSON_AddStringToObject(root, "valor", out_char);
    sprintf(sp_char, "%d", set_point);
    cJSON_AddStringToObject(root, "set_point", sp_char);
    if(modo==0)
        cJSON_AddStringToObject(root, "modo", "Manual");
    if(modo==1)
        cJSON_AddStringToObject(root, "modo", "Automatico");
    cJSON_AddStringToObject(root, "salida", out_char);
    sprintf(hon_c, "%d", hon);
    sprintf(mon_c, "%d", mon);
    sprintf(hoff_c, "%d", hoff);
    sprintf(moff_c, "%d", moff);
    cJSON_AddStringToObject(root, "hon", hon_c);
    cJSON_AddStringToObject(root, "mon", mon_c);
    cJSON_AddStringToObject(root, "hoff", hoff_c);
    cJSON_AddStringToObject(root, "moff", moff_c);
    char *json_string = cJSON_PrintUnformatted(root);
    esp_mqtt_client_publish(client, TOPIC_OUT, json_string, 0, 0, 0);
    free(json_string);
    cJSON_Delete(root);
}

void mqtt_rcv_info(void)
{
    bool select=false;
    cJSON *root = cJSON_Parse(buffer_mqtt);
    if (root == NULL) {
        ESP_LOGE(TAG, "Failed to parse MQTT data");
        return;
    }
    const cJSON *id = cJSON_GetObjectItemCaseSensitive(root, "id");
    if (cJSON_IsString(id)) {
        if (strcmp(id->valuestring, ID) == 0) {
            ESP_LOGI(TAG, "Received correct MQTT ID");
            select=true;
            blink_3();
        }
    }
    const cJSON *salida = cJSON_GetObjectItemCaseSensitive(root, "salida");
    if (cJSON_IsNumber(salida) && select) {
        out_dim=(salida->valueint)*102/10;
        ESP_LOGI(TAG, "Received MQTT salida: %d", out_dim);
        sprintf(out_char, "%d", out_dim/102*10);
        pant_main();
    }
    const cJSON *setpoint = cJSON_GetObjectItemCaseSensitive(root, "setpoint");
    if (cJSON_IsNumber(setpoint) && select) {
        set_point=setpoint->valueint;
        ESP_LOGI(TAG, "Received MQTT setpoint: %d", set_point);
    }
    const cJSON *horaon = cJSON_GetObjectItemCaseSensitive(root, "hon");
    if (cJSON_IsNumber(horaon) && select) {
        hon=horaon->valueint;
        ESP_LOGI(TAG, "Received MQTT on hour: %d", hon);
    }
    const cJSON *minutoon = cJSON_GetObjectItemCaseSensitive(root, "mon");
    if (cJSON_IsNumber(minutoon) && select) {
        mon=minutoon->valueint;
        ESP_LOGI(TAG, "Received MQTT on minutes: %d", mon);
    }
    const cJSON *horaoff = cJSON_GetObjectItemCaseSensitive(root, "hoff");
    if (cJSON_IsNumber(horaoff) && select) {
        hoff=horaoff->valueint;
        ESP_LOGI(TAG, "Received MQTT off hour: %d", hoff);
    }
    const cJSON *minutooff = cJSON_GetObjectItemCaseSensitive(root, "moff");
    if (cJSON_IsNumber(minutooff) && select) {
        moff=minutooff->valueint;
        ESP_LOGI(TAG, "Received MQTT off minutes: %d", moff);
    }
    const cJSON *modofunc = cJSON_GetObjectItemCaseSensitive(root, "modo");
    if (cJSON_IsString(modofunc) && select) {
        const char *modo_str = modofunc->valuestring;
        if (strcmp(modo_str, "Manual") == 0) {
            modo = 0; // Manual
            ESP_LOGI(TAG, "Received MQTT mode: Manual");
        } else if (strcmp(modo_str, "Automatico") == 0) {
            modo = 1; // Automático
            ESP_LOGI(TAG, "Received MQTT mode: Automatico");
        } else {
        ESP_LOGW(TAG, "Received unknown MQTT mode: %s", modo_str);
        }
        pant_main();
    }
    cJSON_Delete(root);
}
