#include "../ssd1306/ssd1306.h"
#include "../ssd1306/font8x8_basic.h"
#define tag "SH1106"

#define SDA_DIS	19 // Puerto SDA del i2c
#define SCL_DIS	18 // Puerto SCL del i2c
#define RST_DIS	-1 // Puerto Reset del i2c
#define HOR_RES	128 // Resolución horizontal
#define VER_RES	64 // Resolución vertical
#define DELAY_BIENV 5000 // Delay de pantalla de bienvenida
#define DELAY_CON 5000 // Delay de pantalla de conexión
int	CONTRAST = 15; // Contraste del display

SSD1306_t devd;

void config_dis (void);
void pant_bienv (void);
void pant_inicio (void);
void pant_nocon(void);
void pant_main (void);
void pant_no_sensor(void);
void menu0 (void);
void menu1 (void);

void config_dis (void)
{
	//CONFIG I2C FOR DISPLAY
	ESP_LOGI(tag, "DISPLAY INTERFACE is i2c");
	ESP_LOGI(tag, "SDA_DIS=%d",SDA_DIS);
	ESP_LOGI(tag, "SCL_DIS=%d",SCL_DIS);
	i2c_master_init(&devd, SDA_DIS, SCL_DIS, RST_DIS);
	ESP_LOGI(tag, "Panel is 128x64");
	ssd1306_init(&devd, HOR_RES, VER_RES);
	ssd1306_clear_screen(&devd, false);
	ssd1306_contrast(&devd, CONTRAST);
}

void pant_bienv (void)
{
    //MENSAJE DE BIENVENIDA
	ssd1306_clear_screen(&devd, false);
  	ssd1306_display_text(&devd, 0, "   Bienvenido", 13, false);
	ssd1306_display_text(&devd, 1, "   Sistema de", 13, false);
	ssd1306_display_text(&devd, 2, "    domotica", 12, false);
	ssd1306_display_text(&devd, 3, "  UBA Posgrado", 14, false);
	ssd1306_display_text(&devd, 4, "  Esp. en IoT", 13, false);
	ssd1306_display_text(&devd, 5, " Proyecto final", 15, false);
	ssd1306_display_text(&devd, 6, " Javier Fanelli", 15, false);
	ssd1306_display_text(&devd, 7, " Bs. As. - 2023", 15, false);
	vTaskDelay(1500 / portTICK_PERIOD_MS);
	ssd1306_fadeout(&devd);			// Fade Out
	ssd1306_clear_screen(&devd, false);
}

void pant_inicio ()
{
	ssd1306_display_text(&devd, 0, "Iniciando el", 12, false);
    ssd1306_display_text(&devd, 1, "sistema", 7, false);
}

void pant_conok ()
{
	ssd1306_clear_screen(&devd, false);
	ssd1306_display_text(&devd, 0, "IP address:", 11, false);
	ssd1306_display_text(&devd, 1, ip, strlen(ip), false);
	ssd1306_display_text(&devd, 2, "MAC address:", 12, false);
	ssd1306_display_text(&devd, 3, mac_short, strlen(mac_short), false);
	ssd1306_display_text(&devd, 4, "Gateway IP:", 11, false);
	ssd1306_display_text(&devd, 5, gw, strlen(gw), false);
	ssd1306_display_text_with_value(&devd, 6, "Pot senal: ", 11, RSSI_CHAR, strlen(RSSI_CHAR), false);
	ssd1306_display_text(&devd, 7, "Menu anterior", 13, true);
	if(btn_enc){
		btn_enc=false;
		xTaskCreate(menu0, "menu0", 4096, NULL, 5, NULL);
	}
}

void pant_nocon(void)
{
	ssd1306_clear_screen(&devd, false);
	vTaskDelay(100 / portTICK_PERIOD_MS);
	ssd1306_display_text(&devd, 0, "IP address:", 11, false);
	ssd1306_display_text(&devd, 1, "---.---.---.---", 15, false);
	ssd1306_display_text(&devd, 2, "MAC address:", 12, false);
	ssd1306_display_text(&devd, 3, "XXXXXXXX", 8, false);
	ssd1306_display_text(&devd, 4, "Gateway IP:", 11, false);
	ssd1306_display_text(&devd, 5, "---.---.---.---", 15, false);
	ssd1306_display_text(&devd, 6, "Pot senal: XX", 11, false);
	ssd1306_display_text(&devd, 7, "Menu anterior", 13, true);
	if(btn_enc){
		btn_enc=false;
		xTaskCreate(menu0, "menu0", 4096, NULL, 5, NULL);
	}
}

