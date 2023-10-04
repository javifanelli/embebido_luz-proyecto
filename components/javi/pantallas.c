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
void pant_est (void);
void menu1 (void); // Menú de opciones
void menu2 (void); // Menú de elección de modo manual o automático
void menu3 (void); // Menú para ver hora de encendido y apagado del modo automático

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
	ssd1306_fadeout(&devd);
	ssd1306_clear_screen(&devd, false);
}

void pant_inicio ()
{
	ssd1306_display_text(&devd, 0, "Iniciando el", 12, false);
    ssd1306_display_text(&devd, 1, "sistema", 7, false);
}

void pant_est()
{
	ssd1306_clear_screen(&devd, false);
	char uptime_buffer[20];
	get_device_uptime(uptime_buffer, sizeof(uptime_buffer));
	if(net_con)
		ssd1306_display_text(&devd, 0, "Red: ONLINE ", 12, false);
	else if(!net_con)
		ssd1306_display_text(&devd, 0, "Red: OFFLINE", 12, false);
	if(mqtt_state)
		ssd1306_display_text(&devd, 1, "Server: ONLINE ", 15, false);
	else if(!mqtt_state)
		ssd1306_display_text(&devd, 1, "Server: OFFLINE", 15, false);
	ssd1306_display_text(&devd, 2, "Tiempo encendido", 16, false);
	ssd1306_display_text(&devd, 3, uptime_buffer, strlen(uptime_buffer), false);
	ssd1306_display_text(&devd, 4, "ID dispositivo", 14, false);
	ssd1306_display_text(&devd, 5, ID, strlen(ID), false);
	ssd1306_display_text(&devd, 7, "Menu anterior", 13, true);
	while(level==11){
		if(btn_enc){
			btn_enc=false;
			level=1;
		}
	}
	if(level==1){
		menu1();
	}
}

void pant_conok ()
{
	ssd1306_clear_screen(&devd, false);
	ssd1306_display_text(&devd, 0, "IP address", 10, false);
	ssd1306_display_text(&devd, 1, ip, strlen(ip), false);
	ssd1306_display_text(&devd, 2, "MAC address", 11, false);
	ssd1306_display_text(&devd, 3, mac_short, strlen(mac_short), false);
	ssd1306_display_text(&devd, 4, "IP gateway", 10, false);
	ssd1306_display_text(&devd, 5, gw, strlen(gw), false);
	ssd1306_display_text_with_value(&devd, 6, "Pot senal: ", 11, RSSI_CHAR, strlen(RSSI_CHAR), false);
	ssd1306_display_text(&devd, 7, "Menu anterior", 13, true);
	while(level==10){
		if(btn_enc){
			btn_enc=false;
			level=1;
		}
	}
	if(level==1){
		menu1();
	}
}

void pant_nocon(void)
{
	ssd1306_clear_screen(&devd, false);
	ssd1306_display_text(&devd, 0, "IP address", 10, false);
	ssd1306_display_text(&devd, 1, "---.---.---.---", 15, false);
	ssd1306_display_text(&devd, 2, "MAC address:", 12, false);
	ssd1306_display_text(&devd, 3, "XXXXXXXX", 8, false);
	ssd1306_display_text(&devd, 4, "IP gateway", 10, false);
	ssd1306_display_text(&devd, 5, pant_time, strlen(pant_time), false);
	ssd1306_display_text(&devd, 6, "Pot senal: XXX", 14, false);
	ssd1306_display_text(&devd, 7, "Menu anterior", 13, false);
	while(level==10){
		if(btn_enc){
			btn_enc=false;
			level=1;
		}
	}
	if(level==1){
		menu1();
	}
}

void pant_main(void)
{
	ssd1306_display_text(&devd, 0, pant_time, strlen(pant_time), true);
	ssd1306_display_text_with_value(&devd, 1, "Nivel %: ", 9, out_char, 7, false);
	if(out_dim==0)
		ssd1306_display_text(&devd, 2, "Salida: OFF     ", 16, false);
	if(out_dim>0)
		ssd1306_display_text(&devd, 2, "Salida: ON      ", 16, false);
	if(modo==0)
		ssd1306_display_text(&devd, 3, "Modo: Manual", 16, false);
	if(modo==1)
		ssd1306_display_text(&devd, 3, "Modo: Automatico", 16, false);
	ssd1306_display_text(&devd, 4, "                ", 16, false);
	ssd1306_display_text(&devd, 5, "                ", 16, false);
	ssd1306_display_text(&devd, 6, "                ", 16, false);
	ssd1306_display_text(&devd, 7, "Menu", 4, true);
}

