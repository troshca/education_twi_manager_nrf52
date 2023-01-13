#ifndef MAIN_H__
#define MAIN_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "app_error.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_drv_gpiote.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_twi_mngr.h"
#include "boards.h"

//TWI manager
#include "twi_mng_ds1307.h"

#define TWI_INSTANCE_ID     0

//Timer
#include "app_timer.h"
#include "nrf_drv_clock.h"

//NRF_TWI_MNGR_DEF(twi_mngr_instance, 5, TWI_INSTANCE_ID);

#endif // MAIN_H__
