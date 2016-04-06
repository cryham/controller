#include <games.h>
extern "C" {
	#include <scan_loop.h>  // keys kk
	#include <usb_hid.h>  // key_defs
}

//  main
Games::Games()
{
	old_ti = 0;  dt_sum = 0;
	paused = 0;  demo = 0;  ended = 0;

	Init();
}

void Games::Init()
{
	//  keys
	o.sp_drop = 5;  o.sp_fall = 16;
	o.key_rpt = 30;  o.move_in_drop = 0;
	
	preset = 2;
	xo= 0; yo= 0; xa= 0; ya= 0; xb= 0; yb= 0;

	NewGrid();
	NewGame();
}

//  Init
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 
void Games::NewGrid()
{
	switch (preset)
	{
	case 0:  // tiny
		o.size_x = 3;  o.size_y = 8;  o.btm_junk = 2;
		o.glen_min = 1;  o.glen_max = 4;  o.gsize = 2;  o.nx_cur = 2;
		o.speed = 120 * SpdDet;  o.accel = 20;
		break;

	case 1:  // basic
		o.size_x = 6;  o.size_y = 10;  o.btm_junk = 1;
		o.glen_min = 1;  o.glen_max = 8;  o.gsize = 3;  o.nx_cur = 2;
		o.speed = 80 * SpdDet;  o.accel = 30;
		break;

	case 2:  // simple
		o.size_x = 8;  o.size_y = 12;  o.btm_junk = 0;
		o.glen_min = 1;  o.glen_max = 4;  o.gsize = 3;  o.nx_cur = 2;
		o.speed = 60 * SpdDet;  o.accel = 40;
		break;

	case 3:  // tetris meh
		o.size_x = 10;  o.size_y = 12;  o.btm_junk = 3;
		o.glen_min = 4;  o.glen_max = 4;  o.gsize = 4;  o.nx_cur = 3;
		o.speed = 30 * SpdDet;  o.accel = 30;
		break;

	case 4:  // pentis
		o.size_x = 11;  o.size_y = 14;  o.btm_junk = 2;
		o.glen_min = 2;  o.glen_max = 5;  o.gsize = 5;  o.nx_cur = 3;
		o.speed = 30 * SpdDet;  o.accel = 20;
		break;

	case 5:  // sixtis
		o.size_x = 12;  o.size_y = 16;  o.btm_junk = 1;
		o.glen_min = 2;  o.glen_max = 6;  o.gsize = 5;  o.nx_cur = 3;
		o.speed = 30 * SpdDet;  o.accel = 10;
		break;

	case 6:  // septis
		o.size_x = 13;  o.size_y = 18;  o.btm_junk = 1;
		o.glen_min = 2;  o.glen_max = 8;  o.gsize = 6;  o.nx_cur = 2;
		o.speed = 20 * SpdDet;  o.accel = 1;
		break;

	case 7:  // huge
		o.size_x = 14;  o.size_y = 20;  o.btm_junk = 3;
		o.glen_min = 2;  o.glen_max = 10;  o.gsize = 7;  o.nx_cur = 2;
		o.speed = 10 * SpdDet;  o.accel = 0;
		break;
	}
	
	//  size
	//o.size_x = 12;  o.size_y = 16;  o.btm_junk = 1;
													
	//  box dim
	dim_y = H / o.size_y;
	dim_x = dim_y;

	//  center, bottom
	ofs_x = (W - o.size_x*dim_x) / 2;
	ofs_y = (H - o.size_y*dim_y);

	//  block gen
	//o.glen_min = 3;  o.glen_max = 8;
	//o.gsize = 5;
	//o.nx_cur = 0;  // prv
	
	//  speed
	//o.speed = 30 * SpdDet;  o.accel = 1;
}

