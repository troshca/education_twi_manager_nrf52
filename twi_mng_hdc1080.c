/*
 *      twi_mng_hdc1080.c
 *
 *	The MIT License.
 *      Created on: 19.01.2022
 *      Author: troshca
 *		Contact:
 *
 *      Website:
 *      GitHub: https://github.com/troshca
 */

#include "twi_mng_hdc1080.h"
#include "main.h"

static const nrf_twi_mngr_t *TWI_manager = NULL;
uint8_t receive_data[4];
volatile float *temp;
volatile uint8_t *humi;

/**
 * @brief Initializes the HDC1080. Sets clock halt bit to 0 to start timing.
 * @param m_twi User TWI handle pointer.
 * @param Temp_Reso Temperature_Resolution in bits
 * @param Humi_Reso Humidity_Resolution in bits
 * @param temperature Pointer to the temperature
 * @param humidity Pointer to the humidity
 */
void hdc1080_init(nrf_twi_mngr_t *nrf_twi_mngr_t, Temp_Reso Temperature_Resolution_x_bit, Humi_Reso Humidity_Resolution_x_bit, volatile float *temperature, volatile uint8_t *humidity)
{
  /* Temperature and Humidity are acquired in sequence, Temperature first
   * Default:   Temperature resolution = 14 bit,
   *            Humidity resolution = 14 bit
   */
  /* Set the acquisition mode to measure both temperature and humidity by setting Bit[12] to 1 */
  TWI_manager = nrf_twi_mngr_t;

  temp = temperature;
  humi = humidity;
  ret_code_t err_code;
  uint16_t config_reg_value = 0x1000;
  uint8_t data_send[2];

  if (Temperature_Resolution_x_bit == Temperature_Resolution_11_bit)
  {
    config_reg_value |= (1 << 10); // 11 bit
  }

  switch (Humidity_Resolution_x_bit)
  {
  case Humidity_Resolution_11_bit:
    config_reg_value |= (1 << 8);
    break;
  case Humidity_Resolution_8_bit:
    config_reg_value |= (1 << 9);
    break;
  }

  data_send[0] = (config_reg_value >> 8);
  data_send[1] = (config_reg_value & 0x00ff);
  uint8_t buffer[1 + 2];
  memset(buffer, Configuration_register_add, 1);
  memcpy(buffer + 1, data_send, 2);
  nrf_twi_mngr_transfer_t const write_transfer[] =
      {
          NRF_TWI_MNGR_WRITE(HDC_1080_ADD, buffer, sizeof(buffer), NRF_TWI_MNGR_NO_STOP),
      };
  ret_code_t error_code = nrf_twi_mngr_perform(TWI_manager, NULL, write_transfer, 1, NULL);
  APP_ERROR_CHECK(error_code);
  NRF_LOG_FLUSH();
}

/**
 * @brief Measure temperature and humudity. 15ms delay in function
 * @param float Temperature
 * @param uint8_t Humidity
 */
void hdc1080_start_measurement(float *temperature, uint8_t *humidity)
{
  uint8_t receive_data[4];
  uint16_t temp_x, humi_x;
  uint8_t send_data = Temperature_register_add;
  ret_code_t err_code;

  nrf_twi_mngr_transfer_t const write_transfer[] =
      {
          NRF_TWI_MNGR_WRITE(HDC_1080_ADD, send_data, sizeof(send_data), NRF_TWI_MNGR_NO_STOP),
      };
  ret_code_t error_code = nrf_twi_mngr_perform(TWI_manager, NULL, write_transfer, 1, NULL);
  APP_ERROR_CHECK(error_code);
  nrf_delay_ms(15);
  nrf_twi_mngr_transfer_t const read_transfer[] =
      {
          NRF_TWI_MNGR_READ(HDC_1080_ADD, &receive_data, sizeof(receive_data), NRF_TWI_MNGR_NO_STOP),
      };
  error_code = nrf_twi_mngr_perform(TWI_manager, NULL, read_transfer, 1, NULL);
  APP_ERROR_CHECK(error_code);
  temp_x = ((receive_data[0] << 8) | receive_data[1]);
  humi_x = ((receive_data[2] << 8) | receive_data[3]);
  *temperature = ((temp_x / 65536.0) * 165.0) - 40.0;
  *humidity = (uint8_t)((humi_x / 65536.0) * 100.0);
}

/**
 * @brief Measure temperature and humudity. 15ms delay in function
 * @param float Temperature
 * @param uint8_t Humidity
 * @return 0
 */
static void HDC1080_CommandStartMeasuring()
{
  ret_code_t err_code;

  static nrf_twi_mngr_transfer_t const transfers[] =
      {
          NRF_TWI_MNGR_WRITE(HDC_1080_ADD, Temperature_register_add, 1, NRF_TWI_MNGR_NO_STOP),
      };
  static nrf_twi_mngr_transaction_t NRF_TWI_MNGR_BUFFER_LOC_IND transaction =
      {
          .callback = NULL,
          .p_user_data = NULL,
          .p_transfers = transfers,
          .number_of_transfers = sizeof(transfers) / sizeof(transfers[0])};
  APP_ERROR_CHECK(nrf_twi_mngr_schedule(TWI_manager, &transaction));
}

void HDC1080_Convert()
{
  uint16_t temp_x, humi_x;
  temp_x = ((receive_data[0] << 8) | receive_data[1]);
  humi_x = ((receive_data[2] << 8) | receive_data[3]);

  *temp = ((temp_x / 65536.0) * 165.0) - 40.0;
  *humi = (uint8_t)((humi_x / 65536.0) * 100.0);
}

/**
 * @brief Callback from HDC1080_ReceiveData.
 * @note call
 * @note
 */
void HDC1080_ReadRegisters(ret_code_t result, void *p_user_data)
{
  if (result != NRF_SUCCESS)
  {
    NRF_LOG_WARNING("DS1307_ReadDateTimeRegisters - error: %d", (int)result);
    return;
  }
  HDC1080_Convert();
}

static void HDC1080_CommandReceiveData()
{
  uint8_t send_data = Temperature_register_add;

  static nrf_twi_mngr_transfer_t const transfers[] =
      {
          NRF_TWI_MNGR_READ(HDC_1080_ADD, &receive_data, sizeof(receive_data), 0),
      };
  static nrf_twi_mngr_transaction_t NRF_TWI_MNGR_BUFFER_LOC_IND transaction =
      {
          .callback = HDC1080_ReadRegisters,
          .p_user_data = NULL,
          .p_transfers = transfers,
          .number_of_transfers = sizeof(transfers) / sizeof(transfers[0])};
  APP_ERROR_CHECK(nrf_twi_mngr_schedule(TWI_manager, &transaction));
}

void HDC1080_Start()
{
  HDC1080_CommandStartMeasuring();
}

void HDC1080_ReceiveData()
{
  HDC1080_CommandReceiveData();
}