#include <games.h>
extern "C" {
	#include <scan_loop.h>  // keys kk
	#include <usb_hid.h>  // key_defs
}
#define PROGMEM
#include "FreeSans9pt7b.h"


//  main
Games::Games()
{
	old_ti = 0;  dt_sum = 0;
	paused = 0;  demo = 0;  ended = 1;

	Init();
}

void Games::Init()
{
	//  keys
	o.sp_drop = 5;  o.sp_fall = 16;
	o.key_rpt = 27;  o.move_in_drop = 0;
	
	preset = 5;
	xo= 0; yo= 0; xa= 0; ya= 0; xb= 0; yb= 0;

	gui = 1;
	yg=0;  oyg=0;  opg=0;

	NewGrid();
	//NewGame();
}

//  Init
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 
const static char* sPresets[Games::Presets] = {
	"Tiny 3", "Basic 3/4", "Simple 3/2", "Tetris 4", "Pentis 5/1", "Pentis 5/4", "Sixtis 6", "Septis 7"};
	
void Games::NewGrid()
{
	o.nx_cur = 3;  o.dots = 0;  o.frame = 2;  o.bbias = 2;
	switch (preset)
	//  H 64  sy dim  21 3 .. 16 4 .. 12 5 .. 10 6
	{
	case 0:  // tiny
		o.size_x = 3;  o.size_y = 10;  o.btm_junk = 2;  o.dots = 0;  o.frame = 0;
		o.blen_min = 1;  o.blen_max = 4;  o.bsize = 2;  o.bdiag = 4;
		o.speed = 40 * SpdDet;  o.accel = 20;  break;

	case 1:  // basic
		o.size_x = 4;  o.size_y = 10;  o.btm_junk = 1;	o.dots = 0;  o.frame = 0;
		o.blen_min = 1;  o.blen_max = 4;  o.bsize = 3;	o.bdiag = 8;
		o.speed = 30 * SpdDet;  o.accel = 30;  break;

	case 2:  // simple
		o.size_x = 5;  o.size_y = 12;  o.btm_junk = 2;  o.dots = 0;  o.frame = 0;
		o.blen_min = 1;  o.blen_max = 4;  o.bsize = 3;  o.bdiag = 6;
		o.speed = 40 * SpdDet;  o.accel = 40;  break;

	case 3:  // tetris meh
		o.size_x = 9;  o.size_y = 12;  o.btm_junk = 3;  o.dots = 0;  o.frame = 3;
		o.blen_min = 4;  o.blen_max = 4;  o.bsize = 4;  o.bdiag = 4;
		o.speed = 15 * SpdDet;  o.accel = 30;  break;

	case 4:  // pentis
		o.size_x = 11;  o.size_y = 14;  o.btm_junk = 2;  o.dots = 1;  o.frame = 2;
		o.blen_min = 2;  o.blen_max = 5;  o.bsize = 5;  o.bdiag = 5;
		o.speed = 10 * SpdDet;  o.accel = 20;  break;

	case 5:  // pentis diag
		o.size_x = 12;  o.size_y = 16;  o.btm_junk = 1;  o.dots = 2;  o.frame = 2;
		o.blen_min = 2;  o.blen_max = 6;  o.bsize = 5;  o.bdiag = 8;
		o.speed = 8 * SpdDet;  o.accel = 10;  break;

	case 6:  // sixtis
		o.size_x = 13;  o.size_y = 16;  o.btm_junk = 1;  o.dots = 3;  o.frame = 3;
		o.blen_min = 2;  o.blen_max = 8;  o.bsize = 6;  o.bdiag = 4;
		o.speed = 4 * SpdDet;  o.accel = 2;  break;

	case 7:  // septis
		o.size_x = 14;  o.size_y = 21;  o.btm_junk = 0;  o.dots = 3;  o.frame = 3;
		o.blen_min = 1;  o.blen_max = 9;  o.bsize = 7;  o.bdiag = 4;
		o.speed = 1 * SpdDet;  o.accel = 0;  break;
	}
									
	//  box dim
	dim_y = H / o.size_y;  dim_x = dim_y;

	//  center, bottom
	ofs_x = (W - o.size_x*dim_x) / 2;
	ofs_y = (H - o.size_y*dim_y);
	NewGame();
}


