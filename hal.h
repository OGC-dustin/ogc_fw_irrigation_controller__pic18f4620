#ifndef HAL_H
#define HAL_H

#include <stdint.h>
#include <stdbool.h>
#include "hw_irrigation_controller_pic18f4620.h"

const bool OFF = false;
const bool ON = true;

/* User Interface - Binary Status Indicator */
void set_status_indicator( bool state );

/* NRTPS configuration support */
void hal_config( void );
void hal_init( void );
uint64_t hal_get_sys_tick( void );

#endif /* HAL_H */
