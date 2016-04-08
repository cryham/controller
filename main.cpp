
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
#include "games.h"

Demos demos;
#ifdef KII
Gui gui;
#endif
Games game;


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
	gui.tInfo = 0;
	#endif
	game.Init();


	//--------------------------------  loop
	#define iwait 50  // 2nd reset
	int any_old = 0;
	int ii = 0;
	while (1)
	{
	
		#ifdef KII   //----  kiibohd
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
		if (ii == iwait)
		{
			demos.Reset(display);  // oled init 2nd
		}
		
		//--------------------------------  display
		#ifdef KII
		int any = gui.menu;
		#else
		int any = 1;
		#endif

		//  clear on display off
		if (!any && any_old && ii > iwait)
		{
			display.clearDisplay();
			display.display();
		}
		if (ii > iwait)
			any_old = any;
			
		#ifdef KII
		int demo = gui.DrawDemo(), play = gui.IsGame();
		#else
		int demo = 0, play = 1;
		#endif
		
		if (any)
		{
			if (ii > iwait)
			{
				if (play)
				{	game.Draw(display);
					if (game.KeyPress(gui.mlevel))
					{	gui.help = 1;  gui.hpage = Gui::HAll-2;  }
				}
				else
				#ifdef KII
				demos.Draw(display, demo,  gui.ym, gui.ym2[gui.ym]);
				#else
				demos.Draw(display, 1, 1, 0);  //par
				#endif
			}
			
			#ifdef KII
			if (gui.menu && !play)
				gui.Draw(display);  // gui
			#endif

			display.display();  // show
		}
		
		#ifdef KII
		if (!play)
		{	if (gui.menu)
			{
				demos.KeyPress(demo && gui.ym >= MDemos,  gui.ym, gui.ym2[gui.ym]);
				demos.KeyGlob(display);
			}
			gui.KeyPress();
			gui.ExecSeqs();
		}
		//  old key states  ----
		for (int i=0; i < 0xFF; ++i)
			kko[i] = kk[i];
		#endif
	}
}