void Games::NewBlock()
{
	//  pos center
	pos_x = o.size_x / 2 - o.bsize / 2;
	pos_y = 0;  time_y = 0;
	fall = 0;  drop = 0;

	if (!o.nx_cur)
	{	GenBlock(blk);  return;  }
	
	//  get first
	Copy(blk, next[0]);
	
	//  move next blocks
	for (int n=0; n < o.nx_cur-1; ++n)
		Copy(next[n], next[n+1]);

	//  new last
	GenBlock(next[o.nx_cur-1]);
}

void Games::NewGame()
{
	paused = 0;  demo = 0;  ended = 0;
	score = 0;  errors = 0;
	lines = 0;
		
	speed_y = o.speed;  UpdSpeed();

	//  clear
	int i,x,y;
	for (y=0; y < smax_y; ++y)
	for (x=0; x < smax_x; ++x)
		grid[y][x] = 0;

	Clear(blk);  Clear(cpy);  Clear(prv);
	for (i=0; i < nx_max; ++i)
		Clear(next[i]);
	
	//  fill bottom junk  . ..
	int y0 = o.size_y - o.btm_junk;  y0 = max(y0, o.bsize);
	for (y=y0; y < o.size_y; ++y)
	for (x=0; x < o.size_x; ++x)
		if (random(100) < min(60, (y-y0+1) * 30))
			grid[y][x] = 1;
		
	//  randomize  --
	for (i=0; i < 1323; ++i)
		random(1000);
		
	//  generate all previews
	for (i=0; i < o.nx_cur; ++i)
		GenBlock(next[i]);
	NewBlock();
}


//  Draw block
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 
void Games::Draw(Adafruit_SSD1306& d, const Block& b,
	int pos_x, int pos_y, int o_y, int dim)
{
	if (!dim)
	for (int y=0; y < o.bsize; ++y)
	for (int x=0; x < o.bsize; ++x)
	{
		int yy = (pos_y + y) % o.size_y * dim_y + ofs_y + o_y;
		int xx = (pos_x + x) % o.size_x * dim_x + ofs_x;
		if (b.b[y][x] &&
			yy >= 0 && yy < H)
			d.fillRect(xx, yy, dim_x, dim_y, WHITE);
	}
	else  //  dim, prv
	for (int y=0; y < o.bsize; ++y)
	for (int x=0; x < o.bsize; ++x)
	{
		int yy = (pos_y + y) % o.size_y * dim_y + ofs_y + o_y;
		int xx = (pos_x + x) % o.size_x * dim_x + ofs_x;
		if (b.b[y][x] &&
			yy >= 0 && yy < H)
			for (int j=0; j < dim_y; ++j)
			for (int i=0; i < dim_x; ++i)
				if ((xx+i) % 2 == (yy+j) % 2)
					d.drawPixel(xx + i, yy + j, WHITE);
	}
}
void Games::DrawNext(Adafruit_SSD1306& d, const Block& b,
	int pos_x, int pos_y)
{
	for (int y=0; y < o.bsize; ++y)
	for (int x=0; x < o.bsize; ++x)
	{
		int yy = pos_y + y * dim_y;
		int xx = pos_x + x * dim_x;
		d.drawPixel(xx + dim_x/2, yy + dim_y/2, WHITE);  //.

		if (b.b[y][x] &&
			xx < W && yy < H)
			d.fillRect(xx, yy, dim_x, dim_y, WHITE);
	}
}


//  utility
int Games::Overlaps(const Block& b, int pos_x, int pos_y)
{
	for (int y=0; y < o.bsize; ++y)
	for (int x=0; x < o.bsize; ++x)
	if (b.b[y][x])
	{
		int yy = (pos_y + y) % o.size_y;
		int xx = (pos_x + x) % o.size_x;
		//  sides cycle, no check
		if (pos_y + y >= o.size_y)  return 2;  //  bottom
		if (grid[yy][xx])  return 1;  //  overlaps
	}
	return 0;  // empty
}

int Games::Copy(Block& to, const Block& from)
{
	for (int y=0; y < bmax; ++y)
	for (int x=0; x < bmax; ++x)
		to.b[y][x] = from.b[y][x];
}
void Games::Clear(Block& b)
{
	for (int y=0; y < bmax; ++y)
	for (int x=0; x < bmax; ++x)
		b.b[y][x] = 0;
}



