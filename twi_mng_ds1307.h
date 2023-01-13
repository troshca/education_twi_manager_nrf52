/**
 * Copyright (c) 2015 - 2021, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef TWI_MNG_DS1307_H__
#define TWI_MNG_DS1307_H__

// #include "nrf_twi_mngr.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"

// Registers
#define DS1307_I2C_ADDR (0x68)
#define DS1307_REG_SECOND (0x00)
#define DS1307_REG_MINUTE (0x01)
#define DS1307_REG_HOUR (0x02)
#define DS1307_REG_DOW (0x03)
#define DS1307_REG_DATE (0x04)
#define DS1307_REG_MONTH (0x05)
#define DS1307_REG_YEAR (0x06)
#define DS1307_REG_CONTROL (0x07)
#define DS1307_REG_UTC_HR (0x08)
#define DS1307_REG_UTC_MIN (0x09)
#define DS1307_REG_CENT (0x10)
#define DS1307_REG_RAM (0x11)
#define DS1307_REG_RAM_END (0x3F)

#define DS1307_TIMEOUT (1000)

	typedef enum DS1307_Rate
	{
		DS1307_1HZ,
		DS1307_4096HZ,
		DS1307_8192HZ,
		DS1307_32768HZ
	} DS1307_Rate;

	typedef enum DS1307_SquareWaveEnable
	{
		DS1307_DISABLED,
		DS1307_ENABLED
	} DS1307_SquareWaveEnable;

	typedef struct
	{
		uint16_t Year;
		uint8_t Month;
		uint8_t Day;
		uint8_t Hour;
		uint8_t Minute;
		uint8_t Second;
		uint8_t DayOfWeek;
	} RTCDateTime;

	void DS1307_Init(nrf_twi_mngr_t *nrf_twi_mngr_t);

	void DS1307_SetClockHalt(uint8_t halt);
	uint8_t DS1307_GetClockHalt(void);

	void DS1307_SetRegByteTWIManager(uint8_t regAddr, uint8_t val);
	uint8_t DS1307_GetRegByteTWIManager(uint8_t regAddr);

	void DS1307_SetEnableSquareWave(DS1307_SquareWaveEnable mode);
	void DS1307_SetInterruptRate(DS1307_Rate rate);

	uint8_t DS1307_GetDayOfWeek(void);
	uint8_t DS1307_GetDate(void);
	uint8_t DS1307_GetMonth(void);
	uint16_t DS1307_GetYear(void);

	uint8_t DS1307_GetHour(void);
	uint8_t DS1307_GetMinute(void);
	uint8_t DS1307_GetSecond(void);
	int8_t DS1307_GetTimeZoneHour(void);
	uint8_t DS1307_GetTimeZoneMin(void);

	void DS1307_SetDayOfWeek(uint8_t dow);
	void DS1307_SetDate(uint8_t date);
	void DS1307_SetMonth(uint8_t month);
	void DS1307_SetYear(uint16_t year);

	void DS1307_SetHour(uint8_t hour_24mode);
	void DS1307_SetMinute(uint8_t minute);
	void DS1307_SetSecond(uint8_t second);
	void DS1307_SetTimeZone(int8_t hr, uint8_t min);

	uint8_t DS1307_DecodeBCD(uint8_t bin);
	uint8_t DS1307_EncodeBCD(uint8_t dec);

#ifdef __cplusplus
}
#endif

#endif // TWI_MNG_DS1307_H__
