/*
 *      twi_mng_ds1307.c
 *
 *	The MIT License.
 *      Created on: 09.01.2022
 *      Author: troshca
 *		Contact:
 *
 *      Website:
 *      GitHub: https://github.com/troshca
 */

#include "main.h"
#include "twi_mng_ds1307.h"

static const nrf_twi_mngr_t *TWI_manager = NULL;
RTCDateTime *DateTime;
uint8_t DS1307Buffer[7];

/**
 * @brief Initializes the DS1307 module, sets TWI_manager. Sets clock halt bit to 0 to start timing.
 * @param nrf_twi_mngr_t Pointer to the TWI transaction manager instance.
 * @param datetime Pointer to the user RTCDateTime struc
 */
void DS1307_Init(nrf_twi_mngr_t *nrf_twi_mngr_t, RTCDateTime *datetime)
{
	TWI_manager = nrf_twi_mngr_t;
	DateTime = datetime;
	DS1307_SetClockHalt(0);
}

/**
 * @brief Sets clock halt bit.
 * @param halt Clock halt bit to set, 0 or 1. 0 to start timing, 0 to stop.
 */
void DS1307_SetClockHalt(uint8_t halt)
{
	uint8_t ch = (halt ? 1 << 7 : 0);
	DS1307_SetRegByteTWIManager(DS1307_REG_SECOND, ch | (DS1307_GetRegByteTWIManager(DS1307_REG_SECOND) & 0x7f));
}

/**
 * @brief Gets clock halt bit.
 * @return Clock halt bit, 0 or 1.
 */
uint8_t DS1307_GetClockHalt(void)
{
	return (DS1307_GetRegByteTWIManager(DS1307_REG_SECOND) & 0x80) >> 7;
}

/**
 * @brief Sets the byte in the designated DS1307 register to value by TWI Manager
 * @param regAddr Register address to write.
 * @param val Value to set, 0 to 255.
 */
void DS1307_SetRegByteTWIManager(uint8_t regAddr, uint8_t val)
{
	uint8_t bytes[2] = {regAddr, val};

	nrf_twi_mngr_transfer_t const write_transfer[] =
		{
			NRF_TWI_MNGR_WRITE(DS1307_I2C_ADDR, bytes, 2, 0),
		};
	ret_code_t error_code = nrf_twi_mngr_perform(TWI_manager, NULL, write_transfer, 1, NULL);
	APP_ERROR_CHECK(error_code);
}

/**
 * @brief Gets the byte in the designated DS1307 register.
 * @param regAddr Register address to read.
 * @return Value stored in the register, 0 to 255.
 */
uint8_t DS1307_GetRegByteTWIManager(uint8_t regAddr)
{
	uint8_t val;
	nrf_twi_mngr_transfer_t const read_transfer[] =
		{
			NRF_TWI_MNGR_WRITE(DS1307_I2C_ADDR, &regAddr, 1, NRF_TWI_MNGR_NO_STOP),
			NRF_TWI_MNGR_READ(DS1307_I2C_ADDR, &val, 1, 0),
		};
	ret_code_t error_code = nrf_twi_mngr_perform(TWI_manager, NULL, read_transfer, 2, NULL);
	APP_ERROR_CHECK(error_code);
	return val;
}

/**
 * @brief Toggle square wave output on pin 7.
 * @param mode DS1307_ENABLED (1) or DS1307_DISABLED (0);
 */
void DS1307_SetEnableSquareWave(DS1307_SquareWaveEnable mode)
{
	uint8_t controlReg = DS1307_GetRegByteTWIManager(DS1307_REG_CONTROL);
	uint8_t newControlReg = (controlReg & ~(1 << 4)) | ((mode & 1) << 4);
	DS1307_SetRegByteTWIManager(DS1307_REG_CONTROL, newControlReg);
}

/**
 * @brief Set square wave output frequency.
 * @param rate DS1307_1HZ (0b00), DS1307_4096HZ (0b01), DS1307_8192HZ (0b10) or DS1307_32768HZ (0b11).
 */
