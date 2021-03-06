#include "MAX44009.h"
#include <math.h>

MAX44009::MAX44009() {}

int MAX44009::begin()
{
	Wire.beginTransmission(MAX_ADDR);
	Wire.write(0x02);
	Wire.write(0x40);
	return Wire.endTransmission();//I2C_ERROR_OK=0,
}

//writen by zell
bool MAX44009::begin2(uint8_t addr) 
{
	Wire.beginTransmission(addr);
	if (MAX44009::manual_mode) {
		Wire.write(MAX44009_CONFIGURATION);
		Wire.write(MAX44009_manualmode);
	}
	int res = Wire.endTransmission();  
	if (0 == res)
		return true;
    else 
		return false;
}

float MAX44009::get_lux(void)
{
	unsigned int data[2];
	Wire.beginTransmission(MAX_ADDR);
	Wire.write(0x03);
	Wire.endTransmission();
 
	// Request 2 bytes of data
	Wire.requestFrom(MAX_ADDR, 2);
 
	// Read 2 bytes of data luminance msb, luminance lsb
	if (Wire.available() == 2)
	{
    	data[0] = Wire.read();  // high byte
    	data[1] = Wire.read(); // low byte
	}
 
	// Convert the data to lux
	int exponent = (data[0] & 0xF0) >> 4;
	int mantissa = ((data[0] & 0x0F) << 4) | (data[1] & 0x0F);
	
	//float luminance = pow(2, exponent) * mantissa * 0.045;
	float luminance = (float)(((0x00000001 << exponent) * (float)mantissa) * 0.045);
  
	return luminance; 
}

/*
//read both high and low lux reg test
float MAX44009::read_lux(void)
{
	unsigned int data[2];
	Wire.beginTransmission(MAX_ADDR);
	Wire.write(0x03);
	Wire.endTransmission();
 
	// Request 2 bytes of data
	Wire.requestFrom(MAX_ADDR, 2);
 
	// Read 2 bytes of data luminance msb, luminance lsb
	if (Wire.available() == 2)
	{
    	data[0] = Wire.read();
    	data[1] = Wire.read();
	}
 
	// Convert the data to lux
	int exponent = (data[0] & 0xF0) >> 4;
	int mantissa = ((data[0] & 0x0F) << 4) | (data[1] & 0x0F);
	
	//float luminance = pow(2, exponent) * mantissa * 0.045;
	float luminance = (float)(((0x00000001 << exponent) * (float)mantissa) * 0.045);
  
	return luminance; 
}
*/