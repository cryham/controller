#include "gui.h"
#define PROGMEM
#include "FreeSans9pt7b.h"
//#include "FreeSans12pt7b.h"

//#ifdef KII
extern "C" {
	#include <scan_loop.h>
	#include <macro.h>
	#include <output_com.h>
	#include <usb_hid.h>
}
//#endif

//  string from usb hid code
const char str[0xF0][6] = { ".",".",".",".", /*04*/"A","B","C","D",
/*08*/"E","F","G","H","I","J","K","L", /*10*/"M","N","O","P","Q","R","S","T",
/*18*/"U","V","W","X","Y","Z","1","2", /*20*/"3","4","5","6","7","8","9","0",
/*28*/"Enter","Esc","BckSp","Tab","Space",
/*2D*/"-","=","[","]","\\", /*32*/"NUM-",";","\"","`",",",".", "/","CapsL",
/*3A*/"F1","F2","F3","F4", "F5", "F6",
/*40*/"F7","F8","F9","F10","F11","F12", /*46*/"PtrSc","ScrL","Pause",
/*49*/"Ins","Home","PgUp", "Del","End","PgDn", //*4F*/"Right","Left","Down","Up",
/*4F*/{26,0},{27,0},{25,0},{24,0},
/*53*/"NumL","P/","P*","P-","P+","PEnt",
/*59*/"P1","P2","P3","P4","P5","P6","P7","P8","P9","P0","P.",
/*68*/"F13","F14","F15","F16","F17","F18","F19","F20","F21","F22","F23","F24",
/*E0*/ "Ctr", "Shi", "Alt", "Gui",
//*E4*/"LCtr","LShi","LAlt","LGui"
/*E4*/"RCtr","RShi","RAlt","RGui",};


//  Main  ----

Gui::Gui()
{
	Init();
}
void Gui::Init()
{
	menu = 0;
	slot = 0;  page = 0;
	edit = 0;  edpos = 0;

	iInfo = 0;  memSize = 0;
	//eeprom_initialize();

	//for (int i=0; i<iSlots; ++i)
	//	seql[i] = 0;
	//  test
	seql[0]=2;
	seq[0][0]=KEY_1;  seq[0][1]=KEY_2;
	seql[1]=3;
	seq[1][0]=KEY_A;  seq[1][1]=KEY_B;  seq[1][2]=KEY_C;
	seql[2]=1;
	seq[2][0]=KEY_X;
}

void Gui::Clear()
{
	memSize = 0;
	memset(seql,0,sizeof(seql));
	memset(seq,0,sizeof(seq));
	infType = 0;
}

//  load, save seq in eeprom
// ........................................
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
	for (i=0; i < iSlots; ++i)  //if (seql[i])
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
// ....................................................................................
void Gui::Draw(Adafruit_SSD1306& d)
{
	if (!menu)  return;

	d.setFont(&FreeSans9pt7b);
	d.setCursor(0, 0);
	//d.drawPixel(0,0,1);  d.drawPixel(64,0,1);  d.drawPixel(127,0,1);
	//d.drawPixel(0,63,1); d.drawPixel(127,63,1);
	if (!edit)
	{
		d.print("View");  d.setFont(0);
		
		//  list slots
		int s = page * iPage, i,n,x, xm;
		for (i=0; i < iPage && s < iSlots; ++i,++s)
		{
			d.setCursor(0, 16 + i*8);
			const char ar[2]={16,0};  //>
			//d.print(i==slot ? ar:" ");
			//d.print(s);  d.print(": ");
			d.print(s);
			d.print(i==slot ? ar:" ");
			
			//  write sequence
			n=0;  x=0;  xm=1;
			#define W 128
			#define xmax  W-6*5
			while (n < seql[s] && xm)
			{
				const char* st = str[seq[s][n]];
				if (d.getCursorX() + 6*strlen(st) >= W -6*2)  // outside
				{	d.print("..");  xm = 0;  // more sign
				}else
				{	d.print(st);  ++n;  }
			}
		}
		//  page, center
		d.setCursor(58, 4);
		d.print(page+1);  d.print("/");
		d.print(iSlots/iPage);
	}
	else
	{	d.print("Edit");  d.setFont(0);
		d.setCursor(80,4);
		int q = slot + page*iPage;
		d.print("Slot:");  d.print(q);

		//  write sequence
		d.setCursor(0,16);
		int n;
		for (n=0; n < seql[q]; ++n)
		{
			d.print(str[seq[q][n]]);  //d.print(" ");
		}
	}
	
	if (iInfo > 0)  //  info ee
	{	--iInfo;
		int x = 90;

		d.setCursor(x,0);
		d.print(infType==2 ? "Saved:" :
				infType==1 ? "Load:" : "Reset");
		if (infType > 0)
		{
			d.setCursor(x+12,8);
			d.print(memSize);
			d.print(" B");
	}	}
}

//  Key press
// ....................................................................................
void Gui::KeyPress()
{
	if (kk[KEY_A] && !kko[KEY_A])
	{	menu = 1-menu;  return;  }
	
	#define KEY_EDIT  KEY_RIGHT
	if (menu)
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
		}
		else
		{
			if (kk[KEY_LEFT] && !kko[KEY_LEFT])  //  next pages
			{
				++page;
				if (page >= iSlots/iPage)  page = 0;
			}
			if (kk[KEY_B] && !kko[KEY_B])  // save
			{
				Save();  iInfo = 400;
			}
	
			if (kk[KEY_DOWN] && !kko[KEY_DOWN])
			{	slot++;  if (slot >= iPage)  slot = 0;  }
			if (kk[KEY_UP] && !kko[KEY_UP])
			{	slot--;  if (slot < 0)  slot = iPage-1;  }
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
	else  //  seqence execute  -----
	{
		#if 1
		int q = -1;
		for (int i=0; i < 10; ++i)
			if (kk[KEY_1+i] && !kko[KEY_1+i])
				q = i;
		if (kk[KEY_B] && !kko[KEY_B])
			q = 0;
		if (kk[KEY_UP] && !kko[KEY_UP])
			q = 1;

		//  output sequence to usb
		if (q>=0 && seql[q])
		{
			int8_t sh=0,ct=0,al=0;  // modifiers are toggleable
			for (int n=0; n < seql[q]; ++n)
			{
				uint8_t k = seq[q][n];
				//  modifier press  or release when 2nd time
				if (k==KEY_CTRL || k==KEY_RCTRL)
				{	ct = 1-ct;  if (ct==0)  k = 0;  Output_usbCodeSend_capability(1, 0, &k);  Output_send();  }
				else if (k==KEY_SHIFT || k==KEY_RSHIFT)
				{	sh = 1-sh;  if (sh==0)  k = 0;  Output_usbCodeSend_capability(1, 0, &k);  Output_send();  }
				else if (k==KEY_ALT || k==KEY_RALT)
				{	al = 1-al;  if (al==0)  k = 0;  Output_usbCodeSend_capability(1, 0, &k);  Output_send();  }
				else
				{	// key press and release
					Output_usbCodeSend_capability(1, 0, &k);  Output_send();  k = 0;
					Output_usbCodeSend_capability(1, 0, &k);  Output_send();
				}
			}
			//todo? move use to scan.c
			// shift etc ed, display uppercase,
		}
		#endif
	}
}
