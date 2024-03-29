#include "main.h"

NRF_TWI_MNGR_DEF(twi_mngr_instance, 50, TWI_INSTANCE_ID);
APP_TIMER_DEF(m_repeated_timer_id);
RTCDateTime r;
volatile float temperature;
volatile uint8_t humidity;

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

/**@brief Function starting the internal LFCLK oscillator.
 *
 * @details This is needed by RTC1 which is used by the Application Timer
 *          (When SoftDevice is enabled the LFCLK is always running and this is not needed).
 */
static void lfclk_request(void)
{
  ret_code_t err_code = nrf_drv_clock_init();
  APP_ERROR_CHECK(err_code);
  nrf_drv_clock_lfclk_request(NULL);
}

/**@brief Timeout handler for the repeated timer.
 */
static void repeated_timer_handler(void *p_context)
{
  // DS1307_GetDateTime(&r);
  nrf_delay_ms(15);
  HDC1080_Start();
  nrf_delay_ms(15);
  HDC1080_ReceiveData();
  // hdc1080_start_measurement((float *)&temp, (uint8_t *)&humi);
  NRF_LOG_INFO("Temp " NRF_LOG_FLOAT_MARKER "*C Humidity %d%%\r\n", NRF_LOG_FLOAT(temperature), humidity);
  nrf_gpio_pin_toggle(LED_1);
  ssd1306_SetCursor(0, 0);
  char s3[8];
  // ssd1306_Fill(Black);
  DS1307_ScheduleDateAndTime();
  sprintf(s3, "%02d:%02d:%02d%", r.Hour, r.Minute, r.Second);
  ssd1306_WriteString(s3, Font_6x8, White);
  NRF_LOG_INFO("%04d-%02d-%02d %02d:%02d:%02d%", r.Year, r.Month, r.Day, r.Hour, r.Minute, r.Second);
  // NRF_LOG_INFO("%04d-%02d-%02d %02d:%02d:%02d%", year, month, date, hour, minute, second);
  // NRF_LOG_FLUSH();
  ssd1306_UpdateScreen();
}

/**@brief Create timers.
 */
static void create_timers()
{
  ret_code_t err_code;

  // Create timers
  err_code = app_timer_create(&m_repeated_timer_id,
                              APP_TIMER_MODE_REPEATED,
                              repeated_timer_handler);
  APP_ERROR_CHECK(err_code);
}

void twi_manager_init()
{
  // initialize TWI
  nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
  i2c_config.scl = SCL_PIN_NUMBER;
  i2c_config.sda = SDA_PIN_NUMBER;
  i2c_config.frequency = TWIM_FREQUENCY_FREQUENCY_K400;
  i2c_config.interrupt_priority = 6;
  APP_ERROR_CHECK(nrf_twi_mngr_init(&twi_mngr_instance, &i2c_config));
}

int main(void)
{
  ret_code_t err_code;
  nrf_gpio_cfg_output(LED_1);
  ret_code_t error_code = NRF_SUCCESS;

  // initialize RTT library
  error_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(error_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  NRF_LOG_INFO("Log initialized\n");
  NRF_LOG_FLUSH();
  err_code = nrf_pwr_mgmt_init();
  APP_ERROR_CHECK(err_code);
  twi_manager_init();

  // i2c_manager = &twi_mngr_instance;
  DS1307_Init(&twi_mngr_instance, &r);
  // set_output(sideA);
  // set_input(DS1307_REG_SECOND);
  nrf_delay_ms(50);
  gpiote_init();
  lfclk_request();
  app_timer_init();
  ssd1306_TWI_Init(&twi_mngr_instance);
  hdc1080_init(&twi_mngr_instance, Temperature_Resolution_14_bit, Humidity_Resolution_14_bit, &temperature, &humidity);
  create_timers();
  err_code = app_timer_start(m_repeated_timer_id, APP_TIMER_TICKS(1000), NULL);
  // APP_ERROR_CHECK(err_code);
  // ssd1306_TWI_Init(&twi_mngr_instance);
  // hdc1080_init(&twi_mngr_instance, Temperature_Resolution_14_bit, Humidity_Resolution_14_bit);
  //  r.Year = 2023;
  //  r.DayOfWeek = 4;
  //  r.Month = 1;
  //  r.Day = 13;
  //  r.Hour = 16;
  //  r.Minute = 0;
  //  r.Second = 0;
  //  DS1307_SetDateTime(&r);
  //   DS1307_Init(&m_twi);
  //  uint8_t date = DS1307_GetDate();
  //  uint8_t month = DS1307_GetMonth();
  //  uint16_t year = DS1307_GetYear();
  //   uint8_t dow = DS1307_GetDayOfWeek();
  //  uint8_t hour = DS1307_GetHour();
  //  uint8_t minute = DS1307_GetMinute();
  //  uint8_t second = DS1307_GetSecond();
  while (1)
  {
    // write_values(sideA, 0xff);
    // nrf_delay_ms(1000);
    // write_values(sideA, 0x00);
    // nrf_delay_ms(1000);

    // date = DS1307_GetDate();
    // month = DS1307_GetMonth();
    // year = DS1307_GetYear();
    //  dow = DS1307_GetDayOfWeek();
    // hour = DS1307_GetHour();
    // minute = DS1307_GetMinute();
    // second = DS1307_GetSecond();
    //  zone_hr = DS1307_GetTimeZoneHour();
    //  zone_min = DS1307_GetTimeZoneMin();
    // DS1307_GetDateTime(&r);
    // ssd1306_SetCursor(2, 24);
    // char s3[8];
    // ssd1306_Fill(Black);
    // DS1307_ScheduleDateAndTime();
    // sprintf(s3, "%02d:%02d:%02d%", r.Hour, r.Minute, r.Second);
    // ssd1306_WriteString(s3, Font_6x8, White);
    // NRF_LOG_INFO("%04d-%02d-%02d %02d:%02d:%02d%", r.Year, r.Month, r.Day, r.Hour, r.Minute, r.Second);
    // NRF_LOG_INFO("%04d-%02d-%02d %02d:%02d:%02d%", year, month, date, hour, minute, second);
    NRF_LOG_FLUSH();
    // ssd1306_UpdateScreen();
    nrf_pwr_mgmt_run();
    // nrf_delay_ms(1500);
  }
}
