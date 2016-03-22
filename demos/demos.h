#pragma once
#include <stdint.h>
#include <Adafruit_SSD1306.h>

struct Demos
{
	//  main  ----
	Demos();
	void Init(), Reset(Adafruit_SSD1306& display);
	void Draw(Adafruit_SSD1306& display);
	void KeyPress();

	int8_t iAllCount, iCurrent;
	void Next(int dir=1);  // 1 next, 0 prev

	int8_t fps;  // 1 on, 0 off
	uint32_t ti, oti;  // fps: time ms, old


	//  sin table  ----
	#define SX 8192   // mem size
	#define SY 16384  // y quality
	#define Cos(x)  sint[(SX/2+(x))%SX]
	#define Sin(x)  sint[      (x) %SX]

	uint t;  // frame counter
	int16_t sint[SX];  // sin table

	void SinInit(), Plasma0(uint8_t* buf);
	void Plasma1(uint8_t* buf), Plasma2(uint8_t* buf);
	void Plasma3(uint8_t* buf), Plasma4(uint8_t* buf);


	//  balls  --------
	const static int sMax=80, bMax=150;
	int sCnt;  // stars count
	int bCnt, bSpd, bRad;  // balls count, speed, radius max
	#define bDet 256  // move detail

	enum EInit {  INone=0, IBalls, ISpace  } einit;
	struct Star {  int x,y,z, v;  };
	struct Ball {  int16_t x,y, vx,vy, r;  };
	union
	{	//  used for both
		Star star[sMax];
		Ball ball[bMax];
	};
	void BallsInit();
	void Balls(Adafruit_SSD1306& display);

	//  simple  ----
	int n, u;  // Ngons vars
	void Ngons(Adafruit_SSD1306& display);
	void Rain(Adafruit_SSD1306& display);

	void Fonts(Adafruit_SSD1306& display);
	void Chars(Adafruit_SSD1306& display, uint8_t set=0);
	void CK_logo(Adafruit_SSD1306& display);

	//  space  ----
	void SpaceInit();
	void Star(int i);  // new
	void Space(Adafruit_SSD1306& display);

	//void Cube3D
	//void Fountain  Spiral
};