void DS1307_SetInterruptRate(DS1307_Rate rate)
{
	uint8_t controlReg = DS1307_GetRegByteTWIManager(DS1307_REG_CONTROL);
	uint8_t newControlReg = (controlReg & ~0x03) | rate;
	DS1307_SetRegByteTWIManager(DS1307_REG_CONTROL, newControlReg);
}

/**
 * @brief Gets the current day of week.
 * @return Days from last Sunday, 0 to 6.
 */
uint8_t DS1307_GetDayOfWeek(void)
{
	return DS1307_DecodeBCD(DS1307_GetRegByteTWIManager(DS1307_REG_DOW));
}

/**
 * @brief Calculate Date and Time from DS1307_GetDateTime
 * @param DateTime RTCDateTime pointer
 */
void DS1307_CalculateDateTime()
{
	DateTime->Second = DS1307_DecodeBCD(DS1307Buffer[0]);
	DateTime->Minute = DS1307_DecodeBCD(DS1307Buffer[1]);
	DateTime->Hour = DS1307_DecodeBCD(DS1307Buffer[2] & 0x3F);
	DateTime->DayOfWeek = DS1307Buffer[3];
	DateTime->Day = DS1307_DecodeBCD(DS1307Buffer[4]);
	DateTime->Month = DS1307_DecodeBCD(DS1307Buffer[5] & 0x1F);
	DateTime->Year = 2000 + DS1307_DecodeBCD(DS1307Buffer[6]);
}

/**
 * @brief Callback from DS1307_GetDateTimeSchedule.
 * @note call DS1307_CalculateDateTime to convert array
 */
void DS1307_ScheduleDateAndTime()
{
	DS1307_GetDateTimeSchedule();
}

/**
 * @brief Callback from DS1307_GetDateTimeSchedule.
 * @note call DS1307_CalculateDateTime to convert array.
 * @note
 */
void DS1307_ReadDateTimeRegisters(ret_code_t result, void *p_user_data)
{
	if (result != NRF_SUCCESS)
	{
		NRF_LOG_WARNING("DS1307_ReadDateTimeRegisters - error: %d", (int)result);
		return;
	}
	DS1307_CalculateDateTime();
}

/**
 * @brief Gets the current time and date
 * @note call DS1307_CalculateDateTime to convert array
 */
static void DS1307_GetDateTimeSchedule()
{
	static nrf_twi_mngr_transfer_t const transfers[] =
		{
			NRF_TWI_MNGR_WRITE(DS1307_I2C_ADDR, DS1307_REG_SECOND, 1, NRF_TWI_MNGR_NO_STOP),
			NRF_TWI_MNGR_READ(DS1307_I2C_ADDR, &DS1307Buffer, 7, 0),
		};
	static nrf_twi_mngr_transaction_t NRF_TWI_MNGR_BUFFER_LOC_IND transaction =
		{
			.callback = DS1307_ReadDateTimeRegisters,
			.p_user_data = NULL,
			.p_transfers = transfers,
			.number_of_transfers = sizeof(transfers) / sizeof(transfers[0])};
	APP_ERROR_CHECK(nrf_twi_mngr_schedule(TWI_manager, &transaction));
}

/**
 * @brief Write memory
 * @param reg Register address
 * @param bufp - pointer on buffer to read
 * @param len - length of buffer
 */
void DS1307_WriteMem(uint8_t reg, uint8_t *bufp, uint16_t len)
{
	uint8_t buffer[1 + len];
	memcpy(buffer, &reg, 1);
	memcpy(buffer + 1, bufp, len);
	nrf_twi_mngr_transfer_t const read_transfer[] =
		{
			NRF_TWI_MNGR_WRITE(DS1307_I2C_ADDR, &buffer, len + 1, NRF_TWI_MNGR_NO_STOP),
		};
	ret_code_t error_code = nrf_twi_mngr_perform(TWI_manager, NULL, read_transfer, 2, NULL);
	APP_ERROR_CHECK(error_code);
}

/**
 * @brief Gets the current time and date
 * @note call DS1307_CalculateDateTime to convert array
 */
