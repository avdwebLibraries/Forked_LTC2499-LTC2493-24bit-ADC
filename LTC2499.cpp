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

*************************************************************************

This library can be used for: 
LTC2493: 2/4  channel 24bit ADC 
LTC2499: 8/16 channel 24bit ADC 

This library contains the ADC part of the combined library Ard2499 which contains the libraries for the LTC2499 24bit ADC and the 24AA025E48 EEPROM
See the 16-Channel 24-Bit ADC Data Acquisition Shield for Arduino from Iowa Scaled Engineering:
* https://www.iascaled.com/store/ARD-LTC2499
* GitHub https://github.com/IowaScaledEngineering/ard-ltc2499

Changelog
Version 1.0.0 
*/

#include <stdlib.h>
#include <string.h>

#include "LTC2499.h"

byte Ltc2499::changeChannel(byte channel)
{
	return(changeChannel(channel, true));
}

byte Ltc2499::changeChannel(byte channel, bool addStop)
{
	uint8_t config1=0, config2=0, blockingCountdown;	
	
	if (CHAN_TEMPERATURE == channel)
	{
		// The temperature channel is fake - it really has to change the config byte in the message
		currentChannel = CHAN_TEMPERATURE;
		config1 = 0x80 | _BV(CONFIG1_ENABLE);
		config2 = _BV(CONFIG2_ENABLE2) | _BV(CONFIG2_IM) | (CONFIG2_CONFBITS & currentConfig);
	}
	else
	{
		currentChannel = CONFIG1_CHANBITS & channel;
		config1 = 0x80 | _BV(CONFIG1_ENABLE) | (0x1F & currentChannel);
		config2 = _BV(CONFIG2_ENABLE2) | (CONFIG2_CONFBITS & currentConfig);
	}

	// If there's no  address, error out
	if(0 == i2cAddr_)
		return(ERR);

	// Block in 5ms increments for up to 200ms.  Given the conversion rate 
	// should be greater than 7.5sps, 200ms is more than we should ever have to
	// wait.

	blockingCountdown = 40;
	while(blockingCountdown--)
	{
		this->wireInterface->beginTransmission(i2cAddr_);
		this->wireInterface->write(config1);
		this->wireInterface->write(config2);
		if (0 != this->wireInterface->endTransmission(addStop))
		{
			if (0 == blockingCountdown)
				return(ERR);
			delay(5);
		}	
		else
			break;
	}

	return(SUCCESS);
}

byte Ltc2499::changeConfiguration(byte config)
{
	currentConfig = CONFIG2_CONFBITS & config;
	return(changeChannel(currentChannel));
}

long Ltc2499::read()
{
	unsigned long rawValue = readRaw();
	
	if (RAW_READ_ERROR == rawValue)
		return(READ_ERROR);

	uint8_t upperByte = 0xFF & (rawValue>>24);
	switch(upperByte)
	{
		case 0xC0:
			return(OVERRANGE_POSITIVE);
		case 0x3F:
			return(OVERRANGE_NEGATIVE);
	}
	rawValue = (rawValue & 0x7FFFFFFF)>>6; // Get rid of the sub-LSBs

	if (0x01000000 & rawValue)
		rawValue |= 0xFF000000;
	return((long)rawValue);

}

float Ltc2499::readVoltage()
{
	long value = read();
	
	if (OVERRANGE_POSITIVE == value)
		return(INFINITY);
	if (OVERRANGE_NEGATIVE == value)
		return(-1*INFINITY);

	return((value / 16777216.0) * (referenceMillivolts / 2000.0));
}

float Ltc2499::readVoltageAndChangeChannel(byte nextChannel)
{
	long value = readAndChangeChannel(nextChannel);
	
	if (OVERRANGE_POSITIVE == value)
		return(INFINITY);
	if (OVERRANGE_NEGATIVE == value)
		return(-1*INFINITY);

	return((value / 16777216.0) * (referenceMillivolts / 2000.0));
}

long Ltc2499::readAndChangeChannel(byte nextChannel)
{
	if (SUCCESS != changeChannel(nextChannel, false))
	{
		// Run another transmission through just to send out a stop bit
		this->wireInterface->beginTransmission(i2cAddr_);
		this->wireInterface->endTransmission((uint8_t)true);
		return(READ_ERROR);
	}
	// Otherwise, we've succeeded and are sitting on a restart condition
	return(read());
}

