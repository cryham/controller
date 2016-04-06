#include "demos.h"
#include <math.h>
#include <arm_math.h>


//  Plasma
//...................................................

void Demos::SinInit()
{
	t = 1210;
	tadd[0]=2; tadd[1]=1;  tadd[2]=2; tadd[3]=5; tadd[4]=5;

	for (int i=0; i < SX; ++i)
		sint[i] = sin(2.f*PI * i/SX) * (SY-1);
}

const static int dither[8][8] = {
{ 0, 32,  8, 40,  2, 34, 10, 42}, // Bayer ordered dithering pattern 8x8.
{48, 16, 56, 24, 50, 18, 58, 26}, // Input pixel c in 0..63 range, output 0,1
{12, 44,  4, 36, 14, 46,  6, 38},
{60, 28, 52, 20, 62, 30, 54, 22},
{ 3, 35, 11, 43,  1, 33,  9, 41},
{51, 19, 59, 27, 49, 17, 57, 25},
{15, 47,  7, 39, 13, 45,  5, 37},
{63, 31, 55, 23, 61, 29, 53, 21} };

#define dith(x,y,c)  (c <= dither[x][y])


void Demos::Plasma0(uint8_t* buf)
{
	uint y0 = 0;
	uint yy=0,y8=0;

	for (uint y=0; y<SSD1306_LCDHEIGHT; ++y)
	{
		y0 = 1-y0;  uint a = (y/8)*SSD1306_LCDWIDTH;
		uint8_t b1 = (1 << (y&7));  uint8_t b0 = ~(1 << (y&7));
		uint x0 = 0;
		uint xx=0,x8=0;

		for (uint x=0; x<SSD1306_LCDWIDTH; ++x, ++a)
		{
			x0 = 1-x0;  int c;
			c = SY*SY - Sin( yy) * Cos( xx +t);
			c *= 6;
			c = abs(c);
			xx+=60;
		
			c /= SY*(SY/12);
			if (dith(x8,y8,c))
				buf[a] |= b1;
			else
				buf[a] &= b0;
			++x8;  if (x8==8) x8=0;
		}
		yy+=60;
		++y8;  if (y8==8) y8=0;
	}	t+=tadd[0];
}

void Demos::Plasma1(uint8_t* buf)
{
	uint y0 = 0, y8=0;
	uint tt[2]={t*14,t*11}, t2[2]={t*7,t*5};
	uint yy[2]={0,0};

	for (uint y=0; y<SSD1306_LCDHEIGHT; ++y)
	{
		y0 = 1-y0;  uint a = (y/8)*SSD1306_LCDWIDTH;
		uint8_t b1 = (1 << (y&7));  uint8_t b0 = ~(1 << (y&7));
		uint x0 = 0, x8=0;
		uint xx[2]={0,0};

		for (uint x=0; x<SSD1306_LCDWIDTH; ++x, ++a)
		{
			x0 = 1-x0;  int c;
			c = Sin(xx[0]+tt[0]) + Sin(xx[1]+tt[1]) +
				Sin(yy[0]+t2[0]) + Sin(yy[1]+t2[1]);
			c *= SY*5/2;
			c = abs(c);
			xx[0]+=172;  xx[1]+=112;
		
			c /= SY*(SY/12);
			if (dith(x8,y8,c))
				buf[a] |= b1;
			else
				buf[a] &= b0;
			++x8;  if (x8==8) x8=0;
		}
		yy[0]+=155;  yy[1]+=131;
		++y8;  if (y8==8) y8=0;
	}	t+=tadd[1];
}

