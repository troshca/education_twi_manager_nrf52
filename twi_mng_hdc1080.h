#ifndef __HDC1080_H__
#define __HDC1080_H__

#include "main.h"

#define         HDC_1080_ADD                            (0x40)
#define         Configuration_register_add              (0x02)
#define         Temperature_register_add                (0x00)
#define         Humidity_register_add                   (0x01)

typedef enum
{
  Temperature_Resolution_14_bit = 0,
  Temperature_Resolution_11_bit = 1
}Temp_Reso;

typedef enum
{
  Humidity_Resolution_14_bit = 0,
  Humidity_Resolution_11_bit = 1,
  Humidity_Resolution_8_bit =2
}Humi_Reso;

static void HDC1080_CommandStartMeasuring();
static void HDC1080_CommandReceiveData();
void HDC1080_Start();
void HDC1080_ReceiveData();
void hdc1080_init(nrf_twi_mngr_t *nrf_twi_mngr_t, Temp_Reso Temperature_Resolution_x_bit, Humi_Reso Humidity_Resolution_x_bit, volatile float *temperature, volatile uint8_t *humidity);
void hdc1080_start_measurement(float* temperature, uint8_t* humidity);

#endif // __HDC1080_H__