unsigned long Ltc2499::readRaw()
{
	unsigned long retval=0;
	uint8_t blockingCountdown=40;

	if (0 == i2cAddr_)
		return(0);

	// Block in 5ms increments for up to 200ms.  Given the conversion rate 
	// should be greater than 7.5sps, 200ms is more than we should ever have to
	// wait.
	while(blockingCountdown--)
	{
		this->wireInterface->requestFrom((uint8_t)i2cAddr_, (uint8_t)4, (uint8_t)true);
		// Error occurred, we don't have as many bytes as expected
		if (this->wireInterface->available() < 4)
		{
			if (0 == blockingCountdown)
				return(RAW_READ_ERROR);
			delay(5);
		}	
		else
			break;
	}

	retval |= this->wireInterface->read();
	retval <<= 8;
	retval |= this->wireInterface->read();
	retval <<= 8;
	retval |= this->wireInterface->read();
	retval <<= 8;
	retval |= this->wireInterface->read();

	return(retval);
}

unsigned long Ltc2499::readRawAndChangeChannel(byte nextChannel)
{
	if (SUCCESS != changeChannel(nextChannel, false))
	{
		// Run another transmission through just to send out a stop bit
		this->wireInterface->beginTransmission(i2cAddr_);
		this->wireInterface->endTransmission((uint8_t)true);
		return(RAW_READ_ERROR);
	}
	// Otherwise, we've succeeded and are sitting on a restart condition
	return(readRaw());
}

float Ltc2499::readTemperature(byte temperatureUnits)
{
	unsigned int tempDK = readTemperatureDeciK();
	float tempK = (float)tempDK/10.0;
	switch(temperatureUnits)
	{
		case TEMP_K:
			return(tempK);
		case TEMP_C:
			return(tempK - 273.15);
		case TEMP_F:
			return(((tempK - 273.15) * 9.0)  / 5.0 + 32.0);
	}
	return(0);
}

unsigned int Ltc2499::readTemperatureDeciK()
{
	unsigned long readVal = 0;
	unsigned long tempDK = 0;
	
	// If we're currently not set for the temperature channel, switch us over
	// If we are currently set for the temp channel, then that write will have
	// triggered a conversion, and readRaw will block until it's done
	if(CHAN_TEMPERATURE != currentChannel)
		changeChannel(CHAN_TEMPERATURE, true);
	
	readVal = readRaw();
	
	if (RAW_READ_ERROR == readVal)
		return(0);

	// Throw away the sub-LSBs
	readVal >>= 6;

	tempDK = (0x00FFFFFF & readVal);
	// Divide by 2 for FS = 1/2 VREF
	// Divide by 1570 (eqn from 2499 datasheet)
	// Divide by 100 to convert millivolts to decivolts
	tempDK = (tempDK * (unsigned long)referenceMillivolts) / 314000;

	// tempDK is now the temperature in deci-kelvin
	return(tempDK);
}

Ltc2499::Ltc2499()
{
	init_status = ERR;
 // init_status = ERR | EEPROM_ERR;
	i2cAddr_ = 0;
	//i2cAddr_eeprom = 0;
	currentConfig = 0;
	currentChannel = 0;

#ifdef ARDUINO_SAM_DUE
	wireInterface = &Wire1;
#else 
	wireInterface = &Wire;
#endif
}

Ltc2499::Ltc2499(TwoWire& wire)
{
	Ltc2499();
	this->wireInterface = &wire;
}

byte Ltc2499::begin(byte Address, uint16_t referenceMillivolts)
{
	byte retval = 0;
	byte i;
	
	init_status = SUCCESS;
	
	i2cAddr_ = Address;
	currentChannel = CHAN_DIFF_0P_1N;
	currentConfig = CONFIG2_60_50HZ_REJ;
	
	this->wireInterface->beginTransmission(i2cAddr_);
	this->wireInterface->write(0x80 | _BV(CONFIG1_ENABLE) | (0x1F & currentChannel));
	this->wireInterface->write(_BV(CONFIG2_ENABLE2) | (0x7F & currentConfig));
	retval = this->wireInterface->endTransmission(true);
	// Anything but zero means we couldn't initialize the LTC2499
	if (0 != retval)
	{
		i2cAddr_ = 0;
		init_status |= ERR;
	}
	this->referenceMillivolts = referenceMillivolts;
	
	return(init_status);
}
