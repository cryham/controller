#include "gui.h"
#include "demos.h"  // ver
#define PROGMEM
#include "FreeSans9pt7b.h"

//#ifdef KII
extern "C" {
	#include <macro.h>
	#include <Output/pjrcUSB/arm/usb_mouse.h>
	#include <scan_loop.h>  // keys kk
	#include <usb_hid.h>  // key_defs
	#include "matrix_scan.h"  // defines
}
//#include "matrix.h"
//#endif

///  string from usb hid code
const int strALL = 0x73;
const char str[strALL][6] = { ".",".",".",".", /*04*/"A","B","C","D",
/*08*/"E","F","G","H","I","J","K","L", /*10*/"M","N","O","P","Q","R","S","T",
/*18*/"U","V","W","X","Y","Z","1","2", /*20*/"3","4","5","6","7","8","9","0",
/*28*/"Ent","Esc","Bck","Tab","Spc",
/*2D*/"-","=","[","]","\\", /*32*/"NUM-",";","\"","`",",",".", "/","CapsL",
/*3A*/"F1","F2","F3","F4", "F5", "F6",
/*40*/"F7","F8","F9","F10","F11","F12", /*46*/"PtrSc","ScrL","Pause",
/*49*/"Ins","Home","PgUp", "Del","End","PgDn", //*4F*/"Right","Left","Down","Up",
/*4F*/{26,0},{27,0},{25,0},{24,0},
/*53*/"NumL","P/","P*","P-","P+","PEnt",
/*59*/"P1","P2","P3","P4","P5","P6","P7","P8","P9","P0","P.",
/*68*/"F13","F14","F15","F16","F17","F18","F19","F20","F21","F22","F23","F24"};
/*..*/
const char mod[0x08][6] = {
/*E0*/"Ct","Sh","Al","Gui", //L
/*E4*/"Ct","Sh","Al","Gui", //R
};

//  key to string  // wait, delay symbols,  modifiers, key
#define STR(k)  (k==2 ? "\xB1" : k==1 ? "\xB0" : \
				 k >= 0xE0 ? mod[k-0xE0] : \
				 k <= strALL ? str[k] : "()")


///  Key for sequence  - update after ck4layer2.kll change
const char csSeqKey[Gui::iSlots][5] = {
#if 0  // ck4
	"1","2","3", "Q","W",
	"E",";","'", "/","\\",
	"Home","Left", "End","Ins","A",
	"S","D","Z","X","C"  };
#else  // ck3
	"1","2","3", "Q","W",
	"E","\\","Back", "Home","Ent",
	"Left", ";", "'","/","A",
	"S","D","Z","X","C"  };
#endif