void Games::NewBlock()
{
	//  pos center
	pos_x = o.size_x / 2 - o.gsize / 2;
	pos_y = 0;  time_y = 0;
	fall = 0;  drop = 0;

	if (!o.nx_cur)
	{
		GenBlock(blk);
		return;
	}
	
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
	int i,x,y;
	for (i=0; i < bmax; ++i)
		lines[i] = 0;
		
	speed_y = o.speed;  UpdSpeed();

	//  clear
	for (y=0; y < smax_y; ++y)
	for (x=0; x < smax_x; ++x)
		grid[y][x] = 0;

	for (y=0; y < bmax; ++y)
	for (x=0; x < bmax; ++x)
	{	blk.b[y][x] = 0;
		cpy.b[y][x] = 0;
		for (i=0; i < nx_max; ++i)
			next[i].b[y][x] = 0;
	}
	
	//  bottom stuff  . ..
	int y0 = o.size_y - o.btm_junk;
	for (y=y0; y < o.size_y; ++y)
	for (x=0; x < o.size_x; ++x)
		if (random(100) < min(60, (y-y0+1) * 30))
			grid[y][x] = 1;
		
	//  randomize  --
	for (i=0; i < 1323; ++i)
		random(1000);
		
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
	for (int y=0; y < o.gsize; ++y)
	for (int x=0; x < o.gsize; ++x)
	{
		int yy = (pos_y + y) % o.size_y * dim_y + ofs_y + o_y;
		int xx = (pos_x + x) % o.size_x * dim_x + ofs_x;
		if (b.b[y][x]  && yy >= 0)
			d.fillRect(xx, yy, dim_x, dim_y, WHITE);
	}
	else  //  dim, prv
	for (int y=0; y < o.gsize; ++y)
	for (int x=0; x < o.gsize; ++x)
	{
		int yy = (pos_y + y) % o.size_y * dim_y + ofs_y + o_y;
		int xx = (pos_x + x) % o.size_x * dim_x + ofs_x;
		if (b.b[y][x])
			for (int j=0; j < dim_y; ++j)
			for (int i=0; i < dim_x; ++i)
				if ((xx+i) % 2 == (yy+j) % 2)
					d.drawPixel(xx + i, yy + j, WHITE);
	}
}
void Games::DrawNext(Adafruit_SSD1306& d, const Block& b,
	int pos_x, int pos_y)
{
	for (int y=0; y < o.gsize; ++y)
	for (int x=0; x < o.gsize; ++x)
	{
		int yy = pos_y + y * dim_y;
		int xx = pos_x + x * dim_x;
		d.drawPixel(xx + dim_x/2, yy + dim_y/2, WHITE);  //.

		if (b.b[y][x]  && xx < W && yy < H)
			d.fillRect(xx, yy, dim_x, dim_y, WHITE);
	}
}