void menu1 (void)
{
	ssd1306_clear_screen(&devd, false);
	pos_menu=1;
	while(level==1){
		if(inc_enc){
			pos_menu++;
			inc_enc=false;
			if (pos_menu>6)
				pos_menu=1;
		}	
		if (dec_enc){
			pos_menu--;
			dec_enc=false;
			if (pos_menu<1)
				pos_menu=6;
		}
		if(pos_menu==1){
			ssd1306_display_text(&devd, 0, "Estado          ", 16, true);
			ssd1306_display_text(&devd, 1, "Info conexion   ", 16, false);
			ssd1306_display_text(&devd, 2, "Modo            ", 16, false);
			ssd1306_display_text(&devd, 3, "Conf modo auto  ", 16, false);
			ssd1306_display_text(&devd, 4, "Actualizar hora ", 16, false);
			ssd1306_display_text(&devd, 5, "Pant. principal ", 16, false);
			if(btn_enc){
				btn_enc=false;
				level=10;
			}
		}
		if(pos_menu==2){
			ssd1306_display_text(&devd, 0, "Estado          ", 16, false);
			ssd1306_display_text(&devd, 1, "Info conexion   ", 16, true);
			ssd1306_display_text(&devd, 2, "Modo            ", 16, false);
			ssd1306_display_text(&devd, 3, "Conf modo auto  ", 16, false);
			ssd1306_display_text(&devd, 4, "Actualizar hora ", 16, false);
			ssd1306_display_text(&devd, 5, "Pant. principal ", 16, false);
			if(btn_enc){
				btn_enc=false;
				level=11;
			}
		}
		if(pos_menu==3){
			ssd1306_display_text(&devd, 0, "Estado          ", 16, false);
			ssd1306_display_text(&devd, 1, "Info conexion   ", 16, false);
			ssd1306_display_text(&devd, 2, "Modo            ", 16, true);
			ssd1306_display_text(&devd, 3, "Conf modo auto  ", 16, false);
			ssd1306_display_text(&devd, 4, "Actualizar hora ", 16, false);
			ssd1306_display_text(&devd, 5, "Pant. principal ", 16, false);	
			if(btn_enc){
				btn_enc=false;
				level=2;
			}
		}
		if(pos_menu==4){
			ssd1306_display_text(&devd, 0, "Estado          ", 16, false);
			ssd1306_display_text(&devd, 1, "Info conexion   ", 16, false);
			ssd1306_display_text(&devd, 2, "Modo            ", 16, false);
			ssd1306_display_text(&devd, 3, "Conf modo auto  ", 16, true);
			ssd1306_display_text(&devd, 4, "Actualizar hora ", 16, false);
			ssd1306_display_text(&devd, 5, "Pant. principal ", 16, false);	
			if(btn_enc){
				btn_enc=false;
				level=3;
			}
		}
		if(pos_menu==5){
			ssd1306_display_text(&devd, 0, "Estado          ", 16, false);
			ssd1306_display_text(&devd, 1, "Info conexion   ", 16, false);
			ssd1306_display_text(&devd, 2, "Modo            ", 16, false);
			ssd1306_display_text(&devd, 3, "Conf modo auto  ", 16, false);
			ssd1306_display_text(&devd, 4, "Actualizar hora ", 16, true);
			ssd1306_display_text(&devd, 5, "Pant. principal ", 16, false);
			if(btn_enc){
				btn_enc=false;
				ssd1306_display_text(&devd, 6, "Obteniendo la", 13, false);
    			ssd1306_display_text(&devd, 7, "hora...", 7, false);
				obtain_time();
				ssd1306_display_text(&devd, 7, "hora... OK", 10, false);
				vTaskDelay(pdMS_TO_TICKS(1000));
				ssd1306_display_text(&devd, 6, "               ", 15, false);
				ssd1306_display_text(&devd, 7, "               ", 15, false);
			}
		}
		if(pos_menu==6){
			ssd1306_display_text(&devd, 0, "Estado          ", 16, false);
			ssd1306_display_text(&devd, 1, "Info conexion   ", 16, false);
			ssd1306_display_text(&devd, 2, "Modo            ", 16, false);
			ssd1306_display_text(&devd, 3, "Conf modo auto  ", 16, false);
			ssd1306_display_text(&devd, 4, "Actualizar hora ", 16, false);
			ssd1306_display_text(&devd, 5, "Pant. principal ", 16, true);
			if (btn_enc){
				btn_enc=false;
				level=0;	
			}
		}
	}
	if(level==10){
			if (net_con)
				pant_conok();
			if (!net_con)
				pant_nocon();
	}
	if(level==11){
		pant_est();
	}
	if(level==2){
		menu2();
	}
	if(level==3){
		menu3();
	}
	if(level==0){
		pant_main();
	}
}