//  Draw, settings
//....................................................................................
void Gui::Draw(Adafruit_SSD1306& d)
{
	if (mlevel==0 || mlevel==1 && ym == MSeq || tInfo > 0 || help || status)
	{
		d.clearDisplay();
		d.setFont(&FreeSans9pt7b);
		d.setCursor(0,0);
	}
	
	if (help)
	{
		d.print("Help");  d.setFont(0);
		d.setCursor(W-1 -5*6, 0);
		d.print(hpage+1);  d.print("/");  d.print(HAll);

		d.setCursor(52, 8);  // page titles
		const static char* title[HAll] = {"", "Toggle", "Menu",
			"Sequences", "Seq. cd", "Edit Seq", "Edit Seq cd",
			"Demos", "Demos cd", "Sixtis", "Sixtis cd"};
		d.print(title[hpage]);

		switch (hpage)
		{
		case 0:  // help  ------------------------
			d.setCursor(0, 16+8);
			d.println("ScrLck F1  Help");  // < x1B  > x1A  ^ x18  v x19
			d.moveCursor(0,4);
			d.println("\x18\x19  Prev/Next Page");
			d.moveCursor(0,4);
			d.println("\x1B Esc  Back");
			break;

		case 1:  // toggle
			d.setCursor(0, 16+8);
			d.println("Pause F2  Status");
			d.moveCursor(0,2);
			d.println("  \x18\x19  Page");
			d.moveCursor(0,6);
			d.println("D Spc  Dim display");
			break;

		case 2:  // menu
			d.setCursor(0, 16+8);
			d.println("\x18\x19  Cursor \x10");
			d.moveCursor(0,4);
			d.println("\x1A  Enter");
			d.moveCursor(0,2);
			d.println("\x1B  Back");
			break;

		case 3:  // sequences  ------------------------
			d.setCursor(0, 16+8);
			d.println("\x18\x19  Cursor");
			d.moveCursor(0,2);
			d.println("PgUp,PgDn  Page");
			d.moveCursor(0,4);
			d.println("Num Enter - Edit");
			break;

		case 4:  // seq cd
			d.setCursor(0, 16+8);
			d.println("S Ins   Save");
			d.println("BckSpc  Load");
			d.moveCursor(0,4);
			d.println("C    Copy");
			d.println("V Z  Paste");
			break;

		case 5:  // edit seq
			d.setCursor(0, 16+2);
			//d.println("  All on Layer2");
			//d.moveCursor(0,2);
			d.println("\x18\x19  Cursor  (Layer2)");
			d.println("PgUp,PgDn  Start/End");
			d.moveCursor(0,2);
			d.println("Ins +  Insert/overwr");
			d.println("Del  Delete");
			d.println("Enter  Clear");
			break;

		case 6:  // edit seq cd
			d.setCursor(0, 16+2);
			//\xFA\xF9\x07\xFE\xF8 middle dots
			//\xB0\xB1\xB2\xF0
			d.println("F2  \xB1 Wait command");
			d.println("F1  \xB0 Set Delay");
			d.moveCursor(0,2);
			d.println("  Next Digit 1..0");
			d.println("  1 none  4 20ms");
			d.println("  6 100  9 1s");
			break;

		case 7:  // demos  ------------------------
			d.setCursor(0, 16+2);
			d.println("\x18\x19  Prev/Next");
			d.moveCursor(0,2);
			d.println("+ F  Toggle Fps");
			d.moveCursor(0,4);
			d.println("R  Reset all");
			d.println("- Esc  Params Info");
			d.println("  Ctrl  Mode,Off");
			break;

		case 8:  // demos cd
			d.setCursor(0, 16+8);
			d.println("PgUp,PgDn  Change");
			d.moveCursor(0,2);
			d.println("Home,End   Params");
			d.moveCursor(0,2);
			d.println("  Ctrl  Others");
			d.println("  Shift  Fine");
			break;

		case 9:  // sixtis  ------------------------
			d.setCursor(0, 16+4);
			d.println("\x1B\x1A   Move");
			d.moveCursor(0,2);
			d.println("\x18    Rotate cw");
			d.println("5 /  Rot ccw");
			d.moveCursor(0,2);
			d.println("\x19    Fall");
			d.println("Ins  Drop");
			break;

		case 10:  // sixtis cd
			d.setCursor(0, 16+4);
			d.println("BckSpc Esc  Exit");
			d.println("+ Spc  Pause");
			d.println("Enter  New Game");
			d.moveCursor(0,4);
			d.println("- *  Options");
			d.println("\x1B\x1A  Dec,Inc Param");
			d.println("PgUp,PgDn  Page");
			break;
		}
		return;
	}
	
	
	if (status)  //  status info
	{
		d.print("Status");  d.setFont(0);

		d.setCursor(W-1 -3*6, 0);
		d.print(stpage+1);  d.print("/");  d.print(StAll);

		d.setCursor(0, 16+2);
		
		switch (stpage)  //  ----
		{
		case 0:
			d.print("Protocol: ");  d.print(USBKeys_Protocol);
			d.println(USBKeys_Protocol == 1 ? " NKRO" : " Boot");
			d.moveCursor(0,4);

			//  layers stack  - -
			d.print("Layers: ");
			for (int l=1; l < layersCnt; ++l)  // 1st is 1 in menu
			{
				d.print(layersOn[l]);  d.moveCursor(6,0);
			}
			//d.println();  d.moveCursor(0,2);

			Demos::Font_ver(d, true);  //*
			break;

		case 1:
		{	//  locks
			d.print("Lock: ");
			if (USBKeys_LEDs > 0)
			{
				if (USBKeys_LEDs & 0x1)  d.print("Num ");
				if (USBKeys_LEDs & 0x2)  d.print("Caps ");
				if (USBKeys_LEDs & 0x4)  d.print("Scrl");
				// 8=compose, 16=kana
			}
			d.println();  d.moveCursor(0,2);

			//  modifiers
			d.print("Mod:  ");
			for (int i=KEY_LCTRL; i<=KEY_RGUI; ++i)
			if (kk[i])
			{	d.print(i >= KEY_RCTRL ? "R" : "L");
				d.print(mod[i-KEY_LCTRL]);  d.print(" ");
			}
			d.println();  d.moveCursor(0,2);
			
			//  keys
			d.print("Key:  ");
			int c = 0;
			for (int i = KEY_A; i < strALL; ++i)
				if (kk[i])
				{	d.print(STR(i));  d.print(" ");  ++c;  }
			//  count
			d.setCursor(W-1 -(c > 9 ? 2*6 : 6), H-1-8);
			d.print(c);
			
			//  ghosting
			if (ghost_cols /*|| ghost_rows*/)
			{
				d.setCursor(3*6, H-1-8);
				d.print("Gho |");  d.print(ghost_cols);
				d.print(" -");  d.print(ghost_rows);
			}
		}	break;
			
		case 2:
			//  config setup, build vars
			d.print("Debounce ms: ");  d.println(MinDebounceTime_define);
			d.moveCursor(0,2);
			//d.print("Strobe delay us: ");  d.println(STROBE_DELAY);
			// rest in capabilities.kll *_define
			//DebounceThrottleDiv_define, StateWordSize_define;

			d.print("Sequences: ");  d.println(iSlots);
			d.moveCursor(0,2);
			d.print("Seq. max len: ");  d.println(iSeqLen);
			break;

		case 3:
			//  build vars
			///  dbg  mouse speed  --
			const int16_t y1 = H-1-2*8, y2 = H-1-1*8;
			d.println("Mouse");
			d.moveCursor(0,4);
			d.println("hold delay spd");

			d.setCursor(0, y1);  d.print(mx_holdtime);
			d.setCursor(0, y2);  d.print(my_holdtime);
			
			d.setCursor(W/3, y1);  d.print(mx_delay);
			d.setCursor(W/3, y2);  d.print(my_delay);
			
			d.setCursor(2*W/3, y1);  d.print(mx_speed);
			d.setCursor(2*W/3, y2);  d.print(my_speed);
			break;
		}
		return;
	}
	
	
	if (mlevel==0)  //  main menu
	{
		d.print("Menu");  d.setFont(0);

		d.setCursor(0, 16);
		for (int i=0; i < MAll; ++i)
		{
			d.print(i==ym ? "\x10":" ");
			switch (i)
			{
				case MSeq:    d.println("Sequences");  d.moveCursor(0,2);  break;
				case MDemos:  d.println("Demos");  break;
				case MText:   d.println("Text");  break;
				case MPlasma: d.println("Plasma");  d.moveCursor(0,2);  break;
				case MGames:  d.println("Sixtis");  break;
			}
		}
		d.setCursor(W-1 -7*6, H-8);
		d.print("F1 Help");
		//.
		d.setCursor(W-1 -1*6, H-16);
		d.println(USBKeys_Protocol == 1 ? "N" : "B");
		return;
	}
	
	
	if (mlevel==1 && ym == MSeq)
	if (!edit)
	{
		d.print("View");  d.setFont(0);
		
		//  list slots
		int s = page * iPage, i,n,x, xm;
		for (i=0; i < iPage && s < iSlots; ++i,++s)
		{
			d.setCursor(0, 18 + i*9);  //16 + i*8
			//d.print(i==slot ? "\x10":" ");   d.print(s);  d.print(": ");
			if (s<10)  d.print(" ");  //align
			d.print(s);  d.print(i==slot ? "\x10":" ");
			
			//  write sequence  ---
			n=0;  x=0;  xm=1;
			while (n < seql[s] && xm)
			{
				uint8_t z = seq[s][n];
				const char* st = STR(z);
				if (d.getCursorX() + 6*strlen(st) >= W -6*2)  // outside
				{	d.print(".");  d.moveCursor(-3,0);
					d.print(".");  xm = 0;  // more sign
				}else
				{	d.print(st);  //d.print(" ");
					d.moveCursor(z<=1 ? 0 : 2, 0);
					++n;
			}	}
		}
		//  page, center   / 
		d.setCursor(60, 4);
		d.print(page+1);  d.print("/");  d.print(iSlots/iPage);
		
		///  seq key
		if (tInfo == 0)
		{	int q = seqId();
			int l = strlen(csSeqKey[q]);
			d.setCursor(W-1 -l*8, 4);
			d.print(csSeqKey[q]);
		}
	}
	else
	{
		d.print("Edit");  d.setFont(0);
		int q = seqId();
		d.setCursor(W-1 -2*6, 4);  d.print(q);
		d.setCursor(W/2-6, 4);  d.print(edins ? "ins" : "ovr");
		//d.setCursor(W-1 -2*6, H-8);  d.print(edpos);

		//  write sequence  ---
		d.setCursor(0, 22);
		int n, l = seql[q];
		for (n=0; n <= l; ++n)  // +1 for cursor
		{
			int16_t x = d.getCursorX(), y = d.getCursorY();
			if (n < l)
			{
				uint8_t z = seq[q][n];
				d.print(STR(z));  //d.print(" ");
				d.moveCursor(z<=1 ? 0 : 2, 0);
			}
			if (n == edpos)  // show cursor
			{
				int16_t b = 8 - 8 * tBlnk / cBlnk;
				if (edins)  // ins |
					d.drawFastVLine(x, y-1-b+8, b+1, WHITE);
				else  // ovr _
					d.drawFastHLine(x, y+8, b+1, WHITE);
			}
		}
		++tBlnk;  // blink cur
		if (tBlnk > cBlnk)  tBlnk = 0;
	}
	d.setFont(0);
	
	
	if (tInfo < 0)  // trigger
		tInfo = 400;  // par
	
	if (tInfo > 0)  //  info eeprom
	{	--tInfo;
		int x = 90;

		d.setFont(0);
		d.setCursor(x, 0);
		const static char* strInf[5] = {
			"Reset", "Loaded", "Saved:", "Copied", "Pasted" };
		d.print(strInf[infType]);
		if (infType == 1 || infType == 2)
		{
			d.setCursor(x+6, 8);
			d.print(memSize);  d.print(" B");
	}	}
}
