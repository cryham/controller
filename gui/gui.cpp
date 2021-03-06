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
	slot = 0;  page = 0;
	edpos = 0;  edins = 1;

	Clear();  tInfo = 0;  tBlnk = 0;
	sql = 0;
	memset(sq,0,sizeof(sq));
}

void Gui::LedsInit()
{
	edins = 1;
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
//  offset and max size to use
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


///  seqence execute
//.............................................
void Gui::ExecSeqs()
{	
	if (menu || id_seq < 0)
		return;

	//  output sequence to usb  ----
	int q = id_seq, n;
	
	bool cmd = false;
	uint32_t del = 0;  // send delay ms
	
	if (q >= 0 && q < iSlots && seql[q])
	{
		for (n=0; n < seql[q]; ++n)
		{
			uint8_t k = seq[q][n];  int m = k - KEY_LCTRL;
			
			//  command, not a key
			//  next byte is digit for value
			if (k == 1 || k == 2)
			{	cmd = true;  continue;
			}
			if (cmd)
			{	cmd = false;

				//  delay ms
				const static uint16_t delTim[10] =
				//1 2  .3	4	5  .6	 7	  8	  .9     0
				{0, 5, 10, 20, 50, 100, 200, 500, 1000, 2000};
				
				int i = k-KEY_1;
				if (i<0) i=0;  if(i>9) i=9;
				
				if (k==1)  //  set delay
				{	del = delTim[i];
					if (del > 0)  delay(del);  //wait
				}
				else  //  wait only
					delay(delTim[i]);

				continue;
			}

			if (m >= 0 && m <= 8)
			{
				//  modifier press only
				Output_usbCodeSend_capability(0, 1, 0, &k);  Output_send();
				if (del > 0)  delay(del);
			}else
			{	//  key press and release
				Output_usbCodeSend_capability(0, 1, 0, &k);  Output_send();  k = 0;
				if (del > 0)  delay(del);
				
				Output_usbCodeSend_capability(0, 1, 0, &k);  Output_send();
				if (USBKeys_Protocol == 0)
					USBKeys_Modifiers = 0;  //fix boot mode?
				if (del > 0)  delay(del);
			}
			
			//if (n == seql[q]-1)  // last
			// check if modifiers are left, send clear k=0..
		}
	}
	id_seq = -1;  // done
	//todo: shift etc ed, display uppercase..
}

int Gui::seqId()
{
	return slot + page*iPage;
}

void Gui::SeqClear(int8_t q)
{
	edpos = 0;
	seql[q] = 0;
	for (int n=0; n < iSeqLen; ++n)
		seq[q][n] = 0;
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
		int q = seqId();
		if (edit)
		{			//  edit sequence  ----
			uint8_t edkey = 0;  // none
			if (lay2)
			{	//  move cursor  //todo: auto repeat
				if (key(HOME) || key(PAGE_UP))    edpos = 0;
				if (key(END)  || key(PAGE_DOWN))  edpos = seql[q];
				
				if (key(UP)   || key(LEFT))   if (edpos > 0)  --edpos;
				if (key(DOWN) || key(RIGHT))  if (edpos < seql[q])  ++edpos;
				
				if (key(DELETE) || keyp(5))
				if (seql[q] > 0)
				{
				#if 0
					int i = edpos;  // del>
				#else
					int i = max(0, edpos-1);  // <del
					edpos = i;  //
				#endif
					for (; i < seql[q]; ++i)
						seq[q][i] = seq[q][i+1];
					--seql[q];
					if (edpos > seql[q])
						edpos = seql[q];
				}
				if (key(INSERT) || keyp(PLUS))
					edins = 1 - edins;  // ins/ovr
				if (key(ENTER) ||
					key(BACKSPACE))  SeqClear(q);  // erase
				
				if (key(F1))  edkey = 1;  // set delay cmd
				if (key(F2))  edkey = 2;  // wait cmd
			}
			if (!lay2 || edkey > 0)
			{
				//  find key, if none
				uint8_t k = 3;
				while (edkey==0 && k < 0xF0)
				{
					if (kk[k] && !kko[k] && k != KEY_EDIT)
						edkey = k;
					else  ++k;
				}
				//  add key to sequence
				if (edkey > 0 && edpos < iSeqLen-1)
				if (edpos == seql[q])  // at end
				{
					seq[q][edpos] = edkey;
					edpos++;  seql[q]++;
				}else
				if (edins)  // insert
				{
					for (int i=seql[q]; i > edpos; --i)
						seq[q][i] = seq[q][i-1];
					seq[q][edpos] = edkey;
					edpos++;  seql[q]++;
				}
				else  // overwrite
				{	seq[q][edpos] = edkey;
					if (edpos < seql[q])  ++edpos;
				}
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
			
			if (key(C))  // copy
			{	sql = seql[q];
				for (int i=0; i < sql; ++i)  sq[i] = seq[q][i];
				infType = 3;  tInfo = -1;
			}
			//  paste, set
			if (key(Z) || key(V))
			if (sql < iSeqLen)
			{	seql[q] = sql;
				for (int i=0; i < sql; ++i)  seq[q][i] = sq[i];
				infType = 4;  tInfo = -1;
			}
		}

		if (key(EDIT))  //  toggle edit  ----
		{
			edit = 1-edit;
			if (edit)  // enter edit
			{
				int q = seqId();
				//if (edpos > seql[q])  // if
					edpos = seql[q];
			}
		}
	}

	if (!edit)  //  global  --------
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