void DS1307_GetDateTime(RTCDateTime *DateTime)
{
	nrf_twi_mngr_transfer_t const read_transfer[] =
		{
			NRF_TWI_MNGR_WRITE(DS1307_I2C_ADDR, DS1307_REG_SECOND, 1, NRF_TWI_MNGR_NO_STOP),
			NRF_TWI_MNGR_READ(DS1307_I2C_ADDR, &DS1307Buffer, 7, 0),
		};
	ret_code_t error_code = nrf_twi_mngr_perform(TWI_manager, NULL, read_transfer, 2, NULL);
	APP_ERROR_CHECK(error_code);
	DS1307_CalculateDateTime(DateTime);
}

/**
 * @brief Gets the current day of month.
 * @return Day of month, 1 to 31.
 */
uint8_t DS1307_GetDate(void)
{
	return DS1307_DecodeBCD(DS1307_GetRegByteTWIManager(DS1307_REG_DATE));
}

/**
 * @brief Gets the current month.
 * @return Month, 1 to 12.
 */
uint8_t DS1307_GetMonth(void)
{
	return DS1307_DecodeBCD(DS1307_GetRegByteTWIManager(DS1307_REG_MONTH));
}

/**
 * @brief Gets the current year.
 * @return Year, 2000 to 2099.
 */
uint16_t DS1307_GetYear(void)
{
	uint16_t cen = DS1307_GetRegByteTWIManager(DS1307_REG_CENT) * 100;
	return DS1307_DecodeBCD(DS1307_GetRegByteTWIManager(DS1307_REG_YEAR)) + cen;
}

/**
 * @brief Gets the current hour in 24h format.
 * @return Hour in 24h format, 0 to 23.
 */
uint8_t DS1307_GetHour(void)
{
	return DS1307_DecodeBCD(DS1307_GetRegByteTWIManager(DS1307_REG_HOUR) & 0x3f);
}

/**
 * @brief Gets the current minute.
 * @return Minute, 0 to 59.
 */
uint8_t DS1307_GetMinute(void)
{
	return DS1307_DecodeBCD(DS1307_GetRegByteTWIManager(DS1307_REG_MINUTE));
}

/**
 * @brief Gets the current second. Clock halt bit not included.
 * @return Second, 0 to 59.
 */
uint8_t DS1307_GetSecond(void)
{
	return DS1307_DecodeBCD(DS1307_GetRegByteTWIManager(DS1307_REG_SECOND) & 0x7f);
}

/**
 * @brief Gets the stored UTC hour offset.
 * @note  UTC offset is not updated automatically.
 * @return UTC hour offset, -12 to 12.
 */
int8_t DS1307_GetTimeZoneHour(void)
{
	return DS1307_GetRegByteTWIManager(DS1307_REG_UTC_HR);
}

/**
 * @brief Gets the stored UTC minute offset.
 * @note  UTC offset is not updated automatically.
 * @return UTC time zone, 0 to 59.
 */
uint8_t DS1307_GetTimeZoneMin(void)
{
	return DS1307_GetRegByteTWIManager(DS1307_REG_UTC_MIN);
}

int dayofweek(int Day, int Month, int Year)
{
	int Y, C, M, N, D;
	M = 1 + (9 + Month) % 12;
	Y = Year - (M > 10);
	C = Y / 100;
	D = Y % 100;
	N = ((13 * M - 1) / 5 + D + D / 4 + 6 * C + Day + 5) % 7;
	return (7 + N) % 7;
}

void DS1307_SetDateTime(RTCDateTime *DateTime)
{
	uint8_t tmp[8];

	if (DateTime->Second > 59)
		DateTime->Second = 59;
	if (DateTime->Minute > 59)
		DateTime->Minute = 59;
	if (DateTime->Hour > 23)
		DateTime->Hour = 23;
	if (DateTime->Day > 31)
		DateTime->Day = 31;
	if (DateTime->Month > 12)
		DateTime->Month = 12;
	if (DateTime->Year > 2099)
		DateTime->Year = 2099;
	tmp[0] = DS1307_REG_SECOND;
	tmp[1] = DS1307_EncodeBCD(DateTime->Second);
	tmp[2] = DS1307_EncodeBCD(DateTime->Minute);
	tmp[3] = DS1307_EncodeBCD(DateTime->Hour);
	tmp[4] = dayofweek(DateTime->Day, DateTime->Month, DateTime->Year) + 1;
	tmp[5] = DS1307_EncodeBCD(DateTime->Day);
	tmp[6] = DS1307_EncodeBCD(DateTime->Month);
	tmp[7] = DS1307_EncodeBCD(DateTime->Year - 2000);

	nrf_twi_mngr_transfer_t const write_transfer[] =
		{
			NRF_TWI_MNGR_WRITE(DS1307_I2C_ADDR, tmp, sizeof(tmp), NRF_TWI_MNGR_NO_STOP),
		};
	ret_code_t error_code = nrf_twi_mngr_perform(TWI_manager, NULL, write_transfer, 1, NULL);
	APP_ERROR_CHECK(error_code);
}

