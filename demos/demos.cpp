#include "demos.h"
#include "gui.h"  // menu ym etc
#define PROGMEM
#include "FreeSans9pt7b.h"
#include "FreeSans12pt7b.h"
//#include "FreeSans18pt7b.h"
//#include "FreeSansBold9pt7b.h"
extern "C" {
	#include <scan_loop.h>
	#include <output_com.h>
	#include <usb_hid.h>
}

//  Main
//....................................................................................
Demos::Demos()
{
	Init();
}
void Demos::Init()
{
	fps = 0;  //par
	ti = 0;  oti = 0;

	SinInit();
	einit = INone;  iPrev = -1;
}

void Demos::Draw(Adafruit_SSD1306& d, int8_t menu, int8_t y, int8_t y2)
{
	if (menu)
	if (y == MDemos)
	{
		if (iPrev != y2)  //  Init on change
		switch (y2)
		{
			case 0:  sCnt = 80;  // Stars
			case 1:  einit = INone;  bCnt = 40;  bSpd = 45;  bRad = 3;  break;  // Balls
			case 2:  einit = INone;  bCnt = 80;  bSpd = 80;  bRad = 3;  break;
			case 3:  einit = INone;  bCnt = 150; bSpd = 40;  bRad = 2;  break;
			
			case 7:  ngCur = ngMin;  ngt = 0;  // Ngons
		}
		switch (y2)
		{
			case 0: d.clearDisplay();	Space(d);  break;
			case 1:
			case 2:
			case 3: d.clearDisplay();	Balls(d);  break;

			case 4:						Rain(d);  break;
			case 5:						Rain2(d);  break;
			case 6: d.clearDisplay();	Fountain(d);  break;
			
			case 7: d.clearDisplay();	Ngons(d);  break;
			case 8: d.clearDisplay();	Cube(d);  break;
		}
		iPrev = y2;
	}
	else if (y == MText)
	switch (y2)
	{
		case 0: d.clearDisplay();  CK_logo(d);  break;
		case 1: d.clearDisplay();  Font_ver(d);  break;
		case 2: d.clearDisplay();  Chars(d,0);  break;
		case 3: d.clearDisplay();  Chars(d,1);  break;
	}
	else if (y == MPlasma)
	switch (y2)
	{
		case 0: Plasma1(d.getBuffer());  break;
		case 1: Plasma2(d.getBuffer());  break;
		case 2: Plasma3(d.getBuffer());  break;
		case 3: Plasma4(d.getBuffer());  break;
	}

	if (fps)  // fps
	{
		oti = ti;  ti = micros();
		d.setFont(0);
		d.setCursor(0,0);
		d.println(int16_t(1000000.f/(ti-oti)));
	}
}

void Demos::KeyPress()
{
	if (kk[KEY_B] && !kko[KEY_B])
		fps = 1-fps;
}

void Demos::Reset(Adafruit_SSD1306& d)
{
	d.begin(SSD1306_SWITCHCAPVCC);
	d.clearDisplay();
	d.dim(1);
	d.display();
	d.setTextColor(WHITE);
}


//  regular polygons with diagonals
//....................................................................................
void Demos::Ngons(Adafruit_SSD1306& d)
{
	#define ngtM 200
	float a=0, b=0, e = 2*PI / ngCur;  // angles
	float tm = float(ngt)/ngtM;
	float c = -PI/2 * tm, s = 0.65 + 0.75 * tm;
	float xx = W/2, yy = H/2, sy = s * yy;

	for (b=0; b < 2*PI; b+=e)
	{
		float xb = xx + sy*cos(b+c), yb = yy - sy*sin(b+c);
		for (a=0; a < b && a < 2*PI; a+=e)
		{
			float xa = xx + sy*cos(a+c), ya = yy - sy*sin(a+c);
			if (a != b)
				d.drawLine(xb, yb, xa, ya, 1);
		}
	}
	++ngt;
	if (ngt > ngtM) {  ngt = 0;
		++ngCur;  if (ngCur > ngMax)  ngCur = ngMin;  }

	d.setCursor(0, H-8);
	d.print(ngCur);  // n sides
}


//  Rain circles
//....................................................................................
void Demos::Rain(Adafruit_SSD1306& d)
{
	int x,y,r;
		x=random(W); y=random(H);  r= random(26);  d.drawCircle(x,y,r,BLACK);
		x=random(W); y=random(H);  r= random(24);  d.drawCircle(x,y,r,BLACK);
		x=random(W); y=random(H);  r= random(20);  d.drawCircle(x,y,r,BLACK);
	if (random(100) < 30)
	{	x=random(W); y=random(H);  r= random(6)+1;  d.drawCircle(x,y,r,WHITE);  }
	delay(8);
}

void Demos::Rain2(Adafruit_SSD1306& d)
{
	int x,y,r;
	for (int i=0; i<12; ++i)
	{
		if (random(400) < 10) {  x=random(W); y=random(H);  r= random(28)+20;  d.drawCircle(x,y,r,BLACK);  }
		if (random(200) < 10) {  x=random(W); y=random(H);  r= random(20)+16;  d.drawCircle(x,y,r,BLACK);  }
		if (random(100) < 20) {  x=random(W); y=random(H);  r= random(14)+14;  d.drawCircle(x,y,r,BLACK);  }
		if (random(10)  < 3)  {  x=random(W); y=random(H);  r= random(10)+1;   d.drawCircle(x,y,r,BLACK);  }
	}
	for (int i=0; i<2; ++i)  //if (random(10) < 8)
	{
		if (random(600) < 30) {  x=random(W); y=random(H);  r= random(6)+3;  d.drawCircle(x,y,r,WHITE);  }
		if (random(300) < 40) {  x=random(W); y=random(H);  r= random(5)+4;  d.drawCircle(x,y,r,WHITE);  }
		if (random(100) < 50) {  x=random(W); y=random(H);  r= random(4)+2;  d.drawCircle(x,y,r,WHITE);  }
								 x=random(W); y=random(H);  r= random(3)+1;  d.drawCircle(x,y,r,WHITE);
	}	delay(8);
}


