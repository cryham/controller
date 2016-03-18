
#include <SPI.h>
#include <Lib/delay.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED
#ifdef OLED
	// hw spi  13 clk  9. d/c  10 cs  11 mosi (dout)
	#define OLED_CS    10
	#define OLED_DC     9
#endif


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


//------------------------------
//#define TIM
#ifdef TIM
//#include "IntervalTimer.h"

volatile uint32_t t1 = 0;

void timerCallback0()
{	t1++;
	//if (t1 % 2)
	//	digitalWrite(14, HIGH);
	//else
	//	digitalWrite(14, LOW);
}
#endif

Demos demos;


//------------------------------
int main()
{
	//  timer
	//pit0_setup();
	//IntervalTimer timer0;

	#ifdef KII
	// Enable CLI
	//CLI_init();

	// Setup Modules
	Output_setup();
	Macro_setup();
	Scan_setup();
	#endif

	//pinMode(12, OUTPUT);
	//pinMode(14, OUTPUT);
	//digitalWrite(12, HIGH);
	//digitalWrite(14, LOW);

	#ifdef OLED
	//  display
	pinMode(OLED_DC, OUTPUT);
	pinMode(OLED_CS, OUTPUT);

	Adafruit_SSD1306 display(OLED_DC, -1, OLED_CS);

	display.begin(SSD1306_SWITCHCAPVCC);
	display.clearDisplay();
	display.dim(1);

	display.setTextSize(1);
	display.setTextColor(WHITE);

	int i;
	//uint16_t u = 0, n = 3;
	uint32_t ti = 0, oti = 0;
	#endif
	
	#if 0  // ee
	eeprom_initialize();
	uint16_t w = eeprom_read_word((uint16_t*)0x00);
	//eeprom_write_word((uint16_t*)0x00, 0x1234);
	#endif
	
	///  menu state
	uint8_t fps = 1, menu = 0, anyo = 0;
	int8_t demo = 0;

	demos.Init();

	int ii = 0;
	while ( 1 )
	{
	
		#ifdef KII
		// Process CLI
		//CLI_process();

		// Acquire Key Indices
		cli();
		while ( Scan_loop() );
		sei();

		// Run Macros over Key Indices and convert to USB Keys
		Macro_process();

		// Sends USB data only if changed
		Output_send();
		#endif

		ii++;
		if (ii == 50)
		{
			#ifdef OLED
			display.begin(SSD1306_SWITCHCAPVCC);
			display.clearDisplay();
			display.dim(1);

			display.setTextSize(1);
			display.setTextColor(WHITE);
			#endif

			#ifdef TIM
			//timer0.begin(timerCallback0, 20000);  // 100 Hz
			//pit0_setup();
			#endif
		}
		
	#ifdef OLED
	//--------------------------------
	uint8_t any = fps || menu || demo ? 1 : 0;

	//  clear on display off
	if (!any && anyo)
	{
		display.clearDisplay();
		display.display();
	}		
	anyo = any;
	
	if (any)
	{
		if (ii > 50)
		{
		//  demos  ....
		switch (demo)
		{
			case 0: display.clearDisplay();  break;
			case 1: display.clearDisplay();  demos.CK_logo(display);  break;
			case 2: display.clearDisplay();  demos.Space(display);  break;
			case 3: display.clearDisplay();  demos.Balls(display);  break;
			case 4: demos.Rain(display);  break;
			case 5: display.clearDisplay();  demos.Ngons(display);  break;
			case 6: display.clearDisplay();  demos.Chars(display,0);  break;
			case 7: display.clearDisplay();  demos.Chars(display,1);  break;
			case 8: display.clearDisplay();  demos.Fonts(display);  break;
			case 9: demos.Plasma1(display.getBuffer());  break;
			case 10: demos.Plasma2(display.getBuffer());  break;
			case 11: demos.Plasma3(display.getBuffer());  break;
			case 12: demos.Plasma4(display.getBuffer());  break;
			#define DLAST 12
		}


		if (fps)  // fps
		{
			oti = ti;  ti = micros();
			display.setFont(0);
			display.setCursor(0,0);
			display.println(int16_t(1000000.0/(ti-oti)));
			///display.println(w);  // ee
			//display.println(ii);
			#ifdef TIM
			display.println(t1);  // tim1
			#endif
		}
		
		if (menu)  // settings..
		{
			display.setFont(0);
			display.setCursor(64,0);
			display.println("Menu");
			// cd_..
		}

		//  show  ----
		display.display();
		}
	}

		
		//  key press
		//--------------------------------
		#ifdef KII
		if (kk[4] && !kko[4])  // ^
		{
			menu = 1-menu;
			
			/*uint8_t key;
			key = KEY_P;	Output_usbCodeSend_capability(1, 0, &key);	Output_send();
			key = 0x00;		Output_usbCodeSend_capability(1, 0, &key);	Output_send();
			key = KEY_A;	Output_usbCodeSend_capability(1, 0, &key);	Output_send();
			key = 0x00;		Output_usbCodeSend_capability(1, 0, &key);	Output_send();
			/**/
		}

		if (kk[6] && !kko[6])  // >
		{	demo++;  if (demo > DLAST)  demo = 0;  }
		if (kk[5] && !kko[5])  // <
		{	demo--;  if (demo < 0)  demo = DLAST;  }
		
		if (kk[7] && !kko[7])  // v
		{	fps = 1-fps;  }

		//  old states
		for (i=0; i<8; ++i)
			kko[i] = kk[i];
		#endif

	#endif
	}
}
