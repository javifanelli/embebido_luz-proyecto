/*
 * sntp_time.h
 *
 *  Created on: 2021-04-17
 *      Author: Leopoldo Zimperz
 */

#include <stddef.h>
#ifndef SNTP_TIME_H_
#define SNTP_TIME_H_

char formatted_time[20];

void obtain_time(void);
void initialize_sntp(void);
void power_on_device(void);
void update_device_start_time(void);
void get_device_uptime(char *uptime_str, size_t max_len);
void set_times(void);

#endif /* SNTP_TIME_H_ */
