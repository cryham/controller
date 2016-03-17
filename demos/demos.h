#pragma once

#include <stdint.h>
#include <Adafruit_SSD1306.h>


struct Demos
{
	//#define SX 1024
	//#define S2 512
	//#define SY 128

	//  sin table
	#define SX 8192   // mem size
	#define S2 4096   // SX/2
	#define SY 16384  // y quality

	#define Cos(x)  sint[(S2+(x))%SX]
	#define Sin(x)  sint[    (x) %SX]

	int t;
	int16_t sint[SX];  // sin table


	void PlasmaInit();
	void Plasma0(uint8_t* buf/*, uint8_t type*/);
	void Plasma1(uint8_t* buf);
	void Plasma2(uint8_t* buf);
	void Plasma3(uint8_t* buf);
	void Plasma4(uint8_t* buf);


	//  balls
	#define W 128  // area
	#define H 64

	#define A 40  // all
	#define D 256   // move detail
	#define E 80   // speed
	#define R 3   // radius max

	int xb[A], yb[A], ab[A], bb[A], rb[A];

	void BallsInit();
	void Balls(Adafruit_SSD1306& display);


	int n, u;
	void Ngons(Adafruit_SSD1306& display);

	void Rain(Adafruit_SSD1306& display);

	void Fonts(Adafruit_SSD1306& display);
	void Chars(Adafruit_SSD1306& display, uint8_t set=0);

	//void Fountain
	//void Space, Cube3D

	Demos();
	void Init();
};