//  Find block range  min [a, max b], from not empty rows x-, cols y|
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 
void Games::GetRange(const Block& b)
{
	xa = -1;  xb = -1;  ya = -1;  yb = -1;
	int ss = o.bsize, s = ss-1;
	int x, y;
	y = 0;  while (y < ss && ya < 0)  {  for (x=0; x<ss; ++x)  if (b.b[y][x])  ya = y;  ++y;  }
	y = s;  while (y >= 0 && yb < 0)  {  for (x=0; x<ss; ++x)  if (b.b[y][x])  yb = y;  --y;  }
	x = 0;  while (x < ss && xa < 0)  {  for (y=0; y<ss; ++y)  if (b.b[y][x])  xa = x;  ++x;  }
	x = s;  while (x >= 0 && xb < 0)  {  for (y=0; y<ss; ++y)  if (b.b[y][x])  xb = x;  --x;  }

	//  check  find geom center
	if (xa < 0 || ya < 0 || xb < 0 || yb < 0)
		errors += 100;
}

//  Rotate block  dest, src
void Games::Rotate(Block& to, const Block& from, int cw)
{
	Clear(to);
	GetRange(from);
	int x,y,xx,yy;
	
	if (cw)  // clockwise
	for (y = ya; y <= yb; ++y)
	for (x = xa; x <= xb; ++x)
	{	yy = x-xa +ya;	//  range not checked
		xx = yb-y +xa;  //  ok after Move in GenBlock
		if (yy<0 || xx<0 ||
			xx>=o.bsize || yy>=o.bsize)
			++errors;  //cut-
		else
			to.b[yy][xx] = from.b[y][x];
	}else
	for (y = ya; y <= yb; ++y)
	for (x = xa; x <= xb; ++x)
	{	yy = xb-x +ya;
		xx = y-ya +xa;
		if (yy<0 || xx<0 ||
			xx>=o.bsize || yy>=o.bsize)
			++errors;  //cut-
		else
			to.b[yy][xx] = from.b[y][x];
	}
}


//  Draw
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 
const static char* sOptPages[Games::O_All] = {
	"Field", "Speed", "Block", "Draw", "Input"};

