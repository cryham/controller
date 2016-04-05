#include "gui.h"
#define PROGMEM
#include "FreeSans9pt7b.h"

//#ifdef KII
extern "C" {
	#include <macro.h>
	#include <Output/pjrcUSB/arm/usb_mouse.h>
}
//#endif

///  string from usb hid code
const char str[0x73][6] = { ".",".",".",".", /*04*/"A","B","C","D",
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
#define STR(k)  (k >= 0xE0 ? mod[k-0xE0] : k <= 0x73 ? str[k] : "()")


//  Draw, settings
//....................................................................................
const static char ar[2]={16,0};  //>

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
		d.setCursor(W-1-3*6, 0);
		d.print(hpage+1);  d.print("/");  d.print(HAll);

		d.setCursor(W-1-13*6, 8);  // page titles
		const static char* title[HAll] = {" ", " Toggle", " Menu", "Sequences", " Demos", " Demos cd"};
		d.print(title[hpage]);

		switch (hpage)
		{
		case 0:  // help
			d.setCursor(0, 16+8);
			d.println("H F1  Help");  //> 26  < 27  v 25  ^ 24
			d.moveCursor(0,4);
			d.print(char(24));  d.print(char(25));  d.println("  Prev/Next Page");
			d.moveCursor(0,4);
			d.print(char(27));  d.println(" Esc  Back");
			break;

		case 1:  // help
			d.setCursor(0, 16+8);
			d.println("L F2  Status");
			d.moveCursor(0,8);
			d.println("D Spc  Dim display");
			break;

		case 2:  // menu
			d.setCursor(0, 16+8);
			d.print(char(24));  d.print(char(25));  d.print("  Cursor ");  d.println(char(16));
			d.moveCursor(0,2);
			d.print(char(26));  d.println("  Enter");
			d.moveCursor(0,2);
			d.print(char(27));  d.println("  Back");
			d.moveCursor(0,8);
			break;

		case 3:  // sequence
			d.setCursor(0, 16+2);
			d.print(char(24));  d.print(char(25));  d.println("  Cursor");
			d.println("PgUp,PgDn  Page");
			d.moveCursor(0,2);
			d.println("Num Enter - Edit");
			d.moveCursor(0,4);
			d.println("S Ins   Save");
			d.println("BckSpc  Load");
			break;

		case 4:  // demos
			d.setCursor(0, 16+2);
			d.print(char(24));  d.print(char(25));  d.println("  Prev/Next");
			d.moveCursor(0,2);
			d.println("Num+ F  Toggle Fps");
			d.moveCursor(0,4);
			d.println("R  Reset all params");
			d.println("Num- Esc  Params Info");
			d.println("  Ctrl  Mode,Off");
			break;

		case 5:  // demos cd
			d.setCursor(0, 16+8);
			d.println("PgUp,PgDn  Change");
			d.println("Home,End   Params");
			d.println("  Ctrl  Others");
			d.println("  Shift  Fine");
			break;
		}
		status = 0;  //
		return;
	}
	
	
	if (status)  //  status info
	{
		d.print("Status");  d.setFont(0);

		d.setCursor(0, 16+2);
		d.print("Protocol: ");  d.print(USBKeys_Protocol);
		d.println(USBKeys_Protocol == 1? " NKRO": " Boot");
		d.moveCursor(0,4);

		//  layers stack  - -
		//d.setCursor(0, 16+8*2);
		d.print("Layers: ");  //d.moveCursor(4, 0);
		for (int l=1; l < layersCnt; ++l)  // 1st is 1 in menu
		{
			d.print(layersOn[l]);  d.moveCursor(6/*3*/,0);
		}
		
		///  dbg  mouse speed  --
		d.setCursor(0, H-1-2*8);  d.print(mx_holdtime);
		d.setCursor(0, H-1-1*8);  d.print(my_holdtime);
		
		d.setCursor(W/3, H-2*8);  d.print(mx_delay);
		d.setCursor(W/3, H-1*8);  d.print(my_delay);
		
		d.setCursor(2*W/3, H-2*8);  d.print(mx_speed);
		d.setCursor(2*W/3, H-1*8);  d.print(my_speed);

		return;
	}
	
	
	if (mlevel==0)  //  main menu
	{
		d.print("Menu");  d.setFont(0);

		d.setCursor(0, 16);
		for (int i=0; i < MAll; ++i)
		{
			d.print(i==ym ? ar:" ");
			switch (i)
			{
				case MSeq:    d.println("Sequences");  break;
				case MDemos:  d.println("Demos");  break;
				case MText:   d.println("Text");  break;
				case MPlasma: d.println("Plasma");  break;
			}
		}
		d.setCursor(W-1-7*6, H-8);
		d.print("F1 Help");
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
			d.setCursor(0, 16 + i*8);
			//d.print(i==slot ? ar:" ");   d.print(s);  d.print(": ");
			d.print(s);  d.print(i==slot ? ar:" ");
			
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
					d.moveCursor(2,0);
					++n;
			}	}
		}
		//  page, center   / 
		d.setCursor(58, 4);
		d.print(page+1);  d.print("/");  d.print(iSlots/iPage);
	}
	else
	{
		d.print("Edit");  d.setFont(0);
		int q = slot + page*iPage;
		d.setCursor(104, 4);  d.print(q);

		//  write sequence  ---
		d.setCursor(0, 24);
		int n;
		for (n=0; n < seql[q]; ++n)
		{
			uint8_t z = seq[q][n];
			d.print(STR(z));
			d.print(" ");
			//d.print("(");  d.print(seq[q][n],16);  d.print(") ");
		}
	}
	d.setFont(0);
	
	
	if (tInfo < 0)  // trigger
		tInfo = 400;  // par
	
	if (tInfo > 0)  //  info eeprom
	{	--tInfo;
		int x = 90;

		d.setFont(0);
		d.setCursor(x, 0);
		d.print(infType==2 ? "Saved:" :
				infType==1 ? "Loaded:" : "Reset");
		if (infType > 0)
		{
			d.setCursor(x+6, 8);
			d.print(memSize);  d.print(" B");
	}	}
}
