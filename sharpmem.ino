

#include <Adafruit_GFX.h>
#include "SharpMemSTM32.h"
#include <SPI.h>

// any pins can be used
#define SCK  13
#define MOSI 11
#define SS   10

SharpMem display(SCK, MOSI, SS);

#define BLACK 0
#define WHITE 1



class Ball {
public:


Ball() {
	speedx = speedy = 2;
};
Ball(int x1, int y1) {
	x = x1;
	y = y1;
	speedx = 2;
	speedy = 2;
}

void update() {
	if (x >= 380) speedx = -2;
	if (x <= 10) speedx = +2;
	x += speedx;
	if (y > 225) speedy = -1.8 * abs(speedy);
	if (y <  10) speedy = +1;
	y += speedy;

	speedy += 0.1f * r;
	if (speedy > 3) speedy = 3;
}

int speedx;
float speedy;
int x;
int y;
int r;
};

Ball balls[30];


void setup(void) 
{
//  delay(5000);
  Serial.begin(9600);
  Serial.println("Hello!");


  // start & clear the display
  display.begin();
 // display.clearDisplay();

  
  for (int i = 0; i < 30; i++) {
	  balls[i].x = rand() % 380 + 10;
	  balls[i].y = rand() % 220 + 10;
	  balls[i].r = 1 + rand() % 5;
  }


}



uint8_t size = 1;
uint16_t tx = 0;
uint8_t p = 0;
long tm1;
void loop(void) 
{

	tx++;
	tx %= 380;
	uint16_t fps;
	fps = 1000 / (millis() - tm1);
	tm1 = millis();

	display.clearBuffer();
	display.setTextColor(BLACK);
	display.setCursor(tx, 10);
	display.setTextSize(4);
	display.print("TEST!!!");
	display.setTextSize(3);
	display.setCursor(10, 40);
	display.print("FPS:");
	display.print(fps);
	


	for (int i = 0; i < 30; i++) {
		balls[i].update();
		display.fillCircle(balls[i].x, balls[i].y, balls[i].r, BLACK);
	}



	
	display.refresh();
	



}
