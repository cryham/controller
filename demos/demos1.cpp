#include "demos.h"
#include "gui.h"  // menu enum, ym etc
#define PROGMEM
#include "FreeSans9pt7b.h"
//#include "FreeSans12pt7b.h"


//  regular polygons with diagonals
//....................................................................................
void Demos::Ngons(Adafruit_SSD1306& d)
{
	#define ngtM 200
	float a=0, b=0, e = 2*PI / ngCur;  // angles
	float tm = float(ngt)/ngtM;
	float c, s;

	switch (ngRot)
	{
	case 0: c = -e*2 * tm;  s = 0.65 + 0.75 * tm;  break;
	case 1: c = -e * tm;  s = 1.0;  break;
	case 2: c = 0.0;  s = 0.8 + 0.6 * tm;  break;
	case 3: c = 0.0;  s = 1.0;  break;
	}
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
		if (ngtOn)	{	++ngCur;  if (ngCur > ngMax)  ngCur = ngMin;  }  }

	if (iInfo > 0)
	{
		d.setCursor(0,8);
		d.print("An ");  d.println(ngRot);

		d.setCursor(0, H-8);
		d.print(ngCur);  // n sides
	}

	delay(ngMax - ngCur + 1);
}


//  Rain circles
//....................................................................................
void Demos::Rain(Adafruit_SSD1306& d)
{
	int x,y,r,i, s = rCur ? r2Size : r1Size;
	if (!rCur)  // 0
	{		x=random(W); y=random(H);  r= random(26);  d.drawCircle(x,y,r,BLACK);
			x=random(W); y=random(H);  r= random(24);  d.drawCircle(x,y,r,BLACK);
			x=random(W); y=random(H);  r= random(20);  d.drawCircle(x,y,r,BLACK);

		if (random(100) < 30 + r1Int * 5)
		{	x=random(W); y=random(H);  r= random(6+s)+1+s;  d.drawCircle(x,y,r,WHITE);
		}
	}else  // 1
	{
		for (i=0; i < 12 + r2Int; ++i)
		{
			if (random(400) < 10) {  x=random(W); y=random(H);  r= random(28)+20;  d.drawCircle(x,y,r,BLACK);  }
			if (random(200) < 10) {  x=random(W); y=random(H);  r= random(20)+16;  d.drawCircle(x,y,r,BLACK);  }
			if (random(100) < 20) {  x=random(W); y=random(H);  r= random(14)+14;  d.drawCircle(x,y,r,BLACK);  }
			if (random(10)  < 3)  {  x=random(W); y=random(H);  r= random(10)+1;   d.drawCircle(x,y,r,BLACK);  }
		}
		for (i=0; i < 2 + r2Int; ++i)  //if (random(10) < 8)
		{
			if (random(600) < 30) {  x=random(W); y=random(H);  r= s+random(6+s)+3;  d.drawCircle(x,y,r,WHITE);  }
			if (random(300) < 40) {  x=random(W); y=random(H);  r= s+random(5+s)+4;  d.drawCircle(x,y,r,WHITE);  }
			if (random(100) < 50) {  x=random(W); y=random(H);  r= s+random(4)+2;  d.drawCircle(x,y,r,WHITE);  }
									 x=random(W); y=random(H);  r= random(3)+1;  d.drawCircle(x,y,r,WHITE);
	}	}
	if (iInfo > 0)
	{
		d.setCursor(0,8);
		d.print("Int ");  d.println(rCur ? r2Int : r1Int);
		d.print("Size ");  d.println(s);
		d.print("Cur ");  d.println(rCur);
	}
	delay(8);
}


//  text fonts
//....................................................................................
void Demos::Font_ver(Adafruit_SSD1306& d)
{
	d.setCursor(0,10);  // logo
	d.setFont(&FreeSans9pt7b/*FreeSans12pt7b*/);  d.print("CrystaL");
	d.setCursor(36,32);
	d.setFont(&FreeSans9pt7b);  d.print("Keyboard");
	
	d.setCursor(96,0);  // ver
	d.setFont(0);  d.print("3.15");
	
	d.setCursor(0*24,H-8);
	const char* a={__DATE__}, *m={__TIME__};
	const char dt[] = {
		//  build date, time  format yyyy-mmm-dd hh:mm
		a[7],a[8],a[9],a[10],' ',a[0],a[1],a[2],' ',a[4],a[5],' ',' ',m[0],m[1],':',m[3],m[4],0};
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
	int v1 = bSpd * (bSpRMax - bSpRnd) / bSpRMax,
		v0 = bSpd * bSpRnd / bSpRMax;

	for (int i=0; i < bCnt; i++)
	{	Ball& b = ball[i];    b.r = random(bRad);
		b.x = (random(W - b.r*2) + b.r) *bDet;
		b.y = (random(H - b.r*2) + b.r) *bDet;
		b.vx = random(v1) + v0;
		b.vy = random(v1) + v0;
		if (random(2))  b.vx = -b.vx;
		if (random(2))  b.vy = -b.vy;
	}
	einit = IBalls;
}

void Demos::Balls(Adafruit_SSD1306& d)
{
	if (einit != IBalls)
		BallsInit();
	
	for (int i=0; i < bCnt; i++)
	{	Ball& b = ball[i];
		if (b.x < b.r *bDet || b.x >= (W-1-b.r) *bDet)  b.vx = -b.vx;
		if (b.y < b.r *bDet || b.y >= (H-1-b.r) *bDet)  b.vy = -b.vy;
		b.x += b.vx;  b.y += b.vy;

		if (b.r)
			d.drawCircle(b.x/bDet, b.y/bDet, b.r, WHITE);
		else
			d.drawPixel(b.x/bDet, b.y/bDet, WHITE);
	}

	if (iInfo > 0)
	{
		d.setCursor(0,8);
		d.print("Cnt ");  d.println(bCnt);
		d.print("Spd ");  d.println(bSpd);
		d.print("Rnd ");  d.println(bSpRnd);
		d.print("Rad ");  d.println(bRad);
	}
	delay(6);
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
	sNear = sDet*200, sFar = sDet*750;  // range: cut off, new appear

void Demos::Star(int i)
{
	star[i].x = (random(W*sRng) -W*sRng/2) * sDet;
	star[i].y = (random(H*sRng) -H*sRng/2) * sDet;
	star[i].z = random(sFar) + sNear;   // depth
	star[i].v = random(sDet*sVel/2) + sDet*sVel/4;  // speed
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
	if (iInfo > 0)
	{
		d.setCursor(0,8);
		d.print("Cnt ");  d.println(sCnt);
		d.print("Vel ");  d.println(sVel);
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

			if (x < 0 || x >= W || //y < 0 ||
				y >= H)  // outside
			{	o.t = -1;  }
			else
				d.drawPixel(x,y, WHITE);
		}
		else  // spawn new
		if (nn < fInt)  // max 9
		{	++nn;
			o.t = random(160) + 20 + fInt * 8;  // time to live
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
	}

	if (iInfo > 0)
	{
		d.setCursor(0,8);
		d.print("Int ");  d.println(fInt);
		d.print("Spd ");  d.println(fWave);

		d.setCursor(0, H-8);
		d.print(io);  // drops used
	}

	t += fWave;
	delay(6);
}