void Games::Draw(Adafruit_SSD1306& d)
{
	d.clearDisplay();
	int x = 0, y = 0;
	d.setCursor(x,y);

	if (gui==1)
	{
		d.setFont(&FreeSans9pt7b);
		d.print("Sixtis");  d.setFont(0);

		d.setCursor(W/2, 4);
		d.println(sPresets[preset]);  // title
		
		d.setCursor(0, 16);
		for (y=0; y < G_All; ++y)
		{
			d.print(y==yg ? "\x10":" ");
			switch (y)
			{	case G_Resume:   d.println("Resume");  break;
				case G_NewGame:  d.println("New Game");  break;
				case G_Preset:   d.print("Preset: ");
					d.print(preset+1);  d.print("/");  d.println(Presets);
					//d.print("   ");  d.println(ssPresets[preset]);  // title
					break;
				case G_Options: d.println("Options");  break;
				case G_Help:    d.println("Help");  break;
			}
		}
		return;
	}
	if (gui==2)
	{
		d.setFont(&FreeSans9pt7b);
		d.print("Sixtis");  d.setFont(0);
		
		d.setCursor(W-1-3*6, 0);
		d.print(opg+1);  d.print("/");  d.print(O_All);
		
		d.setCursor(W/2-6, 4);
		d.print(sOptPages[opg]);  // title

		x = 8;  y = 16+8;  int l = 0;
		#define  line(str, par, yy)  \
			if (l == oyg) {  d.setCursor(0,y);  d.print("\x10");  }  d.setCursor(x,y);  \
			d.print(str);  d.print(par);  ++l;  y+=yy;  d.setCursor(x,y)

		switch (opg)
		{
		case O_Field:
			line("Width   ", o.size_x, 8);
			line("Height  ", o.size_y, 8+4);
			line("Junk lines  ", o.btm_junk, 8);
			break;
		case O_Speed:
			line("Fall Speed    ", o.speed/SpdDet, 8+2);
			line("Acceleration  ", o.accel, 8+4);
			break;
		case O_Block:  y-=6;
			line("Length min  ", o.blen_min, 8);
			line("Length max  ", o.blen_max, 8);
			line("Size   ", o.bsize, 8+2);
			line("Diagonal  ", o.bdiag, 8);
			line("Bias   ", o.bbias, 8);
			break;
		
		case O_Draw:
			line("Preview blocks  ", o.nx_cur, 8+2);
			line("Grid dots   ", o.dots, 8+2);
			line("Frame draw  ", o.frame, 8+2);
			break;
		case O_Input:
			line("Key repeat  ", o.key_rpt, 8+2);
			line("Move in drop  ", o.move_in_drop, 8+2);
			line("Fall speed  ", o.sp_fall, 8);
			line("Drop speed  ", o.sp_drop, 8+2);
			break;
		}
		// cursor out
		if (oyg < 0)  oyg = l-1;
		if (oyg >= l)  oyg = 0;
		return;
	}
	d.setFont(0);

	#if 0  //  debug --
	//x = W-1 - 5*6;  y = H-1 - 4*8;
	d.setCursor(x,y);  d.print(xo);  d.print(",");  d.print(yo);  y+=8+1;
	d.setCursor(x,y);  d.print(xa);  d.print("-");  d.print(xb);  y+=8;
	d.setCursor(x,y);  d.print(ya);  d.print("|");  d.print(yb);  y+=8;
	d.setCursor(x,y);  d.print("e ");  d.print(errors);  y+=8;
	#else
	//  score
	d.setCursor(x,  y);  d.print("Score");  y+=8;
	d.setCursor(x+6,y);  d.print(score);  y+=8+1;

	//d.setCursor(x,  y);  d.print("Lines");  y+=8;
	//d.setCursor(x+6,y);  d.print(lines[0]);  y+=8+2;

	d.setCursor(x,  y);  d.print("Speed");  y+=8;
	d.setCursor(x+6,y);  d.print(speed_y / SpdDet);  y+=8+1;
	#endif

	//d.setCursor(x,  y);  d.print("Size");  y+=8;
	//d.setCursor(0,0);  d.print(o.size_x);  d.print(" ");  d.print(o.size_y);  y+=8;

	//d.setCursor(x,y);  d.print("Set ");  d.print(preset);  y+=8;

	//  status
	d.setCursor(0,H-1-8);
	if (ended)  d.print("Ended");
	if (paused) d.print("Pause");
	
	
	//  Grid  ::
	const int dx = dim_x /2, dy = dim_y /2;
	//const int dx = 0, dy = 0;
	int yy, xx;
	for (y=0; y < o.size_y; ++y)
	{	yy = y * dim_y + ofs_y;
		for (x=0; x < o.size_x; ++x)
		{
			xx = x * dim_x + ofs_x;
			//  grid modes
			if (o.dots==0 || 
				o.dots==1 && (x%2 == y%2) ||
				o.dots==2 && (x%3==1 && y%3==1))
			d.drawPixel(xx+dx, yy+dy, WHITE);  // .
			
			if (grid[y][x])  //  blocks
				d.fillRect(xx, yy, dim_x, dim_y, WHITE);
		}
	}
	//  Frame  | |
	if (o.frame > 0)
	{
		xx = ofs_x + o.size_x * dim_x;
		for (y = ofs_y; y < H; y += o.frame)
		{
			d.drawPixel(ofs_x-1, y, WHITE);
			d.drawPixel(xx, y, WHITE);
		}
	}
	
	//  Current block '
	//Draw(d, blk, pos_x, 0);  // grid y
	y = dim_y * time_y / time_spd;
	Draw(d, blk, pos_x, pos_y-1, y);  // fine y

	//  Drop preview .
	y = pos_y;  //+1
	while (!Overlaps(blk, pos_x, y))  ++y;
	Draw(d, blk, pos_x, y-1, 0, 1);

	
	//  Next blocks :
	if (!o.nx_cur)  return;
	int by = dim_y * o.bsize;
	int xe, ys = (H - o.nx_cur * yy) / o.nx_cur;
	ys = max(0, ys);
	
	for (y=0; y < o.nx_cur; ++y)
	{
		yy = y * by + y * ys;

		xx = ofs_x + (o.size_x+3) * dim_x;   // |next to field
		xe = W-1 - dim_x * o.bsize /*- dim_x/2*/;  // screen right|
		if (xx > xe)  xx = xe;
		
		DrawNext(d, next[y], xx, yy);
		
		//  line ..
		xe = xx + o.bsize * dim_x;
		if (y > 0)
		for (x=xx; x < xe; x+=2)
			d.drawPixel(x, yy-1, WHITE);
	}
}