/**
 * @brief Sets the current day of week.
 * @param dayOfWeek Days since last Sunday, 0 to 6.
 */
void DS1307_SetDayOfWeek(uint8_t dayOfWeek)
{
	DS1307_SetRegByteTWIManager(DS1307_REG_DOW, DS1307_EncodeBCD(dayOfWeek));
}

/**
 * @brief Sets the current day of month.
 * @param date Day of month, 1 to 31.
 */
void DS1307_SetDate(uint8_t date)
{
	DS1307_SetRegByteTWIManager(DS1307_REG_DATE, DS1307_EncodeBCD(date));
}

/**
 * @brief Sets the current month.
 * @param month Month, 1 to 12.
 */
void DS1307_SetMonth(uint8_t month)
{
	DS1307_SetRegByteTWIManager(DS1307_REG_MONTH, DS1307_EncodeBCD(month));
}

/**
 * @brief Sets the current year.
 * @param year Year, 2000 to 2099.
 */
void DS1307_SetYear(uint16_t year)
{
	DS1307_SetRegByteTWIManager(DS1307_REG_CENT, year / 100);
	DS1307_SetRegByteTWIManager(DS1307_REG_YEAR, DS1307_EncodeBCD(year % 100));
}

/**
 * @brief Sets the current hour, in 24h format.
 * @param hour_24mode Hour in 24h format, 0 to 23.
 */
void DS1307_SetHour(uint8_t hour_24mode)
{
	DS1307_SetRegByteTWIManager(DS1307_REG_HOUR, DS1307_EncodeBCD(hour_24mode & 0x3f));
}

/**
 * @brief Sets the current minute.
 * @param minute Minute, 0 to 59.
 */
void DS1307_SetMinute(uint8_t minute)
{
	DS1307_SetRegByteTWIManager(DS1307_REG_MINUTE, DS1307_EncodeBCD(minute));
}

/**
 * @brief Sets the current second.
 * @param second Second, 0 to 59.
 */
void DS1307_SetSecond(uint8_t second)
{
	uint8_t ch = DS1307_GetClockHalt();
	DS1307_SetRegByteTWIManager(DS1307_REG_SECOND, DS1307_EncodeBCD(second | ch));
}

/**
 * @brief Sets UTC offset.
 * @note  UTC offset is not updated automatically.
 * @param hr UTC hour offset, -12 to 12.
 * @param min UTC minute offset, 0 to 59.
 */
void DS1307_SetTimeZone(int8_t hr, uint8_t min)
{
	DS1307_SetRegByteTWIManager(DS1307_REG_UTC_HR, hr);
	DS1307_SetRegByteTWIManager(DS1307_REG_UTC_MIN, min);
}

/**
 * @brief Decodes the raw binary value stored in registers to decimal format.
 * @param bin Binary-coded decimal value retrieved from register, 0 to 255.
 * @return Decoded decimal value.
 */
uint8_t DS1307_DecodeBCD(uint8_t bin)
{
	return (((bin & 0xf0) >> 4) * 10) + (bin & 0x0f);
}

/**
 * @brief Encodes a decimal number to binaty-coded decimal for storage in registers.
 * @param dec Decimal number to encode.
 * @return Encoded binary-coded decimal value.
 */
uint8_t DS1307_EncodeBCD(uint8_t dec)
{
	return (dec % 10 + ((dec / 10) << 4));
}