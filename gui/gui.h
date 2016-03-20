#pragma once
#include <stdint.h>
#include <Adafruit_SSD1306.h>

struct Demos;

struct Gui
{
	Gui(Demos* d);
	Demos* demos;  // needed by keys

	//  main
	void Init();
	void Draw(Adafruit_SSD1306& display);
	void KeyPress();

	//  vars
	const static int iSlots = 18;  //ver changes eeprom
	const static int iSeqLen = 40;  //max bytes for 1 sequence (const in ram, var in eeprom)
	const static int iPage = 6;  // view at once
	int8_t menu, slot, page, edit, edpos;

	int16_t iInfo, infType, memSize;  // info text vars

	//  seq
	uint8_t seql[iSlots];  // lengths of each seq, 0 empty
	uint8_t seq[iSlots][iSeqLen];  // sequence data

	void Clear();  // ram
	void Load(), Save();  // eeprom
};