void menu2 (void)
{
	ssd1306_clear_screen(&devd, false);
	pos_menu=1;
	while(level==2){
		if(inc_enc){
			pos_menu++;
			inc_enc=false;
			if (pos_menu>3)
				pos_menu=1;
		}	
		if (dec_enc){
			pos_menu--;
			dec_enc=false;
			if (pos_menu<1)
				pos_menu=3;
		}
		ssd1306_display_text(&devd, 4, "Modo actual:    ", 16, false);
		if(modo==0)
			ssd1306_display_text(&devd, 5, "Manual", 6, true);
		if(modo==1)
			ssd1306_display_text(&devd, 5, "Automatico", 10, true);
		if(pos_menu==1){
			ssd1306_display_text(&devd, 0, "Manual          ", 16, true);
			ssd1306_display_text(&devd, 1, "Automatico      ", 16, false);
			ssd1306_display_text(&devd, 2, "Menu anterior   ", 16, false);
			if(btn_enc){
				btn_enc=false;
				modo=0;
				out_dim=0;
				sprintf(out_char, "%d", out_dim/102*10);
				ssd1306_display_text(&devd, 6, "Selecciono modo", 15, false);
				ssd1306_display_text(&devd, 7, "Manual", 6, true);
				vTaskDelay(pdMS_TO_TICKS(1500));
				ssd1306_display_text(&devd, 6, "                ", 16, false);
				ssd1306_display_text(&devd, 7, "                ", 16, false);
			}
		}
		if(pos_menu==2){
			ssd1306_display_text(&devd, 0, "Manual          ", 16, false);
			ssd1306_display_text(&devd, 1, "Automatico      ", 16, true);
			ssd1306_display_text(&devd, 2, "Menu anterior   ", 16, false);
			if(btn_enc){
				btn_enc=false;
				modo=1;
				out_dim=0;
				sprintf(out_char, "%d", out_dim/102*10);
				ssd1306_display_text(&devd, 6, "Selecciono modo", 15, false);
				ssd1306_display_text(&devd, 7, "Automatico", 10, true);
				vTaskDelay(pdMS_TO_TICKS(1500));
				ssd1306_display_text(&devd, 6, "                ", 16, false);
				ssd1306_display_text(&devd, 7, "                ", 16, false);
			}
		}
		if(pos_menu==3){
			ssd1306_display_text(&devd, 0, "Manual          ", 16, false);
			ssd1306_display_text(&devd, 1, "Automatico      ", 16, false);
			ssd1306_display_text(&devd, 2, "Menu anterior   ", 16, true);
			if(btn_enc){
				btn_enc=false;
				level=1;
			}
		}
	}
	if(level==1){
		menu1();
	}
}

void menu3(void)
{
	ssd1306_clear_screen(&devd, false);
	ssd1306_display_text(&devd, 0, "Hora encendido  ", 16, false);
	ssd1306_display_text_with_value(&devd, 1, "     ", 5, pant_on_time, strlen(pant_on_time), false);
	ssd1306_display_text(&devd, 2, "Hora apagado    ", 16, false);
	ssd1306_display_text_with_value(&devd, 3, "     ", 5, pant_off_time, strlen(pant_off_time), false);
	ssd1306_display_text(&devd, 4, "Valor encendido ", 16, false);
	ssd1306_display_text_with_value(&devd, 5, "       ", 7, sp_char, strlen(sp_char), false);
	ssd1306_display_text(&devd, 6, "Menu anterior   ", 16, true);
	while(level==3){
		if(btn_enc){
		btn_enc=false;
		level=1;
		}
	}		
	if(level==1){
		menu1();
	}
}