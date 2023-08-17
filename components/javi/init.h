wifi_ap_record_t ap_info;
struct tm *timeinfo;
struct timeval tv;

rotary_encoder_t control = {
    .pin_a = 33,       // Número del GPIO al que está conectado el pin "A" del encoder
    .pin_b = 32,       // Número del GPIO al que está conectado el pin "B" del encoder
    .pin_btn = 35,     // Número del GPIO al que está conectado el botón del encoder
};


