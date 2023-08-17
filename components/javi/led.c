#include <driver/ledc.h>
#include "esp_wifi.h"
#include "esp_log.h"

#define PWM_CHANNEL      LEDC_CHANNEL_0
#define PWM_GPIO_NUM     GPIO_NUM_13
#define PWM_FREQ_HZ      1000
#define PWM_RESOLUTION   LEDC_TIMER_10_BIT // Resolución de 10 bits (0-1023)
#define LED_R GPIO_NUM_15
#define LED_G GPIO_NUM_4
#define TAG "Led"

TickType_t xLastWakeTime;
bool btn_enc;
extern bool inc_enc;
extern bool dec_enc;
extern int out_dim;

void config_led (void);
void pwm_init(void);
void set_pwm_duty(int);
void read_enc (void *pvParameter);

void config_led (void)
{
    gpio_pad_select_gpio(LED_R);
    gpio_set_direction(LED_R, GPIO_MODE_OUTPUT);
	gpio_pad_select_gpio(LED_G);
    gpio_set_direction(LED_G, GPIO_MODE_OUTPUT);
}

void pwm_init()
{
    
    // Configurar el módulo LEDC
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = PWM_RESOLUTION,
        .freq_hz = PWM_FREQ_HZ,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .channel = PWM_CHANNEL,
        .duty = 0,
        .gpio_num = PWM_GPIO_NUM,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_0
    };
    ledc_channel_config(&ledc_channel);
    ledc_fade_func_install(0);
	btn_enc=false;
	set_pwm_duty(0);
}

void set_pwm_duty(int duty)
{
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, PWM_CHANNEL, duty);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, PWM_CHANNEL);
}

void read_enc (void *pvParameter)
{
    while(1){
		if(btn_enc && out_dim>0){
			btn_enc=false;
			out_dim=0;
			ESP_LOGI(TAG,"Apago luz");
		}
		if(btn_enc && out_dim==0){
			btn_enc=false;
			out_dim=510;
			ESP_LOGI(TAG,"Prendo luz al 50");
		}
		if(inc_enc==true){
			inc_enc=false;
			out_dim+=102;
			if(out_dim>1020)
				out_dim=1020;
			ESP_LOGI(TAG,"Sube pwm");
		}
    	if(dec_enc==true){
			dec_enc=false;
			out_dim-=102;
			if(out_dim<1)
				out_dim=0;
			ESP_LOGI(TAG,"Baja pwm");
			
		}
		set_pwm_duty(out_dim);
		
		xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(50));
	}
	vTaskDelete(NULL);
}