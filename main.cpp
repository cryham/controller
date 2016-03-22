
#include <SPI.h>
#include <Lib/delay.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define PROGMEM
#include "FreeSans9pt7b.h"

// hw spi  13 clk  9. d/c  10 cs  11 mosi (dout)
#define OLED_CS    10
#define OLED_DC     9

#define KII

#ifdef KII
extern "C" {
	#include <Lib/MainLib.h>
	#include <macro.h>
	#include <scan_loop.h>
	#include <output_com.h>
	#include <usb_hid.h>
	//#include <cli.h>
	//#include <led.h>
	//#include <print.h>
}
#endif

#include "demos.h"
#include "gui.h"


//------------------------------
//#define TIM
#ifdef TIM
//#include "IntervalTimer.h"
//IntervalTimer timer0;

volatile uint32_t t1 = 0;

void timerCallback0()
{	t1++;
}
#endif

Demos demos;
#ifdef KII
Gui gui;
#endif


//..............................................................................
int main()
{
	#ifdef KII
	//CLI_init();
	Output_setup();
	Macro_setup();  Scan_setup();
	#endif

	//pinMode(12, OUTPUT);  // test led
	pinMode(14, OUTPUT);
	//digitalWrite(12, HIGH);
	digitalWrite(14, LOW);

	//  display
	pinMode(OLED_DC, OUTPUT);
	pinMode(OLED_CS, OUTPUT);
	Adafruit_SSD1306 display(OLED_DC, -1, OLED_CS);

	eeprom_initialize();

	demos.Init();
	demos.Reset(display);  // oled init 1st

	#ifdef KII
	gui.Load();  // load seq data from eeprom
	#endif


	//--------------------------------  loop
	uint8_t anyold = 0;
	int ii = 0;
	while (1)
	{
	
		#ifdef KII
		//  Process CLI
		//CLI_process();

		//  Acquire Key Indices
		cli();
		while ( Scan_loop() );
		sei();

		//  Run Macros over Key Indices and convert to USB Keys
		Macro_process();
		//  Sends USB data only if changed
		Output_send();
		#endif

		++ii;
		if (ii == 50)
		{
			demos.Reset(display);  // oled init 2nd

			#ifdef TIM
			//timer0.begin(timerCallback0, 20000);  // 100 Hz
			#endif
		}
		
		//--------------------------------  display
		uint8_t any = 
			#ifdef KII
			gui.menu || 
			#endif
			demos.fps ? 1 : 0;/**/

		//  clear on display off
		if (!any && anyold && ii > 50)
		{
			display.clearDisplay();
			display.display();
		}
		if (ii > 50)
			anyold = any;
		
		if (any)
		{
			if (ii > 50)
			{
				demos.Draw(display, gui.menu && gui.mlevel > 0,
					gui.ym, gui.ym2[gui.ym]);
				
				#ifdef TIM
				display.setFont(0);
				display.setCursor(0,0);
				///display.println(w);  // ee
				display.println(t1);  // tim1
				#endif
			}
			
			#ifdef KII
			gui.Draw(display);  // gui
			#endif

			display.display();  // show
		}
		
		//if (ii % 300 == 200)  demos.Next();
		#ifdef KII
		if (gui.menu && gui.mlevel > 0 && gui.ym > 0)
			demos.KeyPress();

		gui.KeyPress();
		
		//  old key states  ---
		for (int i=0; i<0xF0; ++i)
			kko[i] = kk[i];
		#endif
	}
}
