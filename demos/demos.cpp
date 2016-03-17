#include "demos.h"
//#include <math.h>
//#include <arm_math.h>
#define PROGMEM
#include "FreeSans9pt7b.h"
#if 0
#include "FreeSans12pt7b.h"
#include "FreeSans18pt7b.h"
#include "FreeSans24pt7b.h"
#include "FreeSansBold12pt7b.h"
#include "FreeSansBold18pt7b.h"
#include "FreeSansBold24pt7b.h"
#include "FreeSansBold9pt7b.h"
#endif

Demos::Demos()
{
	Init();
}

void Demos::Init()
{
	PlasmaInit();
	BallsInit();
}

//  Balls
// ...........................................................................................................

void Demos::BallsInit()
{
	for (int i=0; i<A; i++)
	{
		rb[i] = random(R);
		xb[i] = random(W)*D;  yb[i] = random(H)*D;
		ab[i] = random(E+1)-E/2;  bb[i] = random(E+1)-E/2;
	}
	n = 3;  u = 0;  // Ngons
}


void Demos::Balls(Adafruit_SSD1306& display)
{
	for (int i=0; i < A; i++)
	{
		if (xb[i] < 0 || xb[i] >= W*D-1)  ab[i] = -ab[i];
		if (yb[i] < 0 || yb[i] >= H*D-1)  bb[i] = -bb[i];
		xb[i] += ab[i];  yb[i] += bb[i];

		if (rb[i]==0)
			display.drawPixel(xb[i]/D, yb[i]/D, WHITE);
		else
			display.drawCircle(xb[i]/D, yb[i]/D, rb[i], WHITE);
	}
}

//  lines n-gon x
void Demos::Ngons(Adafruit_SSD1306& display)
{
	#define uu 200
	float a=0, b=0, d = 2*PI/n;
	float c = -PI/2*u/uu, s = 0.65 + 0.75*u/uu;
	float xx = W/2, yy = H/2, sy = s * yy;

	for (b=0; b < 2*PI; b+=d)
	{
		float xb = xx + sy*cos(b+c), yb = yy - sy*sin(b+c);
		for (a=0; a < b && a < 2*PI; a+=d)
		{
			float xa = xx + sy*cos(a+c), ya = yy - sy*sin(a+c);
			if (a != b)
				display.drawLine(xb, yb, xa, ya, 1);
		}
	}
	++u;
	if (u > uu)
	{	u = 0;  ++n;  if (n > 14)  n = 3;  }

	display.setCursor(0, H-1-7);
	display.print(n);  // n
}

//  Rain circles
void Demos::Rain(Adafruit_SSD1306& display)
{
	int x,y,r;
	//for (int i=0; i < 10; ++i)
	{
		x = random(W);  y = random(H);  r = random(36);
		display.drawCircle(x,y,r,BLACK);
		x = random(W);  y = random(H);  r = random(10);
		display.drawCircle(x,y,r,WHITE);
	}	
}


//  text fonts
void Demos::Fonts(Adafruit_SSD1306& display)
{
	display.setCursor(0,16);
	display.setFont(&FreeSans9pt7b);//+
	//display.println("01234567:");
	//display.println("ZXUVQPIK");
	display.println("qwerxpstnug");
	display.println("jkavwriqcmo");
	//display.setFont(&FreeSans12pt7b);//+
	//display.setCursor(30,32);
	//display.println("579");
}

//  text all chars
//  128x64 = 20x8
void Demos::Chars(Adafruit_SSD1306& display, uint8_t set)
{
	display.setCursor(0,8);
	uint8_t cc = set*128;

	for (int i=0; i < 128; i++)
	{	if (i == '\n') continue;
		display.write(cc + i);
		if (i > 0 && (i % 20 == 0))
		  display.println();
	}
}
