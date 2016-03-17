#include "demos.h"
#include <math.h>
#include <arm_math.h>


//  Plasma
// ...........................................................................................................

void Demos::PlasmaInit()
{
	t = 0;
	for (int i=0; i < SX; ++i)
		sint[i] = sin(2.f*PI * i/SX) * (SY-1);
}

void Demos::Plasma0(uint8_t* buf)
{
	int x,y,a, x0, y0 = 0;
	uint8_t b1,b0;  int c;

	for (y=0; y<SSD1306_LCDHEIGHT; ++y)
	{
		y0 = 1-y0;  a = (y/8)*SSD1306_LCDWIDTH;
		b1 = (1 << (y&7));  b0 = ~(1 << (y&7));
		x0 = 0;
		for (x=0; x<SSD1306_LCDWIDTH; ++x, ++a)
		{
			x0 = 1-x0;
		#if 0
			c = SY*SY - Sin(y*60) * Sin(S2+ x*60 +t);
			c *= 6;
		#else
			c = Sin(x*112+t*11) + Sin(x*142+t*9) +
				Sin(y*155+t*10) + Sin(y*121+t*8);
			c *= SY*5/2;
		#endif
			c = abs(c);
		
			//  dither6
			c /= SY*(SY/6);  c = 35-c;
			c = c/6;
			if (c==6 ||
				(c==5 && (y+3)/3 && (x+3)/3) ||
				(c==4 && y0+x0<2) ||
				(c==3 && y0==x0 ) ||
				(c==2 && y0+x0>1) ||
				(c==1 && y%3==0 && x%3==0))
				buf[a] |= b1;
			else
				buf[a] &= b0;
		}
	}	t+=2;
}

void Demos::Plasma1(uint8_t* buf)
{
	int x,y,a, x0, y0 = 0;
	uint8_t b1,b0;  int c;

	for (y=0; y<SSD1306_LCDHEIGHT; ++y)
	{
		y0 = 1-y0;  a = (y/8)*SSD1306_LCDWIDTH;
		b1 = (1 << (y&7));  b0 = ~(1 << (y&7));
		x0 = 0;
		for (x=0; x<SSD1306_LCDWIDTH; ++x, ++a)
		{
			x0 = 1-x0;
		#if 0
			c = SY*SY - Sin(y*60) * Sin(S2+ x*60 +t);
			c *= 6;
		#else
			c = Sin(x*112+t*11) + Sin(x*142+t*9) +
				Sin(y*155+t*10) + Sin(y*121+t*8);
			c *= SY*5/2;
		#endif
			c = abs(c);
		
			//  dither6
			c /= SY*(SY/6);  c = 35-c;
			c = c/6;
			if (c==6 ||
				(c==5 && (y+3)/3 && (x+3)/3) ||
				(c==4 && y0+x0<2) ||
				(c==3 && y0==x0 ) ||
				(c==2 && y0+x0>1) ||
				(c==1 && y%3==0 && x%3==0))
				buf[a] |= b1;
			else
				buf[a] &= b0;
		}
	}	t+=2;
}

void Demos::Plasma2(uint8_t* buf)
{
	static int x,y,a, x0, y0 = 0;
	static uint8_t b1,b0;  static int c;

	for (y=0; y<SSD1306_LCDHEIGHT; ++y)
	{
		y0 = 1-y0;  a = (y/8)*SSD1306_LCDWIDTH;
		b1 = (1 << (y&7));  b0 = ~(1 << (y&7));
		x0 = 0;
		for (x=0; x<SSD1306_LCDWIDTH; ++x, ++a)
		{
			x0 = 1-x0;
			c = 8*Sin(    y*66  +x*15) * Sin(S2+ x*62 +t*7);
			c+= 8*Sin(S2+ y*51  +t*17) * Sin(    x*85 +t*8);
			c+= 6*Sin(    y*166 +t*3 ) * Sin(S2+ x*92 +y*6);
			c+= 6*Sin(S2+ y*151 +x*77) * Sin(    x*115+y*4);
			c = abs(c);
		
			//  dither 6
			c /= SY*(SY/6);  c = 35-c;
			c = c/6;
			if (c==6 ||
				(c==5 && (y+3)/3 && (x+3)/3) ||
				(c==4 && y0+x0<2) ||
				(c==3 && y0==x0 ) ||
				(c==2 && y0+x0>1) ||
				(c==1 && y%3==0 && x%3==0))
				buf[a] |= b1;
			else
				buf[a] &= b0;
		}
	}	t+=3;
}

