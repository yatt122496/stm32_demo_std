#ifndef _DHT11_H
#define _DHT11_H

#include "delay.h"
#include "sys.h"

u32 dht11_read(void);
void mode_output(void);
void mode_input(void);
void dht11_init (void);

#endif