//  text fonts
//....................................................................................
void Demos::Font_ver(Adafruit_SSD1306& d)
{
	d.setCursor(0,10);  // logo
	d.setFont(&FreeSans12pt7b);  d.print("CrystaL");
	d.setCursor(36,32);
	d.setFont(&FreeSans9pt7b);  d.print("Keyboard");
	d.setCursor(96,0);  // ver
	d.setFont(0);  d.print("3.05");
	d.setCursor(0*24,H-8);
	const char* a={__DATE__}, *m={__TIME__};
	const char dt[] = {
		//  build date, time  format yyyy-mmm-dd hh:mm
		a[7],a[8],a[9],a[10],' ',a[0],a[1],a[2],' ',a[4],a[5],' ',m[0],m[1],':',m[3],m[4],0};
	d.print(dt);
}

//  text all chars   128x64 = 20x8
void Demos::Chars(Adafruit_SSD1306& d, uint8_t set)
{
	d.setCursor(0,8);
	uint8_t cc = set*128;

	for (int i=0; i < 128; i++)
	{	if (i == '\n') continue;
		d.write(cc + i);
		if (i > 0 && (i % 20 == 0))
		  d.println();
	}
}


//  Balls
//....................................................................................
void Demos::BallsInit()
{
	for (int i=0; i<bCnt; i++)
	{	Ball& b = ball[i];     b.r = random(bRad);
		b.x = random(W)*bDet;  b.vx = random(bSpd+1)-bSpd/2;
		b.y = random(H)*bDet;  b.vy = random(bSpd+1)-bSpd/2;
	}
	einit = IBalls;
}

void Demos::Balls(Adafruit_SSD1306& d)
{
	if (einit != IBalls)
		BallsInit();
	
	for (int i=0; i < bCnt; i++)
	{	Ball& b = ball[i];
		if (b.x < 0 || b.x >= W*bDet-1)  b.vx = -b.vx;
		if (b.y < 0 || b.y >= H*bDet-1)  b.vy = -b.vy;
		b.x += b.vx;  b.y += b.vy;

		if (b.r==0)
			d.drawPixel(b.x/bDet, b.y/bDet, WHITE);
		else
			d.drawCircle(b.x/bDet, b.y/bDet, b.r, WHITE);
	}
}


//  Space
//....................................................................................
void Demos::SpaceInit()
{
	for (int i=0; i<sCnt; i++)
		Star(i);
	einit = ISpace;
}
#define sRng 3000  // init range
#define sDet 4  // detail

const static int
	sSpdMin = 2*sDet, sSpdMax=4*sDet,   // speed min,max
	sNear = sDet*200, sFar = sDet*750;  // range: cut off, new appear

void Demos::Star(int i)
{
	star[i].x = (random(W*sRng) -W*sRng/2) * sDet;
	star[i].y = (random(H*sRng) -H*sRng/2) * sDet;
	star[i].z = random(sFar) + sNear;   // depth
	star[i].v = random(sSpdMax) + sSpdMin;  // speed
}
void Demos::Space(Adafruit_SSD1306& d)
{
	if (einit != ISpace)
		SpaceInit();
	
	for (int i=0; i < sCnt; i++)
	{
		int z = star[i].z + 1*sDet;
		int x = star[i].x/z / sDet +W/2;  // pos 3d to 2d
		int y = star[i].y/z / sDet +H/2;
		d.drawPixel(x,y, WHITE);
			
		star[i].z -= star[i].v;
		if (x < 0 || x > W ||
			y < 0 || y > H ||  // outside
			star[i].z < sNear)  // too close
		{
			Star(i);  star[i].z = sNear+sFar;
		}
	}
	delay(8);  // ms - limit to 100 fps
}


//  Fountain
//....................................................................................
void Demos::FountainInit()
{
	for (int i=0; i < dMax; i++)
		drop[i].t = -1;
	einit = IDrops;
}

void Demos::Fountain(Adafruit_SSD1306& d)
{
	if (einit != IDrops)
		FountainInit();
	
	int nn = 0;  int io = 0;
	for (int i=0; i < dMax; ++i)
	{	Drop& o = drop[i];

		if (o.t >= 0)  // alive
		{	--o.t;  ++io;

			o.x += o.vx;  // vel
			o.y += o.vy;
			o.vy += 4;  // gravity

			int x = o.x / bDet;  // pos
			int y = o.y / bDet;

			if (x < 0 || x > W || //y < 0 ||
				y > H)  // outside
			{	o.t = -1;  }
			else
				d.drawPixel(x,y, WHITE);
		}
		else  // spawn new
		if (nn < 1)  // max 9
		{	++nn;
			o.t = random(160)+20;  // time to live
			#define dVx  150
			o.x = 62*bDet + random(4*bDet);
			o.y = 63*bDet - random(4*bDet);
			//  vel up, with wave
			int rx = 120 + 110 * Sin(t*61)/SY * Cos(t*44)/SY,
				px =       100 * Sin(t*42)/SY * Cos(t*24)/SY,
				py =   40 + 40 * Sin(t*53)/SY * Cos(t*39)/SY;
			o.vx = random(rx+1)-rx/2 + px;
			o.vy = -random(70) - 320 + py;
		}
	}	++t;
	delay(6);
	//d.setCursor(0, 0);
	//d.print(io);  // drops used
}
