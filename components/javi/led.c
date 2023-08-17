#include <driver/ledc.h>

#define PWM_CHANNEL      LEDC_CHANNEL_0
#define PWM_GPIO_NUM     GPIO_NUM_13
#define PWM_FREQ_HZ      1000
#define PWM_RESOLUTION   LEDC_TIMER_10_BIT // Resolución de 10 bits (0-1023)
#define LED_R GPIO_NUM_15
#define LED_G GPIO_NUM_4
#define refresh 5 // tiempo en segundos para refrescar medición en display
#define TAG "Led"

TickType_t xLastWakeTime;

bool btn_enc;
extern bool inc_enc;
extern bool dec_enc;
extern int out_dim;


void config_led (void);
void pwm_init (void);
void set_pwm_duty (int);
void read_enc (void *pvParameter);


void config_led (void)
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << GPIO_NUM_2);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
	gpio_pad_select_gpio(LED_R);
    gpio_set_direction(LED_R, GPIO_MODE_OUTPUT);
	gpio_pad_select_gpio(LED_G);
    gpio_set_direction(LED_G, GPIO_MODE_OUTPUT);
}

void pwm_init()
{
    // Configuración del módulo LED
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
		if (level==0 && btn_enc){
			btn_enc=false;
			level=1;
			menu1();
		}
		if (modo==1 && level==0 && inc_enc){	
			inc_enc=false;
			set_point+=1;
				if(set_point>28)
					set_point=28;
			sprintf(sp_char, "%d", set_point);
			pant_main();
		}	
    	if(modo==1 && level==0 && dec_enc){
			dec_enc=false;
			set_point-=1;
			if(set_point<18)
				set_point=18;
			sprintf(sp_char, "%d", set_point);
			pant_main();
			}
		if(modo==0 && level==0 && dec_enc){
			dec_enc=false;
			out_dim-=102;
			if(out_dim<102)
				out_dim=0;
			pant_main();
		}
		if(modo==0 && level==0 && inc_enc){	
			inc_enc=false;
			out_dim+=102;
			if(out_dim>1020)
				out_dim=1020;
			pant_main();
		}
		set_pwm_duty(out_dim);
        sprintf(out_char, "%d", out_dim/102*10);
		xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(50));
	}
	vTaskDelete(NULL);
}