#include <games.h>


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
		o.blen_min = 1;  o.blen_max = 6;  o.bsize = 5;  o.bdiag = 7;
		o.speed = 8 * SpdDet;  o.accel = 10;  break;

	case 6:  // sixtis
		o.size_x = 13;  o.size_y = 16;  o.btm_junk = 1;  o.dots = 3;  o.frame = 3;
		o.blen_min = 1;  o.blen_max = 8;  o.bsize = 6;  o.bdiag = 4;
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


//  Update
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 
void Games::Update()
{
	++time_y;
	uint32_t ti = micros(), dt = ti - old_ti;  old_ti = ti;
	if (dt < 100000)  // min 10 fps
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
					for (y=0; y < o.bsize; ++y)
					{	int yy = pos_y + y;

						if (yy < o.size_y)
						for (x=0; x < o.bsize; ++x)
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