//  utility
int Games::Overlaps(const Block& b, int pos_x, int pos_y)
{
	for (int y=0; y < o.gsize; ++y)
	for (int x=0; x < o.gsize; ++x)
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

void Games::Rotate(Block& to, const Block& from, int cw)
{
	if (cw)
		for (int y=0; y < o.gsize; ++y)
		for (int x=0; x < o.gsize; ++x)
			to.b[y][x] = from.b[o.gsize-1-x][y];
	else
		for (int y=0; y < o.gsize; ++y)
		for (int x=0; x < o.gsize; ++x)
			to.b[y][x] = from.b[x][o.gsize-1-y];
}


//  Draw
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 
void Games::Draw(Adafruit_SSD1306& d)
{
	d.clearDisplay();
	d.setFont(0);
	int x, y;

	//  debug --
	x = W-1 - 5*6;  y = H-1 - 4*8;
	d.setCursor(x,y);  d.print(xo);  d.print(",");  d.print(yo);  y+=8+1;
	d.setCursor(x,y);  d.print(xa);  d.print("-");  d.print(xb);  y+=8;
	d.setCursor(x,y);  d.print(ya);  d.print("|");  d.print(yb);  y+=8;
	d.setCursor(x-6,y);  d.print("e ");  d.print(errors);  y+=8;
	
	//  score
	x = 0;  y = 0;
	d.setCursor(x,  y);  d.print("Score");  y+=8;
	d.setCursor(x+6,y);  d.print(score);  y+=8+1;

	//d.setCursor(x,  y);  d.print("Lines");  y+=8;
	//d.setCursor(x+6,y);  d.print(lines[0]);  y+=8+2;

	d.setCursor(x,  y);  d.print("Speed");  y+=8;
	d.setCursor(x+6,y);  d.print(speed_y / SpdDet);  y+=8+1;

	//d.setCursor(x,  y);  d.print("Size");  y+=8;
	//d.setCursor(0,0);  d.print(o.size_x);  d.print(" ");  d.print(o.size_y);  y+=8;

	d.setCursor(x,y);  d.print("Set ");  d.print(preset);  y+=8;


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
			d.drawPixel(xx+dx, yy+dy, WHITE);  // .
			
			if (grid[y][x])  //  blocks
				d.fillRect(xx, yy, dim_x, dim_y, WHITE);
		}
	}
	
	//  Current block '
	//Draw(d, blk, pos_x, 0);
	y = dim_y * time_y / time_spd;
	Draw(d, blk, pos_x, pos_y-1, y);

	//  Drop preview .
	y = pos_y+1;
	while (!Overlaps(blk, pos_x, y))  ++y;
	Draw(d, blk, pos_x, y-1, 0, 1);
	
	//  Next blocks :
	for (x=0; x < o.nx_cur; ++x)
		DrawNext(d, next[x], W-1 - (dim_x+1) * o.gsize, x * dim_y * o.gsize);
}


