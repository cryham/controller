
#include <SPI.h>
#include <Lib/delay.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// hw spi  13 clk  9. d/c  10 cs  11 mosi (dout)
#define OLED_CS    10
#define OLED_DC     9

#define W 128  // area
#define H 64
#define A 40  // all
#define D 256   // move detail
#define E 80   // speed
#define R 3   // radius max
int32_t x[A], y[A], a[A], b[A], r[A];


#define KII
#ifdef KII

#ifdef __cplusplus
extern "C" {
#endif
#include <Lib/MainLib.h>

#include <macro.h>
#include <scan_loop.h>
#include <output_com.h>

#include <cli.h>
#include <led.h>
//#include <print.h>

#ifdef __cplusplus
}
#endif
#endif



//------------------------------

int main()
{


	#ifdef KII
	// Enable CLI
	CLI_init();

	// Setup Modules
	Output_setup();
	Macro_setup();
	Scan_setup();
	#endif


	//  display
	pinMode(OLED_DC, OUTPUT);
	pinMode(OLED_CS, OUTPUT);

	Adafruit_SSD1306 display(OLED_DC, -1, OLED_CS);

	display.begin();
	display.clearDisplay();
	display.dim(1);

	display.setTextSize(1);
	display.setTextColor(WHITE);

	uint16_t i;
	//uint16_t u = 0, n = 3;
	uint32_t ti = 0, oti = 0;

	for (i=0; i<A; i++)
	{
		r[i] = random(R);
		x[i] = random(W)*D;  y[i] = random(H)*D;
		a[i] = random(E+1)-E/2;  b[i] = random(E+1)-E/2;
	}

	while ( 1 )
	{
		#ifdef KII
		// Process CLI
		CLI_process();

		// Acquire Key Indices
		// Loop continuously until scan_loop returns 0
		cli();
		while ( Scan_loop() );
		sei();

		// Run Macros over Key Indices and convert to USB Keys
		Macro_process();

		// Sends USB data only if changed
		Output_send();
		#endif

		
		//  clear
		display.clearDisplay();
		
		#if 1
		//  balls .
		for (i=0; i < A; i++)
		{
			if (x[i] < 0 || x[i] >= W*D-1)  a[i] = -a[i];
			if (y[i] < 0 || y[i] >= H*D-1)  b[i] = -b[i];
			x[i] += a[i];  y[i] += b[i];

			if (r[i]==0)
				display.drawPixel(x[i]/D, y[i]/D, WHITE);
			else
				display.drawCircle(x[i]/D, y[i]/D, r[i], WHITE);
		}
		#endif
		
		#if 0
		//  lines n-gon x
		#define uu 200
		float a=0, b=0, d = 2*PI/n;
		float c = -PI/2*u/uu, s = 0.65 + 0.75*u/uu;
		float xx = W/2, yy = H/2, sy = s * yy;
		for (b=0; b < 2*PI; b+=d)
		{
			float xb = xx + sy*cos(b+c), yb = yy - sy*sin(b+c);
			for (a=0; a < b && a < 2*PI; a+=d)
			{
				float xa = xx + sy*cos(a+c), ya = yy - sy*sin(a+c);
				if (a != b)
					display.drawLine(xb, yb, xa, ya, 1);
		}	}
		++u;  if (u > uu)
		{	u = 0;  ++n;  if (n > 14)  n = 3;  }
		display.setCursor(0, H-1-7);
		display.print(n);  // fps
		#endif


		#if 1
		//  text fps
		display.setCursor(0,0);
		oti = ti;
		ti = micros();
		//display.print("us:  ");
		//display.println(ti-oti);  // us
		//display.print("fps: ");
		display.println(int16_t(1000000.0/(ti-oti)));  // fps
		#endif

		#if 0
		//  text all chars
		display.setCursor(0,8);
		for (i=0; i < 168; i++) {
			if (i == '\n') continue;
			display.write(128+i);
			if ((i > 0) && (i % 20 == 0))
			  display.println();
		}    
		#endif

		//  show
		display.display();
	}
}

