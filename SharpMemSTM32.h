







#if ARDUINO >= 100
#include "Arduino.h"
#include <Adafruit_GFX.h>
#endif

// LCD Dimensions
#define SHARPMEM_LCDWIDTH   ( 400 )
#define SHARPMEM_LCDHEIGHT  ( 240 )

class SharpMem : public Adafruit_GFX {
public:
	SharpMem(uint8_t clk, uint8_t mosi, uint8_t ss);
	void begin(void);
	void drawPixel(int16_t x, int16_t y, uint16_t color);
	uint8_t getPixel(uint16_t x, uint16_t y);
	void clearDisplay();
	void refresh(void);
	void clearBuffer();

private:
	uint8_t _ss, _clk, _mosi;

	volatile uint32_t *dataport, *clkport;
	uint32_t _sharpmem_vcom, datapinmask, clkpinmask;

	inline void sendbyte(uint8_t data);
	inline void sendbyteLSB(uint8_t data);
	const uint8_t reverseBitOrder(const uint8_t b);

};
