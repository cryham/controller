#include "gui.h"

//#ifdef KII
extern "C" {
	#include <scan_loop.h>
	#include <macro.h>
	#include <output_com.h>
	#include <usb_hid.h>
}
//#endif


///-----------------  //par  demo counts
const uint8_t Gui::YM2[MAll] = { iPage, 6, 4, 4 };


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
	
	leds = 0;
	help = 0;  hpage = 0;  status = 0;
	menu = 0;  edit = 0;
	slot = 0;  page = 0;  edpos = 0;

	Clear();  tInfo = 0;
}

void Gui::LedsInit()
{
	leds = 1;  // LEDs init
	pinMode(14, OUTPUT);  //L blue led
	pinMode(26, OUTPUT);  //R orange
	digitalWrite(14, LOW);
	digitalWrite(26, LOW);
}

void Gui::Clear()
{
	memSize = 0;
	memset(seql,0,sizeof(seql));
	memset(seq,0,sizeof(seq));
	infType = 0;  tInfo = -1;
}


//  load, save seq in eeprom
//.............................................
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

	
//  seqence execute
//.............................................
void Gui::ExecSeqs()
{	
	if (menu || id_seq < 0)
		return;

	//  output sequence to usb  ----
	int q = id_seq, n;
	if (q >= 0 && q < iSlots && seql[q])
	{
		for (n=0; n < seql[q]; ++n)
		{
			uint8_t k = seq[q][n];  int m = k - KEY_LCTRL;
			if (m >= 0 && m <= 8)
			{	//  modifier press only
				Output_usbCodeSend_capability(1, 0, &k);  Output_send();
			}else
			{	//  key press and release
				Output_usbCodeSend_capability(1, 0, &k);  Output_send();  k = 0;
				Output_usbCodeSend_capability(1, 0, &k);  Output_send();
			}
			
			//  last
			if (n == seql[q]-1)
			{	// check if modifiers are left, send clear k=0..
			}
		}
	}
	id_seq = -1;  // done
	//todo: shift etc ed, display uppercase..
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
	
	if (leds)  // LEDs update
	{	digitalWrite(14, lay2 ? HIGH : LOW);
		digitalWrite(26, lay3 ? HIGH : LOW);
	}

	if (!menu)
		return;
	if (ym == MGames && mlevel > 0)
		return;

	#define KEY_EDIT  KEYPAD_ENTER
	if (!help && !status && ym == MSeq && mlevel > 0)
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
			{	Save();  tInfo = -1;
			}
			if (kk[KEY_BACKSPACE] && !kko[KEY_BACKSPACE])  // load
			{	Load();  tInfo = -1;
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

	if (!edit)  //  global
	{
		if (kk[KEY_F1] && !kko[KEY_F1] ||
			kk[KEY_SCROLL_LOCK] && !kko[KEY_SCROLL_LOCK])
		{	help = 1-help;  if (help)  status = 0;  }  // H

		if (kk[KEY_F2] && !kko[KEY_F2] ||
			kk[KEY_PAUSE] && !kko[KEY_PAUSE])
		{	status = 1-status;  if (status)  help = 0;  }  // L

		if (help || status)  // <back
		if (kk[KEY_LEFT] && !kko[KEY_LEFT] ||
			kk[KEY_ESC] && !kko[KEY_ESC])
		{	help = 0;  status = 0;  }
	}

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