void Demos::Plasma2(uint8_t* buf)
{
	uint y0 = 0, y8=0;
	uint tt[4]={t*7,t*17,t*8,t*3};
	uint yy[6]={0,0,0,0,0,0};

	for (uint y=0; y<SSD1306_LCDHEIGHT; ++y)
	{
		y0 = 1-y0;  uint a = (y/8)*SSD1306_LCDWIDTH;
		uint8_t b1 = (1 << (y&7));  uint8_t b0 = ~(1 << (y&7));
		uint x0 = 0, x8=0;
		uint xx[6]={0,0,0,0,0,0};

		for (uint x=0; x<SSD1306_LCDWIDTH; ++x, ++a)
		{
			x0 = 1-x0;  int c;
			c = 8*Sin( yy[0] +xx[0]) * Cos( xx[1] +tt[0]);
			c+= 8*Cos( yy[1] +tt[1]) * Sin( xx[2] +tt[2]);
			c+= 6*Sin( yy[2] +tt[3]) * Cos( xx[3] +yy[3]);
			c+= 6*Cos( yy[4] +xx[4]) * Sin( xx[5] +yy[5]);
			c = abs(c);
			xx[0]+=15; xx[1]+=62; xx[2]+=85; xx[3]+=92; xx[4]+=77; xx[5]+=115;
		
			c /= SY*(SY/12);
			if (dith(x8,y8,c))
				buf[a] |= b1;
			else
				buf[a] &= b0;
			++x8;  if (x8==8) x8=0;
		}
		yy[0]+=66; yy[1]+=51; yy[2]+=166; yy[3]+=6; yy[4]+=151; yy[5]+=4;
		++y8;  if (y8==8) y8=0;
	}	t+=tadd[2];
}

void Demos::Plasma3(uint8_t* buf)
{
	uint y0 = 0, y8=0;
	uint tt[16]={t*9,t*12,t*5,t*4, t*14,t*3,t*7,t*12, t*5,t*13,t*4,t*8, t*2,t*6,t*10,t*11};
	uint yy[12]={0,0,0,0,0,0,0,0,0,0,0,0};

	for (uint y=0; y<SSD1306_LCDHEIGHT; ++y)
	{
		y0 = 1-y0;  uint a = (y/8)*SSD1306_LCDWIDTH;
		uint8_t b1 = (1 << (y&7));  uint8_t b0 = ~(1 << (y&7));
		uint x0 = 0;//, x8=0;
		uint xx[12]={0,0,0,0,0,0,0,0,0,0,0,0};

		for (uint x=0; x<SSD1306_LCDWIDTH; ++x, ++a)
		{
			x0 = 1-x0;  int c;
			c = 4*Sin( yy[0] +xx[0]+tt[0]) * Cos( xx[1]        +tt[1]);
			c-= 4*Sin( yy[1]       +tt[2]) * Cos( xx[2] +yy[2] +tt[3]);
			c+= 5*Cos( yy[3] +xx[4]+tt[4]) * Sin( xx[3]        +tt[5]);
			c-= 5*Cos( yy[4]       +tt[6]) * Sin( xx[5] +yy[5] +tt[7]);
			c+= 4*Sin( yy[6]       +tt[8]) * Cos( xx[6]        +tt[9]);
			c-= 4*Sin( yy[8] +xx[7]+tt[10])* Cos( xx[8]        +tt[11]);
			c+= 5*Cos( yy[9] +xx[9]+tt[12])* Sin( xx[10]+yy[7]+tt[13]);
			c-= 5*Cos( yy[11]      +tt[14])* Sin( xx[11]+yy[10]+tt[15]);
			c = abs(c);
			xx[0]+=111; xx[1]+=75; xx[2]+=100; xx[3]+=97; xx[4]+=154; xx[5]+=143;
			xx[6]+=131; xx[7]+=53; xx[8]+=144; xx[9]+=88; xx[10]+=132; xx[11]+=17;
		
			c /= SY*(SY/12);
			if (dith(x%8,y8,c))
				buf[a] |= b1;
			else
				buf[a] &= b0;
			//++x8;  if (x8==8) x8=0;
		}
		yy[0]+=71; yy[1]+=93; yy[2]+=128; yy[3]+=151; yy[4]+=78; yy[5]+=149;
		yy[6]+=136; yy[7]+=56; yy[8]+=213; yy[9]+=109; yy[10]+=82; yy[11]+=132;
		++y8;  if (y8==8) y8=0;
	}	t+=tadd[3];
}

