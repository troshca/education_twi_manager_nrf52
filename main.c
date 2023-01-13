#include "main.h"
NRF_TWI_MNGR_DEF(twi_mngr_instance, 5, TWI_INSTANCE_ID);
static void in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
  nrf_gpio_pin_toggle(LED_1);
}

static void gpiote_init()
{
  // VCE: This block is a one time configuration
  ret_code_t err_code;
  if (!nrf_drv_gpiote_is_init())
  {
    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);
  }

  // VCE: The below block needs to be called for each pin
  nrf_drv_gpiote_in_config_t in_config_1;
  in_config_1.pull = NRF_GPIO_PIN_PULLUP;            // User defined
  in_config_1.sense = GPIOTE_CONFIG_POLARITY_HiToLo; // User defined
  in_config_1.hi_accuracy = true;                    // User defined
  in_config_1.is_watcher = false;                    // Don't change this
  in_config_1.skip_gpio_setup = false;               // Don't change this

  // VCE: Configuring
  err_code = nrf_drv_gpiote_in_init(BUTTON_1, &in_config_1, in_pin_handler);
  APP_ERROR_CHECK(err_code);

  nrf_drv_gpiote_in_event_enable(BUTTON_1, true);
}

int main(void)
{
  RTCDateTime r;	
  nrf_gpio_cfg_output(LED_1);
  ret_code_t error_code = NRF_SUCCESS;

  // initialize RTT library
  error_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(error_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  NRF_LOG_INFO("Log initialized\n");
  NRF_LOG_FLUSH();

  // initialize TWI
  nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
  i2c_config.scl = SCL_PIN_NUMBER;
  i2c_config.sda = SDA_PIN_NUMBER;
  i2c_config.frequency = NRF_TWIM_FREQ_100K;
  error_code = nrf_twi_mngr_init(&twi_mngr_instance, &i2c_config);
  APP_ERROR_CHECK(error_code);

  //i2c_manager = &twi_mngr_instance;
  DS1307_Init(&twi_mngr_instance);
  // set_output(sideA);
  // set_input(DS1307_REG_SECOND);
  nrf_delay_ms(50);
  gpiote_init();
  //r.Year = 2023;
  //r.DayOfWeek = 4;
  //r.Month = 1;
  //r.Day = 12;
  //r.Hour = 16;
  //r.Minute = 46;
  //r.Second = 0;
  //DS1307_SetDateTime(&r);
  // DS1307_Init(&m_twi);
  //uint8_t date = DS1307_GetDate();
  //uint8_t month = DS1307_GetMonth();
  //uint16_t year = DS1307_GetYear();
  // uint8_t dow = DS1307_GetDayOfWeek();
  //uint8_t hour = DS1307_GetHour();
  //uint8_t minute = DS1307_GetMinute();
  //uint8_t second = DS1307_GetSecond();
  while (1)
  {
    // write_values(sideA, 0xff);
    // nrf_delay_ms(1000);
    // write_values(sideA, 0x00);
    // nrf_delay_ms(1000);

    //date = DS1307_GetDate();
    //month = DS1307_GetMonth();
    //year = DS1307_GetYear();
    // dow = DS1307_GetDayOfWeek();
    //hour = DS1307_GetHour();
    //minute = DS1307_GetMinute();
    //second = DS1307_GetSecond();
    // zone_hr = DS1307_GetTimeZoneHour();
    // zone_min = DS1307_GetTimeZoneMin();
    DS1307_GetDateTime(&r);
    NRF_LOG_INFO("%04d-%02d-%02d %02d:%02d:%02d%", r.Year, r.Month, r.Day, r.Hour, r.Minute, r.Second);
    //NRF_LOG_INFO("%04d-%02d-%02d %02d:%02d:%02d%", year, month, date, hour, minute, second);
    NRF_LOG_FLUSH();
    nrf_delay_ms(1000);
  }
}