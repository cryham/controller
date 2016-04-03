
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

	//  display
	pinMode(OLED_DC, OUTPUT);
	pinMode(OLED_CS, OUTPUT);
	Adafruit_SSD1306 display(OLED_DC, -1, OLED_CS);

	eeprom_initialize();

	demos.Init();
	demos.Reset(display);  // oled init 1st

	#ifdef KII
	gui.LedsInit();  //
	gui.Load();  // load seq data from eeprom
	#endif


	//--------------------------------  loop
	#define iwait 50  // 2nd reset
	int anyold = 0;
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
		#ifdef KII
		int any = gui.menu ? 1 : 0;
		#else
		int any = 1;
		#endif

		//  clear on display off
		if (!any && anyold && ii > iwait)
		{
			display.clearDisplay();
			display.display();
		}
		if (ii > iwait)
			anyold = any;
			
		int demo = gui.menu && gui.mlevel > 0 && !gui.help;
		
		if (any)
		{
			if (ii > iwait)
			{
				#ifdef KII
				demos.Draw(display, demo,  gui.ym, gui.ym2[gui.ym]);
				#else
				demos.Draw(display, 1, 1, 8);  //par
				#endif
				
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
		if (gui.menu)
		{
			demos.KeyPress(demo && gui.ym >= MDemos,  gui.ym, gui.ym2[gui.ym]);
			demos.Dim(display);
		}
		gui.KeyPress();
		gui.ExecSeqs();
		
		//  old key states  ---
		for (int i=0; i < 0xFF; ++i)
			kko[i] = kk[i];
		#endif
	}
}