void pant_main(void)
{
	ssd1306_display_text(&devd, 0, pant_time, strlen(pant_time), true);
    sprintf(out_char, "%d", out_dim/102);
	ssd1306_display_text_with_value(&devd, 1, "Salida %: ", 10, out_char, strlen(out_char), false);
	if (out_dim == false){
		ssd1306_display_text(&devd, 3, "Salida: OFF", 11, false);
	}
	else {
		ssd1306_display_text(&devd, 3, "Salida: ON", 10, false);
	}
	if (net_con){
		ssd1306_display_text(&devd, 4, "Red: OK   ", 10, false);
	}
	if (!net_con) {
		ssd1306_display_text(&devd, 4, "Red: ERROR", 10, false);

	}
	if (mqtt_state){
		ssd1306_display_text(&devd, 5, "Server: ONLINE ", 15, false);
	}
	else if (!mqtt_state) {
		ssd1306_display_text(&devd, 5, "Server: OFFLINE", 15, false);
	}
	ssd1306_display_text(&devd, 7, "Menu", 4, true);
	if (btn_enc){
		level=1;
		btn_enc=false;
		
	}
}

void pant_no_sensor(void)
{
	ssd1306_clear_screen(&devd, false);
	vTaskDelay(100 / portTICK_PERIOD_MS);
	ssd1306_display_text(&devd, 0, "No hay sensor", 13, false);
	ssd1306_display_text(&devd, 1, "conectado", 9, false);
	ssd1306_clear_screen(&devd, false);
}

void menu0 (void)
{
	ssd1306_clear_screen(&devd, false);
	btn_enc=false;
	level=1;
	while(1){
	if (inc_enc){
		level++;
		if (level>4){
			level=1;
		}
	inc_enc=false;
	}
	if (dec_enc){
		level--;
		if (level<1){
			level=4;
		}
	dec_enc=false;
	}
	if(level==1){
		ssd1306_display_text(&devd, 0, "Estado", 6, true);
		ssd1306_display_text(&devd, 1, "Actualizar hora", 15, false);
		ssd1306_display_text(&devd, 2, "Modo", 4, false);
		ssd1306_display_text(&devd, 3, "Pant. principal", 15, false);
		if(btn_enc){
			menu1();
			vTaskDelete(NULL);
		}
	}
	if(level==2){
		ssd1306_display_text(&devd, 0, "Estado", 6, false);
		ssd1306_display_text(&devd, 1, "Actualizar hora", 15, true);
		ssd1306_display_text(&devd, 2, "Modo", 4, false);
		ssd1306_display_text(&devd, 3, "Pant. principal", 15, false);
	}
	if(level==3){
		ssd1306_display_text(&devd, 0, "Estado", 6, false);
		ssd1306_display_text(&devd, 1, "Actualizar hora", 15, false);
		ssd1306_display_text(&devd, 2, "Modo", 4, true);
		ssd1306_display_text(&devd, 3, "Pant. principal", 15, false);
	}
	if(level==4){
		ssd1306_display_text(&devd, 0, "Estado", 6, false);
		ssd1306_display_text(&devd, 1, "Actualizar hora", 15, false);
		ssd1306_display_text(&devd, 2, "Modo", 4, false);
		ssd1306_display_text(&devd, 3, "Pant. principal", 15, true);
		if (btn_enc){
			btn_enc=false;
			level=0;
			pant_main();
			vTaskDelete(NULL);
		}
	}}
	ssd1306_clear_screen(&devd, false);
	vTaskDelete(NULL);
}

void menu1 (void)
{
	ssd1306_clear_screen(&devd, false);
	if (net_con){
		btn_enc=false;
		pant_conok();
	}
	if (!net_con){
		btn_enc=false;
		pant_nocon();
	}
}