//  Keys
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 
int kl = 0, kr = 0;
int Games::KeyPress(int8_t& mlevel)
{
	//  global
	if (kk[KEYPAD_MINUS] && !kko[KEYPAD_MINUS] ||
		kk[KEY_ESC] && !kko[KEY_ESC])
	{
		if (gui==2)  gui=0;  // off options
		else
		if (gui==1)  mlevel = 0;  // <back to menu
		else  gui = 1;
	}
	
	if (gui==1)  // - menu -
	{
		if (kk[KEY_UP] && !kko[KEY_UP])      yg = (yg - 1 + G_All) % G_All;
		if (kk[KEY_DOWN] && !kko[KEY_DOWN])  yg = (yg + 1) % G_All;
		
		if (yg == G_Preset)
		{
			if (kk[KEY_LEFT] && !kko[KEY_LEFT])
			{	preset = (preset - 1 + Presets) % Presets;	NewGrid();  }
			if (kk[KEY_RIGHT] && !kko[KEY_RIGHT])
			{	preset = (preset + 1 + Presets) % Presets;  NewGrid();  }
		}else
		if (kk[KEY_RIGHT] && !kko[KEY_RIGHT])  // enter>
		{
			switch (yg)
			{
			case G_Resume:  gui=0;  break;
			case G_NewGame:  NewGame();  gui=0;  break;
			case G_Options:  gui = 2;  break;
			case G_Help:  return 1;
			}
		}		
		return 0;
	}
	if (gui==2)  // - options -
	{
		if (kk[KEY_UP  ] && !kko[KEY_UP  ])  --oyg;  //oyg = (oyg - 1 + 6) % 6;
		if (kk[KEY_DOWN] && !kko[KEY_DOWN])  ++oyg;  //oyg = (oyg + 1) % 6;
		
		int k = 0, s = kk[KEY_RCTRL] ? 4 : 1;
		if (kk[KEY_LEFT ] && !kko[KEY_LEFT ])  k =-s;
		if (kk[KEY_RIGHT] && !kko[KEY_RIGHT])  k = s;
		
		if (k)  // change params  ----
		switch (opg)
		{
		case O_Field:  switch (oyg)  {
			case 0:  o.size_x   += k;  o.size_x   = max(2, min(32, o.size_x));  break;
			case 1:  o.size_y   += k;  o.size_y   = max(2, min(32, o.size_y));  break;  //8 21
			case 2:  o.btm_junk += k;  o.btm_junk = max(0, min(32, o.btm_junk));  break;
			}	break;
		case O_Speed:  switch (oyg)  {
			case 0:  o.speed += k*SpdDet*4;  o.speed = max(0, min(400*SpdDet, o.speed));  break;
			case 1:  o.accel += k;  o.accel = max(0, min( 80, o.accel));  break;
			}	break;
		case O_Block:  switch (oyg)  {
			case 0:  o.blen_min += k;  o.blen_min = max(1, min(32, o.blen_min));
				if (o.blen_max < o.blen_min)  o.blen_max = o.blen_min;  break;
			case 1:  o.blen_max += k;  o.blen_max = max(1, min(32, o.blen_max));
				if (o.blen_min > o.blen_max)  o.blen_min = o.blen_max;  break;
			case 2:  o.bsize += k;  o.bsize = max(2, min(bmax, o.bsize));  break;
			case 3:  o.bdiag += k;  o.bdiag = max(2, min(8, o.bdiag));  break;
			case 4:  o.bbias += k;  o.bbias = max(0, min(16, o.bbias));  break;
			}	break;
		
		case O_Draw:  switch (oyg)  {
			case 0:  o.nx_cur += k;  o.nx_cur = max(0, min(4, o.nx_cur));  break;
			case 1:  o.dots   += k;  o.dots   = max(0, min(4, o.dots));  break;
			case 2:  o.frame  += k;  o.frame  = max(0, min(4, o.frame));  break;
			}	break;
		case O_Input:  switch (oyg)  {
			case 0:  o.key_rpt += k;  o.key_rpt = max(0, min(60, o.key_rpt));  break;
			case 1:  o.move_in_drop = 1-o.move_in_drop;  break;
			case 2:  o.sp_fall += k;  o.sp_fall = max(1, min(24, o.sp_fall));  break;
			case 3:  o.sp_drop += k;  o.sp_drop = max(1, min(10, o.sp_drop));  break;
			}	break;
		}

		if (kk[KEY_PAGE_UP  ] && !kko[KEY_PAGE_UP  ])  opg = (opg - 1 + O_All) % O_All;
		if (kk[KEY_PAGE_DOWN] && !kko[KEY_PAGE_DOWN])  opg = (opg + 1) % O_All;
		return 0;
	}

	//  ---  game  ---
	
	if (kk[KEY_ENTER] && !kko[KEY_ENTER])  // new
	{	/*NewGrid();*/  NewGame();  }

	if (kk[KEY_Q] && !kko[KEY_Q])
		preset = (preset - 1 + Presets) % Presets;
	if (kk[KEY_W] && !kko[KEY_W])
		preset = (preset + 1 + Presets) % Presets;

	if (ended)  return 0;

	if (kk[KEYPAD_PLUS] && !kko[KEYPAD_PLUS] ||
		kk[KEY_SPACE] && !kko[KEY_SPACE])  // pause
		paused = 1 - paused;

	if (paused)  return 0;


	//  --  move  --
	if (!drop || o.move_in_drop)
	{
		//  rotate  control
		if (demo && random(500)==0 ||
			kk[KEY_UP] && !kko[KEY_UP])  // rot cw
		{
			Rotate(cpy, blk, 1);  //  check possible
			if (!Overlaps(cpy, pos_x, pos_y))
				Copy(blk, cpy);
		}
		if (kk[KEYPAD_5] && !kko[KEYPAD_5] ||
			kk[KEYPAD_SLASH] && !kko[KEYPAD_SLASH])  // rot ccw
		{
			Rotate(cpy, blk, 0);  //  check possible
			if (!Overlaps(cpy, pos_x, pos_y))
				Copy(blk, cpy);
		}

		//  move
		#define  rpt  o.key_rpt
		int k = kk[KEY_LEFT];  // auto repeat
		if (k && rpt)  kl++;  else  kl = 0;

		if (demo && random(350)==0 ||
			kl > rpt ||  k && !kko[KEY_LEFT])  // move
		{	kl = 0;
			int old_x = pos_x;
			--pos_x;  if (pos_x < 0)  pos_x = o.size_x-1;
			if (Overlaps(blk, pos_x, pos_y))
				pos_x = old_x;  //  if not possible restore
		}

		k = kk[KEY_RIGHT];  // auto
		if (k && rpt)  kr++;  else  kr = 0;

		if (demo && random(350)==0 ||
			kr > rpt ||  k && !kko[KEY_RIGHT])
		{	kr = 0;
			int old_x = pos_x;
			++pos_x;  if (pos_x > o.size_x-1)  pos_x = 0;
			if (Overlaps(blk, pos_x, pos_y))
				pos_x = old_x;  //  if not possible restore
		}
	}

	fall = kk[KEY_DOWN];  // fall faster hold

	if (kk[KEY_INSERT] && !kko[KEY_INSERT])  // drop
		drop = 1;


	//  - other -
	if (kk[KEY_D] && !kko[KEY_D])  // demo-
		demo = 1 - demo;

	#if 1
	if (kk[KEY_1] && !kko[KEY_1])
	{	speed_y -= 10*SpdDet;  UpdSpeed();  }
	if (kk[KEY_2] && !kko[KEY_2])
	{	speed_y += 10*SpdDet;  UpdSpeed();  }
	#endif

	Update();
	return 0;
}


