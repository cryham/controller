#pragma once
#include <stdint.h>
#include <Adafruit_SSD1306.h>

#define W SSD1306_LCDWIDTH  // area
#define H SSD1306_LCDHEIGHT

enum EMenu {  // main menu entries, level1
	MSeq=0, MDemos, MText, MPlasma, MGames, MAll  };


struct Gui
{
	//  main
	Gui();
	void Init(), LedsInit();
	void Draw(Adafruit_SSD1306& display);
	void KeyPress(), ExecSeqs();

	//  const
	const static uint8_t 
		iSlots = 20,  // MSeq:  ver, changes eeprom
		iSeqLen = 60, //   max bytes+1 for 1 sequence (const in ram, var in eeprom)
		iPage = 5,    //   page size, view at once
		HAll = 11,    // Help pages
		StAll = 4,    // Status pages
		cBlnk = 250,  //   cursor blink rate
		YM2[MAll];    // demo counts

	//  vars
	int8_t leds;  // use kbd LEDs, for layers info
	int8_t help, hpage;  // shows help, on top, page
	int8_t status, stpage;  // status info pages
	
	inline int DrawDemo()
	{	return menu && mlevel > 0 && !help && !status;  }
	inline int IsGame()
	{	return DrawDemo() && ym == MGames;  }
	
	int8_t menu;   // set when on Layer 1, if 0 nothing drawn
	int8_t mlevel;
	int8_t ym;     // EMenu, main1 page y
	int8_t ym2[MAll];  // level2 page,  cur demo for each menu
	
	//  MSeq  ----  seq edit vars
	int8_t edit;   // seq 0 view / 1 edit
	int8_t edins;  // 1 ins 0 overwrite
	int8_t slot, page, edpos;  // edit vars
	int seqId();
	void SeqClear(int8_t q);
	uint8_t tBlnk;  // cur blink anim
	
	int16_t tInfo, infType, memSize;  // info text vars

	//  seq  ----
	uint8_t seql[iSlots];  // lengths of each seq, 0 empty
	uint8_t seq[iSlots][iSeqLen];  // sequence data
	
	//  for copy
	uint8_t sql;
	uint8_t sq[iSeqLen];

	void Clear();  // ram
	void Load(), Save();  // eeprom
};
