/*************************************************************************
Title:    LTC2493 / LTC2499 library
Authors:  Nathan D. Holmes <maverick@drgw.net>
File:     $Id: $
License:  GNU General Public License v3

LICENSE:
    Copyright (C) 2013 Nathan D. Holmes & Michael D. Petersen

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

*************************************************************************/

#ifndef LTC2499_H
#define LTC2499_H

#include <stdlib.h>
#include <string.h>
#include "Arduino.h"
#include "Wire.h"

#ifndef _BV
#define _BV(a) (1<<(a))
#define _UNDEFINE_BV
#endif

#define FAKE_CONFIG1_TEMPERATURE   6
#define CONFIG1_ENABLE     5
#define CONFIG1_SINGLE_END 4
#define CONFIG1_ODD        3
#define CONFIG1_A2         2
#define CONFIG1_A1         1
#define CONFIG1_A0         0

#define CHAN_DIFF_0P_1N    (0)
#define CHAN_DIFF_2P_3N    (_BV(CONFIG1_A0))
#define CHAN_DIFF_4P_5N    (_BV(CONFIG1_A1))
#define CHAN_DIFF_6P_7N    (_BV(CONFIG1_A1) | _BV(CONFIG1_A0))
#define CHAN_DIFF_8P_9N    (_BV(CONFIG1_A2))
#define CHAN_DIFF_10P_11N  (_BV(CONFIG1_A2) | _BV(CONFIG1_A0))
#define CHAN_DIFF_12P_13N  (_BV(CONFIG1_A2) | _BV(CONFIG1_A1))
#define CHAN_DIFF_14P_15N  (_BV(CONFIG1_A2) | _BV(CONFIG1_A1) | _BV(CONFIG1_A0))

#define CHAN_DIFF_1P_0N    (_BV(CONFIG1_ODD))
#define CHAN_DIFF_3P_2N    (_BV(CONFIG1_ODD) | _BV(CONFIG1_A0))
#define CHAN_DIFF_5P_4N    (_BV(CONFIG1_ODD) | _BV(CONFIG1_A1))
#define CHAN_DIFF_7P_6N    (_BV(CONFIG1_ODD) | _BV(CONFIG1_A1) | _BV(CONFIG1_A0))
#define CHAN_DIFF_9P_8N    (_BV(CONFIG1_ODD) | _BV(CONFIG1_A2))
#define CHAN_DIFF_11P_10N  (_BV(CONFIG1_ODD) | _BV(CONFIG1_A2) | _BV(CONFIG1_A0))
#define CHAN_DIFF_13P_12N  (_BV(CONFIG1_ODD) | _BV(CONFIG1_A2) | _BV(CONFIG1_A1))
#define CHAN_DIFF_15P_14N  (_BV(CONFIG1_ODD) | _BV(CONFIG1_A2) | _BV(CONFIG1_A1) | _BV(CONFIG1_A0))

#define CHAN_SINGLE_0P     (_BV(CONFIG1_SINGLE_END))
#define CHAN_SINGLE_1P     (_BV(CONFIG1_SINGLE_END) | _BV(CONFIG1_ODD))
#define CHAN_SINGLE_2P     (_BV(CONFIG1_SINGLE_END) | _BV(CONFIG1_A0))
#define CHAN_SINGLE_3P     (_BV(CONFIG1_SINGLE_END) | _BV(CONFIG1_ODD) | _BV(CONFIG1_A0))
#define CHAN_SINGLE_4P     (_BV(CONFIG1_SINGLE_END) | _BV(CONFIG1_A1))
#define CHAN_SINGLE_5P     (_BV(CONFIG1_SINGLE_END) | _BV(CONFIG1_ODD) | _BV(CONFIG1_A1))
#define CHAN_SINGLE_6P     (_BV(CONFIG1_SINGLE_END) | _BV(CONFIG1_A1) | _BV(CONFIG1_A0))
#define CHAN_SINGLE_7P     (_BV(CONFIG1_SINGLE_END) | _BV(CONFIG1_ODD) | _BV(CONFIG1_A1) | _BV(CONFIG1_A0))
#define CHAN_SINGLE_8P     (_BV(CONFIG1_SINGLE_END) | _BV(CONFIG1_A2))
#define CHAN_SINGLE_9P     (_BV(CONFIG1_SINGLE_END) | _BV(CONFIG1_ODD) | _BV(CONFIG1_A2))
#define CHAN_SINGLE_10P    (_BV(CONFIG1_SINGLE_END) | _BV(CONFIG1_A2) | _BV(CONFIG1_A0))
#define CHAN_SINGLE_11P    (_BV(CONFIG1_SINGLE_END) | _BV(CONFIG1_ODD) | _BV(CONFIG1_A2) | _BV(CONFIG1_A0))
#define CHAN_SINGLE_12P    (_BV(CONFIG1_SINGLE_END) | _BV(CONFIG1_A2) | _BV(CONFIG1_A1))
#define CHAN_SINGLE_13P    (_BV(CONFIG1_SINGLE_END) | _BV(CONFIG1_ODD) | _BV(CONFIG1_A2) | _BV(CONFIG1_A1))
#define CHAN_SINGLE_14P    (_BV(CONFIG1_SINGLE_END) | _BV(CONFIG1_A2) | _BV(CONFIG1_A1) | _BV(CONFIG1_A0))
#define CHAN_SINGLE_15P    (_BV(CONFIG1_SINGLE_END) | _BV(CONFIG1_ODD) | _BV(CONFIG1_A2) | _BV(CONFIG1_A1) | _BV(CONFIG1_A0))

