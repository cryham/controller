#pragma once
#include <stdint.h>
#include <Adafruit_SSD1306.h>

enum EMenu {  // main menu entries, level1
	MSeq=0, MDemos, MText, MPlasma, MAll  };

struct Gui
{
	//  main
	Gui();
	void Init(), LedsInit();
	void Draw(Adafruit_SSD1306& display);
	void KeyPress(), ExecSeqs();

	//  const
	const static uint8_t 
		iSlots = 18,  // MSeq:  ver, changes eeprom
		iSeqLen = 40, //   max bytes for 1 sequence (const in ram, var in eeprom)
		iPage = 6,    //   page size, view at once
		HAll = 4;     // Help: pages
	const static uint8_t YM2[MAll];  // cursor pos in each menu

	//  vars
	int8_t leds;  // use kbd LEDs, for layers info
	int8_t help, hpage;  // shows help, on top, page
	int8_t menu;   // set when on Layer 1, if 0 nothing drawn
	int8_t mlevel;
	int8_t ym;     // EMenu, main1 page y
	int8_t ym2[MAll];  // level2 page
	//  MSeq
	int8_t edit;   // seq 0 view / 1 edit
	int8_t slot, page, edpos;  // edit vars
	
	int16_t iInfo, infType, memSize;  // info text vars
	int8_t iLayers;  // L: show layers stack

	//  seq
	uint8_t seql[iSlots];  // lengths of each seq, 0 empty
	uint8_t seq[iSlots][iSeqLen];  // sequence data

	void Clear();  // ram
	void Load(), Save();  // eeprom
};
