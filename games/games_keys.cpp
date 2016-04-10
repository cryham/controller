#include <games.h>
extern "C" {
	#include <scan_loop.h>  // keys kk
	#include <usb_hid.h>  // key_defs
}

//  options check  . . .
void Games::Checks()
{
	if (o.blen_max < o.blen_min)  o.blen_max = o.blen_min;  // min <= max
	if (o.blen_min > o.blen_max)  o.blen_min = o.blen_max;
	if (o.bsize > o.size_x)  o.bsize = o.size_x;  // block <= field
	if (o.bsize > o.size_y)  o.bsize = o.size_y;
	int bb = o.bsize*o.bsize;
	if (o.blen_min > bb)  o.blen_min = bb;  // block_min < bsize^2
	if (o.blen_max > bb)  o.blen_max = bb;

	if (kk[KEY_PAGE_UP  ] && !kko[KEY_PAGE_UP  ])  opg = (opg - 1 + O_All) % O_All;
	if (kk[KEY_PAGE_DOWN] && !kko[KEY_PAGE_DOWN])  opg = (opg + 1) % O_All;
}

//  Keys
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 
int kl = 0, kr = 0;
int Games::KeyPress(int8_t& mlevel)
{
	//  global
	if (kk[KEY_BACKSPACE] && !kko[KEY_BACKSPACE] ||
		kk[KEY_ESC] && !kko[KEY_ESC])
	{
		if (gui==2)  gui=0;  // off options
		else
		if (gui==1)  mlevel = 0;  // <back to menu
		else  gui = 1;  // gui on
	}

	if (kk[KEYPAD_ASTERISK] && !kko[KEYPAD_ASTERISK] ||
		kk[KEYPAD_MINUS] && !kko[KEYPAD_MINUS])
	{
		if (gui==2)  gui = 0;  // toggle options
		else  gui = 2;
	}
	
	
	if (gui==1)  // - menu -
	{
		if (kk[KEY_UP] && !kko[KEY_UP])      yg = (yg - 1 + G_All) % G_All;
		if (kk[KEY_DOWN] && !kko[KEY_DOWN])  yg = (yg + 1) % G_All;
		
		if (yg == G_Preset)
		{
			if (kk[KEY_LEFT] && !kko[KEY_LEFT])
			{	preset = (preset - 1 + Presets) % Presets;	NewSet();  }
			if (kk[KEY_RIGHT] && !kko[KEY_RIGHT])
			{	preset = (preset + 1 + Presets) % Presets;  NewSet();  }
			return 0;
		}
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
		if (kk[KEY_LEFT] && !kko[KEY_LEFT])
			mlevel = 0;  // <back to menu
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
			case 0:  o.size_x   += k;  o.size_x   = max(2, min(32, o.size_x));  NewGrid();  break;
			case 1:  o.size_y   += k;  o.size_y   = max(2, min(32, o.size_y));  NewGrid();  break;  //8 21
			case 2:  o.btm_junk += k;  o.btm_junk = max(0, min(32, o.btm_junk));  break;
			}	break;
		case O_Speed:  switch (oyg)  {
			case 0:  o.speed += k*SpdDet*4;  o.speed = max(0, min(200*SpdDet, o.speed));  break;
			case 1:  o.accel += k;  o.accel = max(0, min( 80, o.accel));  break;
			}	break;
		case O_Block:  switch (oyg)  {
			case 0:  o.bsize += k;  o.bsize = max(2, min(bmax, o.bsize));  break;

			case 1:  o.blen_min += k;  o.blen_min = max(-16, min(32, o.blen_min));  break;
			case 2:  o.blen_max += k;  o.blen_max = max(1, min(32, o.blen_max));  break;

			case 3:  o.bbias += k;  o.bbias = max(-16, min(16, o.bbias));  break;
			case 4:  o.bdiag += k;  o.bdiag = max(2, min(8, o.bdiag));  break;
			}	break;
		
		case O_Draw:  switch (oyg)  {
			case 0:  o.nx_cur += k;  o.nx_cur = max(0, min(4, o.nx_cur));  break;
			case 1:  o.dots   += k;  o.dots   = max(0, min(3, o.dots));  break;
			case 2:  o.frame  += k;  o.frame  = max(0, min(4, o.frame));  break;
			}	break;
		case O_Input:  switch (oyg)  {
			case 0:  o.key_rpt += k;  o.key_rpt = max(0, min(60, o.key_rpt));  break;
			case 1:  o.move_in_drop = 1-o.move_in_drop;  break;
			
			case 2:  o.sp_fall += k;  o.sp_fall = max(1, min(40, o.sp_fall));  break;
			case 3:  o.sp_drop += k;  o.sp_drop = max(1, min(10, o.sp_drop));  break;
			}	break;
		}
		Checks();
		return 0;
	}


	//  ---  game  ---
	
	if (kk[KEY_ENTER] && !kko[KEY_ENTER])  // new
		NewGame();

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


	#if 0
	//  - other -
	if (kk[KEY_M] && !kko[KEY_M])  // demo-
		demo = 1 - demo;

	if (kk[KEY_1] && !kko[KEY_1])
	{	if (speed_y > 5*SpdDet)  speed_y -= 5*SpdDet;
		else  speed_y = 0;  UpdSpeed();  }
	if (kk[KEY_2] && !kko[KEY_2])
	{	speed_y += 5*SpdDet;  UpdSpeed();  }
	#endif

	Update();

	return 0;
}
