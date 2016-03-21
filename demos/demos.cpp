#include "demos.h"
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
// ....................................................................................
Demos::Demos()
{
	Init();
}
void Demos::Init()
{
	fps = 0;  //
	iCurrent = 0;
	iAllCount = 12;  //par
	ti = 0;  oti = 0;

	SinInit();
	binit = 0;  sinit = 0;
	n = 3;  u = 0;  // Ngons
}

void Demos::Draw(Adafruit_SSD1306& d)
{
	switch (iCurrent)
	{
		case 0: d.clearDisplay();  break;
		case 1: d.clearDisplay();  CK_logo(d);  break;
		case 2: d.clearDisplay();  Space(d);  break;
		case 3: d.clearDisplay();  Balls(d);  break;
		case 4:						Rain(d);  break;
		case 5: d.clearDisplay();  Ngons(d);  break;
		case 6: d.clearDisplay();  Chars(d,0);  break;
		case 7: d.clearDisplay();  Chars(d,1);  break;
		case 8: d.clearDisplay();  Fonts(d);  break;
		case 9: Plasma1(d.getBuffer());  break;
		case 10: Plasma2(d.getBuffer());  break;
		case 11: Plasma3(d.getBuffer());  break;
		case 12: Plasma4(d.getBuffer());  break;
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
	if (kk[KEY_RIGHT] && !kko[KEY_RIGHT])
		Next(1);
	if (kk[KEY_LEFT] && !kko[KEY_LEFT])
		Next(0);
	
	if (kk[KEY_DOWN] && !kko[KEY_DOWN])
	{	fps = 1-fps;  }
}

void Demos::Reset(Adafruit_SSD1306& d)
{
	d.begin(SSD1306_SWITCHCAPVCC);
	d.clearDisplay();
	d.dim(1);
	d.display();
	d.setTextColor(WHITE);
}

void Demos::Next(int dir)
{
	if (dir > 0)  // next
	{	iCurrent++;  if (iCurrent > iAllCount)  iCurrent = 0;  }
	else  // prev
	{	iCurrent--;  if (iCurrent < 0)  iCurrent = iAllCount;  }
}


//  Balls
// ....................................................................................

void Demos::BallsInit()
{
	for (int i=0; i<bCnt; i++)
	{
		rb[i] = random(bRad);
		xb[i] = random(W)*bDet;  yb[i] = random(H)*bDet;
		vb[i] = random(bSpd+1)-bSpd/2;  zb[i] = random(bSpd+1)-bSpd/2;
	}
	binit = 1;  sinit = 0;
}


void Demos::Balls(Adafruit_SSD1306& d)
{
	if (!binit)
		BallsInit();
	
	for (int i=0; i < bCnt; i++)
	{
		if (xb[i] < 0 || xb[i] >= W*bDet-1)  vb[i] = -vb[i];
		if (yb[i] < 0 || yb[i] >= H*bDet-1)  zb[i] = -zb[i];
		xb[i] += vb[i];  yb[i] += zb[i];

		if (rb[i]==0)
			d.drawPixel(xb[i]/bDet, yb[i]/bDet, WHITE);
		else
			d.drawCircle(xb[i]/bDet, yb[i]/bDet, rb[i], WHITE);
	}
}

//  lines n-gon x
void Demos::Ngons(Adafruit_SSD1306& d)
{
	#define uu 200
	float a=0, b=0, e = 2*PI/n;
	float c = -PI/2*u/uu, s = 0.65 + 0.75*u/uu;
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
	++u;
	if (u > uu)
	{	u = 0;  ++n;  if (n > 14)  n = 3;  }

	d.setCursor(0, H-8);
	d.print(n);  // n
}

//  Rain circles
void Demos::Rain(Adafruit_SSD1306& d)
{
	int x,y,r;
	//for (int i=0; i < 10; ++i)
	{
		x = random(W);  y = random(H);  r = random(26);
		d.drawCircle(x,y,r,BLACK);
		x = random(W);  y = random(H);  r = random(36);
		d.drawCircle(x,y,r,BLACK);
		x = random(W);  y = random(H);  r = random(10);
		d.drawCircle(x,y,r,WHITE);
	}
	delay(8);
}


//  text fonts
// ....................................................................................
void Demos::Fonts(Adafruit_SSD1306& d)
{
	d.setCursor(0,11);
	d.setFont(&FreeSans12pt7b);
	d.print("CrystaL");
	d.setCursor(36,34);
	d.setFont(&FreeSans9pt7b);
	d.print("Keyboard");
	d.setFont(0);
	d.setCursor(96,0);
	d.print("3.01");
	d.setCursor(0*24,H-8);
	const char* a={__DATE__}, *m={__TIME__};
	const char dt[] = {
		a[4],a[5],' ',a[0],a[1],a[2],' ',a[7],a[8],a[9],a[10],' ',m[0],m[1],':',m[3],m[4],0};
	d.print(dt);
}

//  text all chars
//  128x64 = 20x8
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

//  CrystaL KeyboarD  logo lines
// ....................................................................................
//  x,y points, 0 break
const static int word1[] = {
/*C*/194,43, 120,133, 8,193, 112,233, 224,299, 0,
	224,299, 141,227, 78,194, 144,139, 194,43, 0,
/*R*/192,110, 260,156, 211,176, 164,201, 220,153, 192,110, 0,
	164,201, 248,222, 314,258, 298,276, 251,237, 164,201, 0,
/*Y*/303,189, 388,224 , 450,190, 398,256 , 331,328, 374,242, 303,189, 0,
/*S*/541,92, 439,152, 533,196, 435,267, 507,226, 574,197, 517,173, 473,147, 541,92, 0,
/*T*/544,62, 628,61, 692,33, 629,75, 592,167, 615,76, 544,62, 0,
/*A*/623,216, 660,154, 673,82, 702,152, 746,206, 679,144, 623,216, 0,
/*L*/766,28, 793,147, 781,233, 738,300, 818,256,
	935,220, 815,229, 800,133, 766,28, 0, -100};
const static int word2[] = {
/*K*/217,345, 152,396, 62,440, 176,468, 288,507, 248,481, 117,438, 173,397, 217,345, 0,
	76,367, 118,471, 130,541, 145,516, 76,367, 0,
/*e*/329,359, 282,404, 207,434, 292,471, 368,517, 306,467, 256,438,
	318,433, 257,429, 297,401, 329,359, 0,
/*y*/357,450, 386,488, 424,507, 387,541, 334,561, 373,568, 427,521,
	468,453, 431,497, 357,450, 0,
/*b*/466,369, 492,399, 523,412, 486,435, 540,457, 505,476, 474,517,
	506,459, 486,435, 494,414, 466,369, 0,
/*o*/533,432, 557,414, 571,382, 587,409, 609,426, 591,452, 576,496, 559,455, 533,432, 0,
	573,409, 595,426, 575,461, 552,433, 573,409, 0,
/*a*/583,535, 608,493, 628,444, 649,477, 675,506, 630,481, 583,535, 0,
/*r*/660,385, 694,407, 651,427, 727,471, 706,479, 682,450,
	651,427, 670,407, 660,385, 0,
/*D*/717,363, 796,400, 861,432, 805,479, 761,504, 726,543,
	749,501, 795,445, 776,411, 717,363, 0, -100};

const static int
	cxw[2] = {496,402}, cyw[2] = {130,160},  // centers
	axw[2] = {222,262}, ayw[2] = {362,282};  // amplitudes
	
void Demos::CK_logo(Adafruit_SSD1306& d)
{
	#define K 1024  // wave																		// scale ofs
	#define CX(x) {  x = w-cx;  x = ( (x*(K +ax*Cos(8*w      +tt[0])/SY*Sin(7*w      +tt[1])/SY) /K) +cx)/8 +6;  }
	#define CY(y) {  y = w-cy;  y = ( (y*(K +ay*Cos(9*w+ x*73+tt[2])/SY*Sin(6*w+ x*52+tt[3])/SY) /K) +cy)/9 -1;  }

	const uint tt[4] = {t*7,t*5,t*8,t*5};
	for (int w2=0; w2<2; ++w2)
	{
		const int cx = cxw[w2], cy = cyw[w2];
		const int ax = axw[w2], ay = ayw[w2];
		int a=0,w, i=0,rst=1,
			x1=0,y1=0,x=0,y=0;
		do
		{	w = w2 ? word2[a++] : word1[a++];
			if (w<=0) {  rst=1;  i=0;  }
			else
			if (rst)  switch(i)
			{	case 0:  CX(x)  ++i;  break;
				case 1:  CY(y)  rst=0; i=0;  break;  }
			else  switch(i)
			{	case 0:  x1=x;  CX(x)  ++i;  break;
				case 1:  y1=y;  CY(y)  i=2;  break;  }

			if (i==2)
			{	i=0;  d.drawLine(x1,y1, x,y, WHITE);  }
		}
		while (w >= 0);
	}
	++t;
}


//  Space
// ....................................................................................
void Demos::SpaceInit()
{
	for (int i=0; i<sCnt; i++)
		Star(i);
	sinit = 1;  binit = 0;
}
#define sRng 3000  // range
#define sDet 4  // detail
const static int
	sSpdMin = 2*sDet, sSpdMax=4*sDet,  // speed min,max
	sNear = sDet*200, sFar = sDet*750;  // range: cut off, new appear

void Demos::Star(int i)
{
	xb[i] = (random(W*sRng) -W*sRng/2) * sDet;
	yb[i] = (random(H*sRng) -H*sRng/2) * sDet;
	zb[i] = random(sFar) + sNear;   // depth
	vb[i] = random(sSpdMax) + sSpdMin;  // speed
}
void Demos::Space(Adafruit_SSD1306& d)
{
	if (!sinit)
		SpaceInit();
	
	for (int i=0; i < sCnt; i++)
	{
		int z = zb[i] + 1*sDet;
		int x = xb[i]/z / sDet +W/2;
		int y = yb[i]/z / sDet +H/2;
		d.drawPixel(x,y, WHITE);
			
		zb[i] -= vb[i];
		if (x < 0 || x > W ||
			y < 0 || y > H ||  // outside
			zb[i] < sNear)  // too close
		{
			Star(i);  zb[i] = sNear+sFar;
		}
	}
	delay(8);  // ms - limit to 100 fps
}
