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
void pant_nocon (void);
void pant_main (void);
void menu1 (void); // Menú de opciones
void menu2 (void); // Menú de elección de modo manual o automático

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
}

void pant_main(void)
{
	ssd1306_display_text(&devd, 0, pant_time, strlen(pant_time), true);
    sprintf(out_char, "%d", out_dim/102);
	ssd1306_display_text_with_value(&devd, 1, "Salida %: ", 10, out_char, strlen(out_char), false);
	if(!out_dim)
		ssd1306_display_text(&devd, 3, "Salida: OFF", 11, false);
	if(out_dim)
		ssd1306_display_text(&devd, 3, "Salida: ON", 10, false);
	if (mqtt_state){
		ssd1306_display_text(&devd, 5, "Server: ONLINE ", 15, false);
	}
	if (!mqtt_state) {
		ssd1306_display_text(&devd, 5, "Server: OFFLINE", 15, false);
	}
	ssd1306_display_text(&devd, 7, "Menu", 4, true);
	if (btn_enc){
		level=1;
		btn_enc=false;
		
	}
}

void menu1 (void)
{
	ssd1306_clear_screen(&devd, false);
	btn_enc=false;
	pos_menu=1;
	while(level==1){
		if(level>0 && inc_enc){
			pos_menu++;
			inc_enc=false;
		}	
		if (level>0 && dec_enc){
			pos_menu--;
			dec_enc=false;
		}
		if (pos_menu>4){
			pos_menu=1;
			}
		if (pos_menu<1){
			pos_menu=4;
			}
		if(pos_menu==1){
			ssd1306_display_text(&devd, 0, "Estado", 6, true);
			ssd1306_display_text(&devd, 1, "Modo", 4, false);
			ssd1306_display_text(&devd, 2, "Actualizar hora", 15, false);
			ssd1306_display_text(&devd, 3, "Pant. principal", 15, false);
			if(btn_enc){
				btn_enc=false;
				ssd1306_clear_screen(&devd, false);
				while(!btn_enc){
				if (net_con)
					pant_conok();
				
				if (!net_con)
					pant_nocon();
				}
				if(btn_enc){
				btn_enc=false;
				level=1;
				menu1();
				}
			}
		}
		if(pos_menu==2){
			ssd1306_display_text(&devd, 0, "Estado", 6, false);
			ssd1306_display_text(&devd, 1, "Modo", 4, true);
			ssd1306_display_text(&devd, 2, "Actualizar hora", 15, false);
			ssd1306_display_text(&devd, 3, "Pant. principal", 15, false);	
			if(btn_enc){
				btn_enc=false;
				level=2;
				menu2();
			}
		}
		if(pos_menu==3){
			ssd1306_display_text(&devd, 0, "Estado", 6, false);
			ssd1306_display_text(&devd, 1, "Modo", 4, false);
			ssd1306_display_text(&devd, 2, "Actualizar hora", 15, true);
			ssd1306_display_text(&devd, 3, "Pant. principal", 15, false);
			if(btn_enc){
				btn_enc=false;
				ssd1306_display_text(&devd, 5, "Obteniendo la", 13, false);
    			ssd1306_display_text(&devd, 6, "hora...", 7, false);
				obtain_time();
				ssd1306_display_text(&devd, 6, "hora... OK", 10, false);
				vTaskDelay(pdMS_TO_TICKS(1000));
				ssd1306_display_text(&devd, 5, "               ", 15, false);
				ssd1306_display_text(&devd, 6, "               ", 15, false);
			}
		}
		if(pos_menu==4){
			ssd1306_display_text(&devd, 0, "Estado", 6, false);
			ssd1306_display_text(&devd, 1, "Modo", 4, false);
			ssd1306_display_text(&devd, 2, "Actualizar hora", 15, false);
			ssd1306_display_text(&devd, 3, "Pant. principal", 15, true);
			if (btn_enc){
				btn_enc=false;
				level=0;
				ssd1306_clear_screen(&devd, false);
				pant_main();	
			}
		}
	}
}

void menu2 (void)
{
	ssd1306_clear_screen(&devd, false);
	pos_menu=1;
	while(level==2){
		if(level>0 && inc_enc){
			pos_menu++;
			inc_enc=false;
		}	
		if (level>0 && dec_enc){
			pos_menu--;
			dec_enc=false;
		}
		if (pos_menu>3){
			pos_menu=1;
			}
		if (pos_menu<1){
			pos_menu=3;
			}
		ssd1306_display_text(&devd, 4, "Modo actual:    ", 16, false);
		if(modo==0)
			ssd1306_display_text(&devd, 5, "Manual", 6, true);
		if(modo==1)
			ssd1306_display_text(&devd, 5, "Automatico", 10, true);
		if(pos_menu==1){
			ssd1306_display_text(&devd, 0, "Manual", 6, true);
			ssd1306_display_text(&devd, 1, "Automatico", 10, false);
			ssd1306_display_text(&devd, 2, "Menu anterior", 13, false);
			if(btn_enc){
				btn_enc=false;
				modo=0;
				ssd1306_display_text(&devd, 6, "Selecciono modo", 15, false);
				ssd1306_display_text(&devd, 7, "Manual", 6, true);
				vTaskDelay(pdMS_TO_TICKS(1500));
				ssd1306_display_text(&devd, 6, "                ", 16, false);
				ssd1306_display_text(&devd, 7, "                ", 16, false);
			}
		}
		if(pos_menu==2){
			ssd1306_display_text(&devd, 0, "Manual", 6, false);
			ssd1306_display_text(&devd, 1, "Automatico", 10, true);
			ssd1306_display_text(&devd, 2, "Menu anterior", 13, false);
			if(btn_enc){
				btn_enc=false;
				modo=1;
				ssd1306_display_text(&devd, 6, "Selecciono modo", 15, false);
				ssd1306_display_text(&devd, 7, "Automatico", 10, true);
				vTaskDelay(pdMS_TO_TICKS(1500));
				ssd1306_display_text(&devd, 6, "                ", 16, false);
				ssd1306_display_text(&devd, 7, "                ", 16, false);
			}
		}
		if(pos_menu==3){
			ssd1306_display_text(&devd, 0, "Manual", 6, false);
			ssd1306_display_text(&devd, 1, "Automatico", 10, false);
			ssd1306_display_text(&devd, 2, "Menu anterior", 13, true);
			if(btn_enc){
				btn_enc=false;
				level=1;
				menu1();
			}
		}
	}
}