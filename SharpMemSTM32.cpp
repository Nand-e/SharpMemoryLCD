
#include <Arduino.h>
#include "SharpMemSTM32.h"

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif
#ifndef _swap_uint16_t
#define _swap_uint16_t(a, b) { uint16_t t = a; a = b; b = t; }
#endif

#include <libmaple/dma.h>
#include <SPI.h>

#define SHARPMEM_BIT_WRITECMD   (0x80)
#define SHARPMEM_BIT_VCOM       (0x40)
#define SHARPMEM_BIT_CLEAR      (0x20)
#define TOGGLE_VCOM             do { _sharpmem_vcom = _sharpmem_vcom ? 0x00 : SHARPMEM_BIT_VCOM; } while(0);

byte sharpmem_buffer[(SHARPMEM_LCDWIDTH * SHARPMEM_LCDHEIGHT) / 8];

/* ************* */
/* CONSTRUCTORS  */
/* ************* */
SharpMem::SharpMem(uint8_t clk, uint8_t mosi, uint8_t ss) :
	Adafruit_GFX(SHARPMEM_LCDWIDTH, SHARPMEM_LCDHEIGHT) {
	_clk = clk;
	_mosi = mosi;
	_ss = ss;

	// Set pin state before direction to make sure they start this way (no glitching)
	digitalWrite(_ss, HIGH);
	digitalWrite(_clk, LOW);
	digitalWrite(_mosi, HIGH);

	pinMode(_ss, OUTPUT);
	pinMode(_clk, OUTPUT);
	pinMode(_mosi, OUTPUT);

	clkport = portOutputRegister(digitalPinToPort(_clk));
	clkpinmask = digitalPinToBitMask(_clk);
	dataport = portOutputRegister(digitalPinToPort(_mosi));
	datapinmask = digitalPinToBitMask(_mosi);

	// Set the vcom bit to a defined state
	_sharpmem_vcom = SHARPMEM_BIT_VCOM;


}

void SharpMem::begin() {
	SPI.begin();
	SPI.setClockDivider(SPI_CLOCK_DIV8);
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);
	SPI.setDataSize(SPI_CR1_DFF_8_BIT);
	Serial.println("LCD begin!");
}


/**************************************************************************/
void SharpMem::sendbyte(uint8_t data)
{
	uint8_t i = 0;
	for (i = 0; i<8; i++)
	{

		*clkport &= ~clkpinmask;
		if (data & 0x80) *dataport |= datapinmask;
		else			 *dataport &= ~datapinmask;	
		*clkport |= clkpinmask;
		data <<= 1;
	}
	*clkport &= ~clkpinmask;
}

void SharpMem::sendbyteLSB(uint8_t data)
{
	uint8_t i = 0;	
	for (i = 0; i<8; i++)
	{
	
		*clkport &= ~clkpinmask;
		if (data & 0x01) *dataport |= datapinmask;
		else     		 *dataport &= ~datapinmask;
		*clkport |= clkpinmask;
		data >>= 1;
	}
	*clkport &= ~clkpinmask;
}


/**************************************************************************/
void SharpMem::drawPixel(int16_t x, int16_t y, uint16_t color)
{
	if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;	
	if (color) {
		sharpmem_buffer[(y*SHARPMEM_LCDWIDTH + x) / 8] |= 128 >> (x & 7);		
	}
	else {
		sharpmem_buffer[(y*SHARPMEM_LCDWIDTH + x) / 8] &= ~( 128 >>  (x & 7));		
	}
}

/**************************************************************************/
uint8_t SharpMem::getPixel(uint16_t x, uint16_t y)
{
	if ((x >= _width) || (y >= _height)) return 0; // <0 test not needed, unsigned
	
	return 1;
}

void SharpMem::clearBuffer() {
	memset(sharpmem_buffer, 0xff, (SHARPMEM_LCDWIDTH * SHARPMEM_LCDHEIGHT) / 8);
}

/**************************************************************************/
void SharpMem::clearDisplay()
{
	memset(sharpmem_buffer, 0xff, (SHARPMEM_LCDWIDTH * SHARPMEM_LCDHEIGHT) / 8);
	// Send the clear screen command rather than doing a HW refresh (quicker)
	digitalWrite(_ss, HIGH);
	sendbyte(_sharpmem_vcom | SHARPMEM_BIT_CLEAR);
	sendbyteLSB(0x00);
	TOGGLE_VCOM;
	digitalWrite(_ss, LOW);
}


/**************************************************************************/
void SharpMem::refresh(void)
{
	static uint8_t start = 0;
	digitalWrite(_ss, HIGH);
	delayMicroseconds(10);	
	TOGGLE_VCOM;
	uint8_t c = SHARPMEM_BIT_WRITECMD | _sharpmem_vcom;
	SPI.transfer(c);

	uint8_t * pBuffer = &sharpmem_buffer[start*50];	
	for (uint16_t y = start; y< start+60; y++)
	{
		SPI.transfer(reverseBitOrder(y));
		SPI.write(pBuffer, 50);
		pBuffer += 50;		
		SPI.transfer(0x00);
	}
	start += 60;
	if (start > 200) start = 0;
	SPI.transfer(0x00);  // trailing padding chars
	delayMicroseconds(10);
	digitalWrite(_ss, LOW);

}

const uint8_t SharpMem::reverseBitOrder(const uint8_t b) {
	uint8_t temp = b;
	temp = (temp & 0xF0) >> 4 | (temp & 0x0F) << 4;
	temp = (temp & 0xCC) >> 2 | (temp & 0x33) << 2;
	temp = (temp & 0xAA) >> 1 | (temp & 0x55) << 1;
	return temp;
}
