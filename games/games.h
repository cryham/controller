#pragma once
#include <stdint.h>
#include <Adafruit_SSD1306.h>

#define W SSD1306_LCDWIDTH  // area
#define H SSD1306_LCDHEIGHT


struct Games
{
	//  main
	Games();
	void Init();
	void Draw(Adafruit_SSD1306& display);
	void Update(), KeyPress(int8_t& mlevel);
	uint32_t old_ti, dt_sum;
	
	struct Options
	{
		//  grid size
		int8_t size_x, size_y;
		int8_t nx_cur;  // number of next blocks preview
		int8_t btm_junk;  // bottom lines filled at start

		//  block generate
		int8_t glen_min, glen_max, gsize;

		//  fall speed start, accel
		uint32_t speed, accel;
		//  fine
		uint8_t sp_drop, sp_fall;
		uint8_t key_rpt, move_in_drop;
	} o;

	//  grid dim
	int8_t dim_x, dim_y, ofs_x, ofs_y;
	//  move
	int8_t pos_x, pos_y;
	int8_t fall, drop;  // bool
	
	//  speed
	#define SpdDet 16384
	int16_t time_y;
	uint32_t speed_y, time_spd;

	inline void UpdSpeed()
	{	time_spd = 10000 * SpdDet / speed_y;  }
	
	//  grid data  ::
	const static int8_t
		smax_x = 24, smax_y = 24;
	int8_t grid[smax_y][smax_x];
	void NewGrid();

	//  blocks  .
	const static int8_t
		nx_max = 4, bmax = 8;
	struct Block
	{	int8_t b[bmax][bmax];  };
	
int aa[32];
	Block blk, cpy, next[nx_max];
	
	//  block generator
int ab[16];
	int xo, yo, xa,xb,ya,yb;  // debug --

	void GenBlock(Block& b);
	void NewBlock();

	//  utility
	void Draw(Adafruit_SSD1306& d, const Block& b,
		int pos_x, int pos_y, int o_y=0, int dim=0);
	void DrawNext(Adafruit_SSD1306& d, const Block& b,
		int pos_x, int pos_y);
	
	int Overlaps(const Block& b,  // test block overlap in grid
		int pos_x, int pos_y);

	int Copy(Block& to, const Block& from);
	void Rotate(Block& to, const Block& from, int cw=1);
	
	//  global state
	int paused, demo, ended;
	
	const int8_t Presets = 8;
	int preset;  // game type  //todo: all options
	void NewGame();

	//  stats
	int score, lines[bmax], errors;
};