//  Keys
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 
int kl = 0, kr = 0;
void Games::KeyPress(int8_t& mlevel)
{
	//  global
	if (kk[KEYPAD_MINUS] && !kko[KEYPAD_MINUS] ||
		kk[KEY_ESC] && !kko[KEY_ESC])
		mlevel = 0;  // <back to menu

	if (kk[KEY_ENTER] && !kko[KEY_ENTER])  // new
	{	NewGrid();  NewGame();  }

	if (kk[KEY_Q] && !kko[KEY_Q])
		preset = (preset - 1 + Presets) % Presets;
	if (kk[KEY_W] && !kko[KEY_W])
		preset = (preset + 1 + Presets) % Presets;

	if (ended)  return;

	if (kk[KEYPAD_PLUS] && !kko[KEYPAD_PLUS] ||
		kk[KEY_SPACE] && !kko[KEY_SPACE])  // pause
		paused = 1 - paused;

	if (paused)  return;

	if (kk[KEY_D] && !kko[KEY_D])  // demo-
		demo = 1 - demo;

	#if 1
	if (kk[KEY_1] && !kko[KEY_1])
	{	speed_y -= 20*SpdDet;  UpdSpeed();  }
	if (kk[KEY_2] && !kko[KEY_2])
	{	speed_y += 20*SpdDet;  UpdSpeed();  }
	#endif

	
if (!drop || o.move_in_drop)
{
	//  rotate  control
	if (demo && random(500)==0 ||
		kk[KEY_UP] && !kko[KEY_UP])  // rot cw
	{
		Rotate(cpy, blk);  //  check possible
		if (!Overlaps(cpy, pos_x, pos_y))
			Copy(blk, cpy);
	}
	if (kk[KEYPAD_5] && !kko[KEYPAD_5] ||
		kk[KEYPAD_SLASH] && !kko[KEYPAD_SLASH])  // rot ccw
	{
		Rotate(cpy, blk);  //  check possible
		if (!Overlaps(cpy, pos_x, pos_y))
			Copy(blk, cpy);
	}

	//  move
	#define  rpt  o.key_rpt
	int k = kk[KEY_LEFT];  // auto repeat
	if (k)  kl++;  else  kl = 0;

	if (demo && random(350)==0 ||
		kl > rpt ||  k && !kko[KEY_LEFT])  // move
	{	kl = 0;
		int old_x = pos_x;
		--pos_x;  if (pos_x < 0)  pos_x = o.size_x-1;
		if (Overlaps(blk, pos_x, pos_y))
			pos_x = old_x;  //  if not possible restore
	}

	k = kk[KEY_RIGHT];  // auto
	if (k)  kr++;  else  kr = 0;

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
	
	Update();
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
				while (y >= 0)
				{
					int c = 0, j;
					for (x=0; x < o.size_x; ++x)
						if (grid[y][x])  ++c;

					if (c == o.size_x)  // full, move down
					{
						for (j=y; j > 0; --j)
						for (x=0; x < o.size_x; ++x)
							grid[j][x] = grid[j-1][x];
						
						++lines[0];  ++l;
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


//  Generate new block
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 
void Games::GenBlock(Block& b)
{
	int x,y;
	int ss = o.gsize, s = ss-1;

	//  clear
	for (y=0; y < bmax; ++y)
	for (x=0; x < bmax; ++x)
		b.b[y][x] = 0;

	//  generate
	//int cx = ss / 2, cy = cx;  // start in center
	int cx = ss % 2 == 1 ? ss / 2 : max(0, ss / 2 - 1), cy = cx;
	int len = random(o.glen_max+1 - o.glen_min) + o.glen_min;
	
	int l = 0, err = 0;
	while (l < len && err < 100)
	{
		if (b.b[cy][cx] == 0)
		{	b.b[cy][cx] = 1;
			++l;  //  inc len
		}else  ++err;
		
		switch (random(4))
		{	//  move +
			case 0: if (cx < s)  ++cx;  break;
			case 1: if (cx > 0)  --cx;  break;
			case 2: if (cy < s)  ++cy;  break;
			case 3: if (cy > 0)  --cy;  break;
		}
	}
	//errors = err;

	//  find min,max not empty rows,cols
	xa = -1;  xb = -1;  ya = -1;  yb = -1;

	y = 0;  while (y < ss && ya < 0)  {  for (x=0; x<ss; ++x)  if (b.b[y][x])  ya = y;  ++y;  }
	y = s;  while (y >= 0 && yb < 0)  {  for (x=0; x<ss; ++x)  if (b.b[y][x])  yb = y;  --y;  }
	x = 0;  while (x < ss && xa < 0)  {  for (y=0; y<ss; ++y)  if (b.b[y][x])  xa = x;  ++x;  }
	x = s;  while (x >= 0 && xb < 0)  {  for (y=0; y<ss; ++y)  if (b.b[y][x])  xb = x;  --x;  }

	//  find geom center
	if (xa < 0 || ya < 0 || xb < 0 || yb < 0)
	{	errors += 100;
		return;  }
	
	int d = 33;  // 8  half adjust-
	s = ss * d / 64;
	xo = s - ((xb - xa) * d / 64 + xa);
	yo = s - ((yb - ya) * d / 64 + ya);
	
	//  center block
	Copy(cpy, b);
		int n, nn;
		n = 0;
		for (y=0; y < ss; ++y)
		for (x=0; x < ss; ++x)
			if (cpy.b[y][x])  ++n;
		nn = n;
	for (y=0; y < bmax; ++y)
	for (x=0; x < bmax; ++x)
		b.b[y][x] = 0;
		
	//  move
	int i,j, o;
	if (0)
	for (y=0; y < ss; ++y)
	for (x=0; x < ss; ++x)
	{	i = x - xo;  j = y - yo;  // ofs
		o = i < 0 || i > s || j < 0 || j > s;  // out
		b.b[y][x] = o ? 0 : cpy.b[j][i];
	}
		n = 0;
		for (y=0; y < ss; ++y)
		for (x=0; x < ss; ++x)
			if (b.b[y][x])  ++n;
		if (nn != n)
			errors += 10;
}
