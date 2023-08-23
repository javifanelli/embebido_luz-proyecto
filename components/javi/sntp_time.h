/*
 * sntp_time.h
 *
 *  Created on: 2021-04-17
 *      Author: Leopoldo Zimperz
 */

#include <stddef.h>
#ifndef SNTP_TIME_H_
#define SNTP_TIME_H_
#include <stddef.h>

char formatted_time[20];

void obtain_time(void);
void initialize_sntp(void);
void set_times(void);
void power_on_device(void);
void get_device_uptime(char *uptime_str, size_t max_size);

#endif /* SNTP_TIME_H_ */