//  Update
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 
void Games::Update()
{
	++time_y;
	uint32_t ti = micros(), dt = ti - old_ti;  old_ti = ti;
	dt_sum += dt;

	const uint32_t iv = 16666;  // 60 fps const
	while (dt_sum > iv)
	{
		dt_sum -= iv;
		if (!paused && !ended)
		{
			++time_y;
			
			//  speed
			uint spd = drop ? o.sp_drop : fall ? o.sp_fall : time_spd;
			//  accel
			speed_y += o.accel;  UpdSpeed();
			
			if (time_y > spd)
			{	time_y = 0;

				//  move down
				if (!Overlaps(blk, pos_x, pos_y+1))
					++pos_y;
				else
				{
					//  landed, store  x
					int x,y;
					for (y=0; y < o.size_y; ++y)
					{	int yy = (pos_y + y);
						
						if (yy < o.size_y)
						for (x=0; x < o.size_x; ++x)
							if (blk.b[y][x])
								grid[yy][(pos_x + x) % o.size_x] = 1;
					}
					
					//  clear full lines  __
					y = o.size_y - 1;  int l=0;
					while (y > 0)
					{
						int c = 0, j;
						for (x=0; x < o.size_x; ++x)
							if (grid[y][x])  ++c;

						if (c == o.size_x)  // full, move down
						{
							for (j = y; j > 0; --j)
							for (x=0; x < o.size_x; ++x)
								grid[j][x] = grid[j-1][x];
							
							++lines;  ++l;
						}else
						{	--y;
							score += l*l;  // the more close
							l = 0;
						}
					}

					NewBlock();  // new
					
					if (Overlaps(blk, pos_x, pos_y))
					{
						ended = 1;
						//NewGame();  // game over
					}
				}
			}
		}
	}
}


