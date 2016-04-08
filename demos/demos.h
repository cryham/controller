#pragma once
#include <stdint.h>
#include <Adafruit_SSD1306.h>

#define DEMOS  // if not, only keyboard Gui

#define W SSD1306_LCDWIDTH  // area
#define H SSD1306_LCDHEIGHT

struct Demos
{
	#define D  Adafruit_SSD1306& display
	//  main  ----
	Demos();
	void Init(int sin=1), Reset(D), KeyGlob(D);
	void Draw(D, int8_t menu, int8_t ym, int8_t ym2);

	int8_t dim;  // dim display toggle
	int8_t fps;  // show frames per second, 1 on, 0 off
	uint32_t ti, oti;  // fps: time us, old

#ifdef DEMOS
	int8_t iPrev;   // prev demo, for init
	int16_t iInfo;
	int8_t iInfoOff;  // params info text
	const static int8_t iOff = 2;
	void KeyPress(int8_t demo, int8_t ym, int8_t ym2);


	//  sin table  ----
	#define SX  8192   // mem size 16kB
	#define SY  16384  // y quality
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
	//const static int sMax = 80, bMax = 150, dMax = 150;
	const static int sMax = 240, bMax = 300, dMax = 950;  //2550
	int sCnt, sVel;  // stars: count, velocity
	int bCnt, bSpd, bSpRnd, bRad;  // balls: count, speed, radius max
	const static int bSpRMax = 8;
	#define bDet  256  // move detail

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


	//  Rain
	int8_t rCur, r1Int,r1Size, r2Int,r2Size;
	void Rain(D);
	
	//  text
	void Chars(D, uint8_t set=0);
	void Font_ver(D);  // ver, date
	
	//  CK logo, 2D lines
	const static int8_t ckMax = 2;
	int8_t ckCur, ckSpeed;
	void CK_logo(D);


	//  Space
	void SpaceInit();
	void Star(int i);  // new
	void Space(D);

	//  Fountain, drops  ----
	int fInt, fWave;
	void FountainInit();
	void Fountain(D);


	//  Ngons 2D
	int16_t ngt;  int8_t ngCur,  ngtOn, ngRot;
	const static int8_t ngMin = 5, ngMax = 14, ngRotMax = 4;
	void Ngons(D);

	//  Polyhedrons 3D  ----
	const static int8_t hdA = 11, hdRotMax = 4;  // all presets
	const static int16_t hdtMax = 400;  // cycle time

	int16_t hdt;  int8_t hdCur,  hdtOn, hdRot;
	void Hedrons(D);
	
	//void Spiral..
#endif
	#undef D
};
