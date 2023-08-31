// Variables globales usadas
#ifndef VARIABLES_H
#define VARIABLES_H

uint8_t mac[6]; // MAC del dispositivo ESP32
static char mac_str[18]; // MAC del dispositivo ESP32 en char
static char mac_short[13]; // MAC del dispositivo ESP32 en char sin los :
static char ip[16]; // IP del dispositivo ESP32
static char gw[16]; // IP del gateway de red
char formatted_time[20]; // Fecha y hora en char para mandar por MQTT
char pant_time[18]; // Fecha y hora en char para mostrar en display
char pant_on_time[8];
char pant_off_time[8];

static int8_t rssi = 0; // Potencia de la señal
char RSSI_CHAR[10]; // Potencia de la señal en char
bool net_con = false; // Variable global para almacenar el estado de la conexión WiFi
bool mqtt_state = false; // Variable para detectar el estado de conexión al servidor por MQTT
int out_dim = 0;
char out_char[20];
int set_point = 50; // Variable para ajustar el valor de set point de la salida enter 0 y 100
char sp_char[20]; // Variable de set point en char
int modo = 0; // Variable para el modo. 0=manual, 1=automático

bool time_func=false; // Variable para analizar si debe activarse la salida
int hon=20; // Hora de encendido
int mon=0; // Minutos de encendido
int hoff=8; // Hora de encendido
int moff=0; // Minutos de encendido

bool btn_enc = false; // Variable para detectar el botón pulsado
bool inc_enc = false; // Variable para detectar el giro horario del encoder
bool dec_enc = false; // Variable para detectar el giro antihorario del encoder

int level = 0; // Variable para detectar el nivel del menú
int pos_menu = 0; // Variable para detectar la posición dentro de los menúes

char tipo_disp[50]="Luz dimmer";
/* char *ID ="2"; */
char *ID ="0128192332001"; // 2 digitos para el tipo, 4 digitos de codigo, 2 digitos para el año, 2 digitos para la semana, 3 digitos para el numero de dispositivo
TickType_t xLastWakeTime;

#endif