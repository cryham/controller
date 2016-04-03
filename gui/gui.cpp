#include "gui.h"
#define PROGMEM
#include "FreeSans9pt7b.h"

//#ifdef KII
extern "C" {
	#include <scan_loop.h>
	#include <macro.h>
	#include <output_com.h>
	#include <usb_hid.h>
	#include <Output/pjrcUSB/arm/usb_mouse.h>
}
//#endif
#define W SSD1306_LCDWIDTH
#define H SSD1306_LCDHEIGHT

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

///-----------------  //par  demo counts
const uint8_t Gui::YM2[MAll] = { iPage, 9, 4, 4 };


//  Main  ----
Gui::Gui()
{
	Init();
}
void Gui::Init()
{
	mlevel = 0;  ym = 0;
	for (int i=0; i < MAll; ++i)
		ym2[i]=0;

	help = 0;  hpage = 0;
	menu = 0;  edit = 0;
	slot = 0;  page = 0;  edpos = 0;

	iInfo = 0;  memSize = 0;
	Clear();
	iLayers = 0;
}
void Gui::Clear()
{
	memSize = 0;
	memset(seql,0,sizeof(seql));
	memset(seq,0,sizeof(seq));
	infType = 0;
}


//  load, save seq in eeprom
//........................................
#define EOfs 0
#define ESize 2048

void Gui::Load()  //  Load
{
	Clear();
	int i,n,a;  uint8_t b, er=0;
	//  lengths
	a = EOfs;
	for (i=0; i < iSlots; ++i,++a)
	{	b = eeprom_read_byte((uint8_t*)a);
		seql[i] = b < iSeqLen ? b : 0;
		if (b >= iSeqLen)  ++er;
	}
	//  error, not inited
	if (er > 0)
	{	Clear();  return;  }
	
	//  data
	a = EOfs + iSlots;
	for (i=0; i < iSlots; ++i)
	for (n=0; n < seql[i] && a < ESize; ++n)
	{
		seq[i][n] = eeprom_read_byte((uint8_t*)a);  ++a;
	}
	memSize = a;  infType = 1;
}

void Gui::Save()  //  Save
{
	int i,n,a;
	//  lengths
	a = 0;  // ofs
	for (i=0; i < iSlots; ++i,++a)
		eeprom_write_byte((uint8_t*)a, seql[i]);

	//  data
	a = EOfs + iSlots;
	for (i=0; i < iSlots; ++i)  //if (seql[i])
	for (n=0; n < seql[i] && a < ESize; ++n)
	{
		eeprom_write_byte((uint8_t*)a, seq[i][n]);  ++a;
	}
	memSize = a;  infType = 2;
}


//  Draw, settings
//....................................................................................
const static char ar[2]={16,0};  //>