void Demos::Plasma3(uint8_t* buf)
{
	//int x,y,a, x0, y0 = 0;
	uint8_t b1,b0;  //int c;

	int y0 = 0;
	for (int y=0; y<SSD1306_LCDHEIGHT; ++y)
	{
		y0 = 1-y0;  int a = (y/8)*SSD1306_LCDWIDTH;
		b1 = (1 << (y&7));  b0 = ~(1 << (y&7));
		int x0 = 0;
		for (int x=0; x<SSD1306_LCDWIDTH; ++x, ++a)
		{
			x0 = 1-x0;  int 
			c = 4*Sin(    y*66  +x*15+t*8 ) * Sin(S2+ x*62       +t*7 );
			c-= 4*Sin(    y*83       +t*13) * Sin(S2+ x*71  +y*8 +t*12);
			c+= 5*Sin(S2+ y*51       +t*17) * Sin(    x*85       +t*8 );
			c-= 5*Sin(S2+ y*77  +x*12+t*7 ) * Sin(    x*57  +y*9 +t*13);
			c+= 4*Sin(    y*166      +t*5 ) * Sin(S2+ x*92  +y*6 +t*11);
			c-= 4*Sin(    y*183 +x*63+t*9 ) * Sin(S2+ x*81       +t*7 );
			c+= 5*Sin(S2+ y*151 +x*77+t*10) * Sin(    x*115 +y*4 +t*10);
			c-= 5*Sin(S2+ y*177      +t*4 ) * Sin(    x*107      +t*5 );
			c = abs(c);
		
			//  dither 4
			c /= SY*SY;  c = 5-c;
			if (c>=4 ||
				(c==3 && y0+x0 <2) ||
				(c==2 && y0==x0) ||
				(c==1 && y0+x0 >1) )
				buf[a] |= b1;
			else
				buf[a] &= b0;
		}
	}	t+=4;
}

void Demos::Plasma4(uint8_t* buf)
{
	//int x,y,a, x0, y0 = 0;
	//uint8_t b1,b0;  int c;

	uint y0 = 0;
	for (uint y=0; y<SSD1306_LCDHEIGHT; ++y)
	{
		y0 = 1-y0;  uint a = (y/8)*SSD1306_LCDWIDTH;
		uint8_t b1 = (1 << (y&7));  uint8_t b0 = ~(1 << (y&7));
		uint x0 = 0;
		for (uint x=0; x<SSD1306_LCDWIDTH; ++x, ++a)
		{
			x0 = 1-x0;  int
			c = 4*Sin(    y*66  +x*15+t*8 ) * Sin(S2+ x*62       +t*7 );
			c-= 4*Sin(    y*83       +t*13) * Sin(S2+ x*71  +y*8 +t*12);
			c+= 5*Sin(S2+ y*51       +t*17) * Sin(    x*85       +t*8 );
			c-= 5*Sin(S2+ y*77  +x*12+t*7 ) * Sin(    x*57  +y*9 +t*13);
			c+= 4*Sin(    y*166      +t*5 ) * Sin(S2+ x*92  +y*6 +t*11);
			c-= 4*Sin(    y*183 +x*63+t*9 ) * Sin(S2+ x*81       +t*7 );
			c+= 5*Sin(S2+ y*151 +x*77+t*10) * Sin(    x*115 +y*4 +t*10);
			c-= 5*Sin(S2+ y*177      +t*4 ) * Sin(    x*107      +t*5 );

			c-= 4*Sin(    y*266+x*315+t*5 ) * Sin(S2+ x*162       +t*7 );
			c+= 4*Sin(    y*283      +t*1 ) * Sin(S2+ x*171 +y*48 +t*5 );
			c-= 5*Sin(S2+ y*351      +t*3 ) * Sin(    x*25        +t*8 );
			c+= 5*Sin(S2+ y*377+x*212+t*2 ) * Sin(    x*27  +y*39 +t*6 );
			c-= 4*Sin(    y*16       +t*5 ) * Sin(S2+ x*82  +y*86 +t*9 );
			c+= 4*Sin(    y*13 +x*163+t*2 ) * Sin(S2+ x*71        +t*7 );
			c-= 5*Sin(S2+ y*11 +x*177+t*3 ) * Sin(    x*315 +y*74 +t*8 );
			c+= 5*Sin(S2+ y*17       +t*4 ) * Sin(    x*307       +t*5 );
			c = abs(c);

			//  dither 4
			c /= SY*SY;  c = 5-c;
			if (c>=4 ||
				(c==3 && y0+x0 <2) ||
				(c==2 && y0==x0) ||
				(c==1 && y0+x0 >1) )
				buf[a] |= b1;
			else
				buf[a] &= b0;
		}
	}	t+=5;
}

