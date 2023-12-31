#include <driver/ledc.h>

#define PWM_CHANNEL      LEDC_CHANNEL_0
#define PWM_GPIO_NUM     GPIO_NUM_13
#define PWM_FREQ_HZ      1000
#define PWM_RESOLUTION   LEDC_TIMER_10_BIT // Resolución de 10 bits (0-1023)
#define LED_R GPIO_NUM_15
#define LED_G GPIO_NUM_4
#define LED_B GPIO_NUM_2
#define BTN_MODO GPIO_NUM_0
#define CONTROL GPIO_NUM_13
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
void blink_2(void);
void blink_3(void);

void config_led (void)
{
    gpio_pad_select_gpio(LED_R);
    gpio_set_direction(LED_R, GPIO_MODE_OUTPUT);
	gpio_pad_select_gpio(LED_G);
    gpio_set_direction(LED_G, GPIO_MODE_OUTPUT);
	gpio_pad_select_gpio(CONTROL);
    gpio_set_direction(CONTROL, GPIO_MODE_OUTPUT);
	gpio_pad_select_gpio(LED_B);
    gpio_set_direction(LED_B, GPIO_MODE_OUTPUT);
	gpio_pad_select_gpio(BTN_MODO);
    gpio_set_direction(BTN_MODO, GPIO_MODE_INPUT);
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
    while(level==0){
		if (btn_enc){
			btn_enc=false;
			level=1;
			menu1();
		}
		if(modo==0) {	
			if(inc_enc){
				inc_enc=false;
				out_dim+=102;
					if(out_dim>1020)
						out_dim=1020;
				sprintf(out_char, "%d", out_dim/102*10);
				pant_main();
			}
			if(dec_enc){
				dec_enc=false;
				out_dim-=102;
				if(out_dim<102)
					out_dim=0;
				sprintf(out_char, "%d", out_dim/102*10);
				pant_main();
			}
		}
		sprintf(sp_char, "%d", set_point);
		set_pwm_duty(out_dim);
		vTaskDelay(pdMS_TO_TICKS(50));
	}
	vTaskDelete(NULL);
}

void blink_1(void){
	int i=0;
	bool led=false;
	while(i<2){
		if(!led){
			gpio_set_level(LED_B, 1);
			led=true;
		}
		else{
			gpio_set_level(LED_B, 0);
			led=false;
		}
		vTaskDelay(pdMS_TO_TICKS(400));
		i+=1;
	}
}

void blink_2(void){
	int i=0;
	bool led=false;
	while(i<4){
		if(!led){
			gpio_set_level(LED_B, 1);
			led=true;
		}
		else{
			gpio_set_level(LED_B, 0);
			led=false;
		}
		vTaskDelay(pdMS_TO_TICKS(200));
		i+=1;
	}
}

void blink_3(void){
	int i=0;
	bool led=false;
	while(i<8){
		if(!led){
			gpio_set_level(LED_B, 1);
			led=true;
		}
		else{
			gpio_set_level(LED_B, 0);
			led=false;
		}
		vTaskDelay(pdMS_TO_TICKS(100));
		i+=1;
	}
}