void Gui::Draw(Adafruit_SSD1306& d)
{
	if (!menu)  return;

	if (mlevel==0 || mlevel==1 && ym == MSeq || iInfo > 0)
	{
		d.clearDisplay();
		d.setFont(&FreeSans9pt7b);
		d.setCursor(0,0);
	}
	
	if (help)
	{
		d.print("Help");  d.setFont(0);
		//d.setCursor(W-1-8*6, H-8);
		//d.print("Page: ");  d.print(hpage);
		d.setCursor(W-1-3*6, 0);
		d.print(hpage+1);  d.print("/");  d.print(HAll);

		d.setCursor(W-1-13*6, 8);  // page titles
		const static char* title[HAll] = {" ", " Menu", "Sequences", " Demos"};
		d.print(title[hpage]);

		switch (hpage)
		{
		case 0:  // help
			d.setCursor(0, 16+8);
			d.println("H F1  Show/Hide");  //> 26  < 27  v 25  ^ 24
			d.print(char(24));  d.print(char(25));  d.println("  Prev/Next Page");
			d.moveCursor(0,8);
			d.println("D Spc  Toggle Dim");
			break;
		case 1:  // menu
			d.setCursor(0, 16+8);
			d.print(char(24));  d.print(char(25));  d.print("  Cursor ");  d.println(char(16));
			d.print(char(26));  d.println("  Enter");
			d.print(char(27));  d.println("  Back");
			d.moveCursor(0,8);
			d.print("L F2  Layer stack");
			break;
		case 2:  // sequence
			d.setCursor(0, 16+2);
			d.print(char(24));  d.print(char(25));  d.println("  Cursor");
			d.println("PgDn,PgUp  Page");
			d.moveCursor(0,3);
			d.println("Num Enter - Edit");
			d.moveCursor(0,3);
			d.println("S Ins   Save");
			d.println("BckSpc  Load");
			break;
		case 3:  // demos
			d.setCursor(0, 16+2);
			d.print(char(24));  d.print(char(25));  d.println("  Prev/Next");
			d.println("End F  Toggle Fps");
			d.moveCursor(0,4);
			d.println("PgDn,PgUp,Home");
			d.println("  Optional in");
			d.println("  Hedrons3D, Plasma");
			break;
		}
		return;
	}

	if (mlevel==0)
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

		//  info layers stack  - -
		if (iLayers)
		{	
			//d.setCursor(0,H-16);  // mouse speed
			//d.println(delay_cur);

			d.setCursor(0, H-8);
			d.print("L:");  d.moveCursor(4, 0);
			for (int l=1; l < layersCnt; ++l)  // 1st is 1 in menu
			{
				d.print(layersOn[l]);  d.moveCursor(3,0);
		}	}
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
			d.print(s+1);  d.print(i==slot ? ar:" ");
			
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
	{	d.print("Edit");  d.setFont(0);
		int q = slot + page*iPage;
		d.setCursor(104, 4);  d.print(q+1);

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

	if (iInfo > 0)  //  info eeprom
	{	--iInfo;
		int x = 90;

		d.setCursor(x, 0);
		d.print(infType==2 ? "Saved:" :
				infType==1 ? "Load:" : "Reset");
		if (infType > 0)
		{
			d.setCursor(x+6, 8);
			d.print(memSize);  d.print(" B");
	}	}
}


//  Key press
//....................................................................................
void Gui::KeyPress()
{
	//  Menu = Layer1, Function1
	menu = 0;  int8_t lay2 = 0, lay3 = 0;
	for (int l=0; l < layersCnt; ++l)
		if (layersOn[l] == 1)  menu = 1; else
		if (layersOn[l] == 2)  lay2 = 1; else
		if (layersOn[l] == 3)  lay3 = 1;
	
	#if 1  // LEDs
	digitalWrite(14, lay2 ? HIGH : LOW);
	digitalWrite(26, lay3 ? HIGH : LOW);
	#endif

	if (menu)
	{
		#define KEY_EDIT  KEYPAD_ENTER
		if (!help && ym == MSeq && mlevel > 0)
		{
			if (edit)
			{
				//  find key
				uint8_t k = 3, edkey = 0;
				while (edkey==0 && k < 0xF0)
				{
					if (kk[k] && !kko[k] && k != KEY_EDIT)
						edkey = k;
					else  ++k;
				}
				//  add to seq
				if (edkey > 0 && edpos < iSeqLen)
				{	int q = slot + page*iPage;
					seq[q][edpos] = edkey;
					edpos++;  seql[q]++;
				}
			}else
			{	if (kk[KEY_S] && !kko[KEY_S] ||  // save
					kk[KEY_INSERT] && !kko[KEY_INSERT])
				{	Save();  iInfo = 400;
				}
				if (kk[KEY_BACKSPACE] && !kko[KEY_BACKSPACE])  // load
				{	Load();  iInfo = 400;
				}
				
				if (kk[KEY_DOWN] && !kko[KEY_DOWN])  // move
				{	++slot;  if (slot >= iPage) {  slot = 0;
					++page;  if (page >= iSlots/iPage)  page = 0;
				}	}
				if (kk[KEY_UP] && !kko[KEY_UP])
				{	--slot;  if (slot < 0) {  slot = iPage-1;
					--page;  if (page < 0)  page = iSlots/iPage-1;
				}	}
				if (kk[KEY_PAGE_DOWN] && !kko[KEY_PAGE_DOWN])  // page
				{	++page;  if (page >= iSlots/iPage)  page = 0;
				}
				if (kk[KEY_PAGE_UP] && !kko[KEY_PAGE_UP])
				{	--page;  if (page < 0)  page = iSlots/iPage-1;
				}
			}

			if (kk[KEY_EDIT] && !kko[KEY_EDIT])  //  toggle edit
			{
				edit = 1-edit;
				if (edit)  // enter edit
				{
					int q = slot + page*iPage;
					//  clear
					edpos = 0;  //todo append
					seql[q] = 0;
					for (int n=0; n < iSeqLen; ++n)
						seq[q][n] = 0;
				}
			}
		}

		if (!edit)
		if (kk[KEY_H] && !kko[KEY_H] || kk[KEY_F1] && !kko[KEY_F1])
			help = 1-help;  // H  global

		if (help)
		{
			if (kk[KEY_DOWN] && !kko[KEY_DOWN])  // pages
			{	++hpage;  if (hpage >= HAll)  hpage = 0;  }
			if (kk[KEY_UP] && !kko[KEY_UP])
			{	--hpage;  if (hpage < 0)  hpage = HAll-1;  }
		}
		else
		if (mlevel==0)  //  main
		{
			if (kk[KEY_DOWN] && !kko[KEY_DOWN])
			{	++ym;  if (ym >= MAll)  ym = 0;  }
			if (kk[KEY_UP] && !kko[KEY_UP])
			{	--ym;  if (ym < 0)  ym = MAll-1;  }
			
			if (kk[KEY_RIGHT] && !kko[KEY_RIGHT])
				mlevel = 1;  // enter>

			if (kk[KEY_L] && !kko[KEY_L] || kk[KEY_F2] && !kko[KEY_F2])
				iLayers = 1-iLayers;  // L
		}else
		if (!edit)  // other
		{
			if (kk[KEY_LEFT] && !kko[KEY_LEFT])
				mlevel = 0;  // <back

			if (kk[KEY_DOWN] && !kko[KEY_DOWN])  // navigate
			{	ym2[ym]++;  if (ym2[ym] >= YM2[ym])  ym2[ym] = 0;  }
			if (kk[KEY_UP] && !kko[KEY_UP])
			{	ym2[ym]--;  if (ym2[ym] < 0)  ym2[ym] = YM2[ym]-1;  }
		}
	}
}
	
	
//  seqence execute  -----
void Gui::ExecSeqs()
{	
	if (menu || id_seq < 0)
		return;

	//  output sequence to usb  ----
	int q = id_seq, i, n;
	if (q >= 0 && q < iSlots && seql[q])
	{
		int8_t md[8];  // modifiers state, toggleable
		for (n=0; n < 8; ++n)  md[n]=0;
		
		for (n=0; n < seql[q]; ++n)
		{
			uint8_t k = seq[q][n], m = k-KEY_LCTRL;
			//  modifier press  or release when 2nd time
			if (m >= 0 && m < 8)
			{	md[m]  = 1 - md[m];  if (md[m]==0)  k = 0;
				Output_usbCodeSend_capability(1, 0, &k);  Output_send();  }
			else
			{	// key press and release
				Output_usbCodeSend_capability(1, 0, &k);  Output_send();  k = 0;
				Output_usbCodeSend_capability(1, 0, &k);  Output_send();
			}
			
			//  last
			if (n == seql[q]-1)
			{	// check if modifiers are left, send clear
				//..
			}
		}
	}
	id_seq = -1;  // done
	//todo: shift etc ed, display uppercase..
}
