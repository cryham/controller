#pragma once
#include <stdint.h>
#include <Adafruit_SSD1306.h>


struct Demos
{
	//  sin table
	#define SX 8192   // mem size
	#define S2 4096   // SX/2
	#define SY 16384  // y quality

	#define Cos(x)  sint[(S2+(x))%SX]
	#define Sin(x)  sint[    (x) %SX]

	uint t;
	int16_t sint[SX];  // sin table

	void SinInit(), Plasma0(uint8_t* buf);
	void Plasma1(uint8_t* buf), Plasma2(uint8_t* buf);
	void Plasma3(uint8_t* buf), Plasma4(uint8_t* buf);

	//  balls
	#define W 128  // area
	#define H 64
	#define sCnt 80   // all* stars
	#define bCnt 40   // all* balls

	#define bSpd 45   // speed*
	#define bRad 3    // radius max
	#define bDet 256  // move detail
	int binit;
	//  [max(sCnt,bCnt)]  used for balls and stars
	int xb[sCnt],yb[sCnt], zb[sCnt],vb[sCnt], rb[bCnt];

	void BallsInit();
	void Balls(Adafruit_SSD1306& display);

	//  simple
	int n, u;
	void Ngons(Adafruit_SSD1306& display);
	void Rain(Adafruit_SSD1306& display);

	void Fonts(Adafruit_SSD1306& display);
	void Chars(Adafruit_SSD1306& display, uint8_t set=0);

	void CK_logo(Adafruit_SSD1306& display);

	//  space
	int sinit;
	void SpaceInit();
	void Star(int i);  // new
	void Space(Adafruit_SSD1306& display);

	//void Cube3D
	//void Fountain

	Demos();
	void Init();
};
