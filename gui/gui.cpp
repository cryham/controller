#include "gui.h"

//#ifdef KII
extern "C" {
	#include <scan_loop.h>
	#include <macro.h>
	#include <output_com.h>
	#include <usb_hid.h>

	#include <kll.h>
	extern void Output_usbCodeSend_capability( TriggerMacro *trigger, uint8_t state, uint8_t stateType, uint8_t *args );
}
//#endif


///-----------------  //par  demo counts
const uint8_t Gui::YM2[MAll] = { iPage, 6, 4, 5 };


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
	help = 0;  hpage = 0;
	status = 0;  stpage = 0;
	
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
				Output_usbCodeSend_capability(0, 1, 0, &k);  Output_send();
			}else
			{	//  key press and release
				Output_usbCodeSend_capability(0, 1, 0, &k);  Output_send();  k = 0;
				Output_usbCodeSend_capability(0, 1, 0, &k);  Output_send();
				if (USBKeys_Protocol == 0)
					USBKeys_Modifiers = 0;  //fix boot mode?
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
		{	if (key(S) || key(INSERT))  // save
			{	Save();  tInfo = -1;
			}
			if (key(BACKSPACE))  // load
			{	Load();  tInfo = -1;
			}
			
			if (key(DOWN))  // move
			{	++slot;  if (slot >= iPage) {  slot = 0;
				++page;  if (page >= iSlots/iPage)  page = 0;
			}	}
			if (key(UP))
			{	--slot;  if (slot < 0) {  slot = iPage-1;
				--page;  if (page < 0)  page = iSlots/iPage-1;
			}	}
			if (key(PAGE_DOWN))  // page
			{	++page;  if (page >= iSlots/iPage)  page = 0;
			}
			if (key(PAGE_UP))
			{	--page;  if (page < 0)  page = iSlots/iPage-1;
			}
		}

		if (key(EDIT))  //  toggle edit
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
		if (key(F1) || key(SCROLL_LOCK))
		{	help = 1-help;  if (help)  status = 0;  }  // H

		if (key(F2) || key(PAUSE))
		{	status = 1-status;  if (status)  help = 0;  }  // L

		if (help || status)  // <back
		if (key(LEFT) || key(ESC))
		{	help = 0;  status = 0;  }
		
		if (status)  // status page
		{
			if (key(DOWN))
			{	++stpage;  if (stpage >= StAll)  stpage = 0;  }
			if (key(UP))
			{	--stpage;  if (stpage < 0)  stpage = StAll-1;  }
		}
	}

	if (help)  // help page
	{
		if (key(DOWN))
		{	++hpage;  if (hpage >= HAll)  hpage = 0;  }
		if (key(UP))
		{	--hpage;  if (hpage < 0)  hpage = HAll-1;  }
	}
	else
	if (mlevel==0)  //  main
	{
		if (key(DOWN))
		{	++ym;  if (ym >= MAll)  ym = 0;  }
		if (key(UP))
		{	--ym;  if (ym < 0)  ym = MAll-1;  }
		
		if (key(RIGHT))
			mlevel = 1;  // enter>
	}else
	if (!edit && !status && ym < MGames)  // other
	{
		if (key(LEFT))
			mlevel = 0;  // <back

		if (key(DOWN))  // navigate
		{	ym2[ym]++;  if (ym2[ym] >= YM2[ym])  ym2[ym] = 0;  }
		if (key(UP))
		{	ym2[ym]--;  if (ym2[ym] < 0)  ym2[ym] = YM2[ym]-1;  }
	}
}