// The temperature channel is actually fake - we manipulate things internally
#define CHAN_TEMPERATURE   (_BV(FAKE_CONFIG1_TEMPERATURE))

#define CONFIG2_ENABLE2     7
#define CONFIG2_IM          6
#define CONFIG2_FA          5
#define CONFIG2_FB          4
#define CONFIG2_SPD         3

#define CONFIG2_50HZ_REJ    (_BV(CONFIG2_FB))
#define CONFIG2_60HZ_REJ    (_BV(CONFIG2_FA))
#define CONFIG2_60_50HZ_REJ (0)  
#define CONFIG2_SPEED_2X    (_BV(CONFIG2_SPD))

#define CONFIG1_CHANBITS (_BV(CONFIG1_SINGLE_END) | _BV(CONFIG1_ODD) | _BV(CONFIG1_A2) | _BV(CONFIG1_A1) | _BV(CONFIG1_A0))
#define CONFIG2_CONFBITS (_BV(CONFIG2_FA) | _BV(CONFIG2_FB) | _BV(CONFIG2_SPD))

#define SUCCESS      0
#define ERR  1

#define ADDR_000   0x14
#define ADDR_00Z   0x15
#define ADDR_001   0x16
#define ADDR_0Z0   0x17
#define ADDR_0ZZ   0x24
#define ADDR_0Z1   0x25
#define ADDR_010   0x26
#define ADDR_01Z   0x27
#define ADDR_011   0x28
#define ADDR_Z00   0x35
#define ADDR_Z01   0x37
#define ADDR_Z0Z   0x36
#define ADDR_ZZ0   0x44
#define ADDR_ZZZ   0x45
#define ADDR_ZZ1   0x46
#define ADDR_Z10   0x47
#define ADDR_Z1Z   0x54
#define ADDR_Z11   0x55
#define ADDR_100   0x56
#define ADDR_10Z   0x57
#define ADDR_101   0x64
#define ADDR_1Z0   0x65
#define ADDR_1ZZ   0x66
#define ADDR_1Z1   0x67
#define ADDR_110   0x74
#define ADDR_111   0x76
#define ADDR_11Z   0x75

#define TEMP_DEG_F     0x01

#define	TEMP_K        0x00
#define TEMP_F        0x01
#define TEMP_C        0x02

#define RAW_READ_ERROR     0xFFFFFFFF

#define READ_ERROR         0x01000001
#define OVERRANGE_POSITIVE 0x01000000
#define OVERRANGE_NEGATIVE 0x11000000

class Ltc2499
{
	public:
		Ltc2499();
		Ltc2499(TwoWire& wire);

		byte begin(byte Address, uint16_t referenceMillivolts = 4096);

		long read(); // 24-bit plus sign
		long readAndChangeChannel(byte nextChannel); // 24-bit plus sign
		float readVoltage();
		float readVoltageAndChangeChannel(byte nextChannel);

		unsigned long readRaw();
		unsigned long readRawAndChangeChannel(byte nextChannel);	
		
		byte changeChannel(byte channel);
		byte changeConfiguration(byte config);
		unsigned int readTemperatureDeciK();
		float readTemperature(byte temperatureUnits);

	private:
		byte changeChannel(byte channel, bool addStop);
		uint8_t init_status;
		uint8_t i2cAddr_;
		uint8_t currentConfig;
		uint8_t currentChannel;
		uint16_t referenceMillivolts;
		TwoWire* wireInterface;
};

#endif 
