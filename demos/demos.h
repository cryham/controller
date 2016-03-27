#pragma once
#include <stdint.h>
#include <Adafruit_SSD1306.h>

#define W 128  // area
#define H 64

struct Demos
{
	#define D  Adafruit_SSD1306& display
	//  main  ----
	Demos();
	void Init(), Reset(D);
	void Draw(D, int8_t menu, int8_t ym, int8_t ym2);
	void KeyPress(int8_t demo, int8_t ym, int8_t ym2);

	int8_t iPrev;

	int8_t fps;  // show frames per second, 1 on, 0 off
	uint32_t ti, oti;  // fps: time us, old


	//  sin table  ----
	#define SX 8192   // mem size 16kB
	#define SY 16384  // y quality
	#define Cos(x)  sint[(SX/2+(x))%SX]
	#define Sin(x)  sint[      (x) %SX]

	uint t;  // frame counter
	int16_t sint[SX];
	void SinInit();

	
	//  Plasma
	int8_t tadd[5];  // speed
	void Plasma0(uint8_t* buf), Plasma1(uint8_t* buf);
	void Plasma2(uint8_t* buf), Plasma3(uint8_t* buf), Plasma4(uint8_t* buf);


	//  Balls  --------
	const static int sMax = 80, bMax = 150, dMax = 150;  //2550
	int sCnt;  // stars: count
	int bCnt, bSpd, bRad;  // balls: count, speed, radius max
	#define bDet 256  // move detail

	enum EInit {  INone=0, IBalls, ISpace, IDrops  } einit;
	struct Star {  int x,y,z, v;  };
	struct Ball {  int16_t x,y, vx,vy, r;  };
	struct Drop {  int16_t x,y, vx,vy, t;  };
	union
	{	//  common data
		Star star[sMax];
		Ball ball[bMax];
		Drop drop[dMax];
	};
	void BallsInit();
	void Balls(D);

	void Rain(D), Rain2(D);
	
	void Chars(D, uint8_t set=0);  // text
	void Font_ver(D);  // Logo
	void CK_logo(D);

	void SpaceInit();  // Space
	void Star(int i);  // new
	void Space(D);


	//  fountain, drops  ----
	int tf;
	void FountainInit();
	void Fountain(D);


	//  Ngons 2D
	int8_t ngCur;  int16_t ngt;
	const static int8_t ngMin = 5, ngMax = 14;
	void Ngons(D);

	//  Polyhedrons 3D  ----
	const static int8_t hdA = 11, hdRotMax = 4;  // all presets
	const static int16_t hdtMax = 400;  // cycle time

	int16_t hdt;  int8_t hdCur,  hdtOn, hdRot;
	void Hedrons(D);
	
	//void Spiral..
	#undef D
};