void Demos::Plasma4(uint8_t* buf)
{
	uint y0 = 0;//, y8=0;
	uint tt[32]={t*8,t*7,t*13,t*12, t*17,t*8,t*7,t*13, t*5,t*11,t*9,t*7, t*10,t*10,t*4,t*5,
				 t*5,t*7,t*1 ,t*5 , t*3 ,t*8,t*2,t*6 , t*5,t*9 ,t*2,t*7, t*3 ,t*8 ,t*4,t*5};	             
	uint yy[24]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	for (uint y=0; y<SSD1306_LCDHEIGHT; ++y)
	{
		y0 = 1-y0;  uint a = (y/8)*SSD1306_LCDWIDTH;
		uint8_t b1 = (1 << (y&7));  uint8_t b0 = ~(1 << (y&7));
		uint x0 = 0;//, x8=0;
		uint xx[24]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

		for (uint x=0; x<SSD1306_LCDWIDTH; ++x, ++a)
		{
			x0 = 1-x0;  int c;
			c = 4*Sin( yy[0] +xx[0]+tt[0]) * Cos( xx[1]        +tt[1]);
			c-= 4*Sin( yy[1]       +tt[2]) * Cos( xx[2] +yy[2] +tt[3]);
			c+= 5*Cos( yy[3]       +tt[4]) * Sin( xx[3]        +tt[5]);
			c-= 5*Cos( yy[4] +xx[4]+tt[6]) * Sin( xx[5] +yy[5] +tt[7]);
			c+= 4*Sin( yy[6]       +tt[8]) * Cos( xx[6] +yy[7] +tt[9]);
			c-= 4*Sin( yy[8] +xx[7]+tt[10])* Cos( xx[8]        +tt[11]);
			c+= 5*Cos( yy[9] +xx[9]+tt[12])* Sin( xx[10]+yy[10]+tt[13]);
			c-= 5*Cos( yy[11]      +tt[14])* Sin( xx[11]       +tt[15]);
						
			c = 4*Sin( yy[12]+xx[12]+tt[16])*Cos( xx[13]       +tt[17]);
			c-= 4*Sin( yy[13]       +tt[18])*Cos( xx[14]+yy[14]+tt[19]);
			c+= 5*Cos( yy[15]       +tt[20])*Sin( xx[15]       +tt[21]);
			c-= 5*Cos( yy[16]+xx[16]+tt[22])*Sin( xx[17]+yy[17]+tt[23]);
			c+= 4*Sin( yy[18]       +tt[24])*Cos( xx[18]+yy[19]+tt[25]);
			c-= 4*Sin( yy[20]+xx[19]+tt[26])*Cos( xx[20]       +tt[27]);
			c+= 5*Cos( yy[21]+xx[21]+tt[28])*Sin( xx[22]+yy[22]+tt[29]);
			c-= 5*Cos( yy[23]       +tt[30])*Sin( xx[23]       +tt[31]);
			c = abs(c);
			xx[0]+=15; xx[1]+=62; xx[2]+=71; xx[3]+=85; xx[4]+=12; xx[5]+=57;
			xx[6]+=92; xx[7]+=63; xx[8]+=81; xx[9]+=77; xx[10]+=115; xx[11]+=107;
			xx[12]+=315; xx[13]+=162; xx[14]+=171; xx[15]+=25; xx[16]+=212; xx[17]+=27;
			xx[18]+=82; xx[19]+=163; xx[20]+=71; xx[21]+=177; xx[22]+=325; xx[23]+=301;

			c /= SY*(SY/12);
			if (dith(x%8,y%8,c))
				buf[a] |= b1;
			else
				buf[a] &= b0;
			//++x8;  if (x8==8) x8=0;
		}
		yy[0]+=66; yy[1]+=83; yy[2]+=8; yy[3]+=51; yy[4]+=77; yy[5]+=9;
		yy[6]+=166; yy[7]+=6; yy[8]+=183; yy[9]+=151; yy[10]+=4; yy[11]+=177;
		yy[12]+=266; yy[13]+=283; yy[14]+=48; yy[15]+=351; yy[16]+=377; yy[17]+=39;
		yy[18]+=16; yy[19]+=86; yy[20]+=13; yy[21]+=11; yy[22]+=74; yy[23]+=17;
		//++y8;  if (y8==8) y8=0;
	}	t+=tadd[4];
}