//  Generate new block
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 
void Games::GenBlock(Block& b)
{
	int x,y;
	int ss = o.bsize, s = ss-1,
		s2 = s/2; //ss/2

	int sc = 0, same = 1;
	while (same && sc < 3)
	{	++sc;
	
		Clear(b);

		//  generate
		int cx = ss / 2, cy = cx;  // start in center
		//int cx = s2, cy = cx;
		
		#if 1
		int len = random(o.blen_max+1 - o.blen_min) + o.blen_min;  //~
		#else  // biased
		int len = o.blen_min;   y = o.blen_max - o.blen_min;
		for (x=0; x < y; ++x)
			if (random(100) < o.bbias*2 + 12)
				++len;
		len = min(len, o.blen_max);
		#endif
		
		int l = 0, err = 0;
		while (l < len && err < 100)
		{
			if (b.b[cy][cx] == 0)
			{	b.b[cy][cx] = 1;  ++l;  //  inc len
			}else  ++err;
			
			switch (random(o.bdiag))
			{	//  move +
				case 0:  if (cx < s)  ++cx;  break;
				case 1:  if (cx > 0)  --cx;  break;
				case 2:  if (cy < s)  ++cy;  break;
				case 3:  if (cy > 0)  --cy;  break;
				//  digonal x
				case 4:  if (cx < s)  ++cx;  if (cy < s)  ++cy;  break;
				case 5:  if (cx > 0)  --cx;  if (cy > 0)  --cy;  break;
				case 6:  if (cx > 0)  --cx;  if (cy < s)  ++cy;  break;
				case 7:  if (cx < s)  ++cx;  if (cy > 0)  --cy;  break;
			}
		}
		//errors = err;

		GetRange(b);
			
		//  new begin pos
		//01   ss2 s1 s2 0
		//012  ss3 s2 s2 1
		//0123 ss4 s3 s2 1  par
		xo = s2 - (xb-xa+1)/2;  xo = max(0,xo);
		yo = s2 - (yb-ya+1)/2;  yo = max(0,yo);
		
		//  Move
		Copy(cpy,b);  Clear(b);

		for (y=ya; y<=yb; ++y)
		for (x=xa; x<=xb; ++x)
		{	int i = x-xa+xo, j = y-ya+yo;
			if (i<0 || i>s || j<0 || j>s)
				errors += 1;
			b.b[j][i] = cpy.b[y][x];
		}
		
		//  check if same as previously generated
		//memcmp(
		same = 1;
		for (y=0; y < bmax; ++y)
		for (x=0; x < bmax; ++x)
			if (prv.b[y][x] != b.b[y][x])
				same = 0;
	}
	Copy(prv, b);
}
