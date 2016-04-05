#include "demos.h"


//  CrystaL KeyboarD  logo lines
//....................................................................................
//  x,y points, 0 break
const static int word1[] = {
/*C*/194,43, 120,133, 8,193, 112,233, 224,299, 0,
	224,299, 141,227, 78,194, 144,139, 194,43, 0,
/*R*/192,110, 260,156, 211,176, 164,201, 220,153, 192,110, 0,
	164,201, 248,222, 314,258, 298,276, 251,237, 164,201, 0,
/*Y*/303,189, 388,224 , 450,190, 398,256 , 331,328, 374,242, 303,189, 0,
/*S*/541,92, 439,152, 533,196, 435,267, 507,226, 574,197, 517,173, 473,147, 541,92, 0,
/*T*/544,62, 628,61, 692,33, 629,75, 592,167, 615,76, 544,62, 0,
/*A*/623,216, 660,154, 673,82, 702,152, 746,206, 679,144, 623,216, 0,
/*L*/766,28, 793,147, 781,233, 738,300, 818,256,
	935,220, 815,229, 800,133, 766,28, 0, -100};
const static int word2[] = {
/*K*/217,345, 152,396, 62,440, 176,468, 288,507, 248,481, 117,438, 173,397, 217,345, 0,
	76,367, 118,471, 130,541, 145,516, 76,367, 0,
/*e*/329,359, 282,404, 207,434, 292,471, 368,517, 306,467, 256,438,
	318,433, 257,429, 297,401, 329,359, 0,
/*y*/357,450, 386,488, 424,507, 387,541, 334,561, 373,568, 427,521,
	468,453, 431,497, 357,450, 0,
/*b*/466,369, 492,399, 523,412, 486,435, 540,457, 505,476, 474,517,
	506,459, 486,435, 494,414, 466,369, 0,
/*o*/533,432, 557,414, 571,382, 587,409, 609,426, 591,452, 576,496, 559,455, 533,432, 0,
	573,409, 595,426, 575,461, 552,433, 573,409, 0,
/*a*/583,535, 608,493, 628,444, 649,477, 675,506, 630,481, 583,535, 0,
/*r*/660,385, 694,407, 651,427, 727,471, 706,479, 682,450,
	651,427, 670,407, 660,385, 0,
/*D*/717,363, 796,400, 861,432, 805,479, 761,504, 726,543,
	749,501, 795,445, 776,411, 717,363, 0, -100};

const static int  cw[2][8] = {  // centers xw1,xw2, yw1,yw2,  amplitudes same
	{496,402, 130,180,  222,262, 362,282},
	{496,502,  50,320,  222,312, 482,342}};
	
void Demos::CK_logo(Adafruit_SSD1306& d)
{
	#define K 1024  // wave																		// scale ofs
	#define CX(x) {  x = w-cx;  x = ( (x*(K +ax*Cos(8*w      +tt[0])/SY*Sin(7*w      +tt[1])/SY) /K) +cx)/8 +6;  }
	#define CY(y) {  y = w-cy;  y = ( (y*(K +ay*Cos(9*w+ x*73+tt[2])/SY*Sin(6*w+ x*52+tt[3])/SY) /K) +cy)/9 -1;  }

	const uint tt[4] = {t*7,t*5,t*8,t*5};
	for (int w2=0; w2<2; ++w2)
	{
		const int cx = cw[ckCur][w2], cy = cw[ckCur][2+w2],
				ax = cw[ckCur][4+w2], ay = cw[ckCur][6+w2];
		int a=0,w, i=0,rst=1,
			x1=0,y1=0,x=0,y=0;
		do
		{	w = w2 ? word2[a++] : word1[a++];
			if (w<=0) {  rst=1;  i=0;  }
			else
			if (rst)  switch(i)
			{	case 0:  CX(x)  ++i;  break;
				case 1:  CY(y)  rst=0; i=0;  break;  }
			else  switch(i)
			{	case 0:  x1=x;  CX(x)  ++i;  break;
				case 1:  y1=y;  CY(y)  i=2;  break;  }

			if (i==2)
			{	i=0;  d.drawLine(x1,y1, x,y, WHITE);  }
		}
		while (w >= 0);
	}

	if (iInfo > 0)
	{
		d.setCursor(0,8);
		d.print("Cur ");  d.println(ckCur);
		d.print("Spd ");  d.println(ckSpeed);
	}

	t += ckSpeed;
	delay(6);
}
//....................................................................................

//  3D

//  Pentagonal Icositetrahedron  

#define NP6  92
const static float p6[NP6][3] = {
{ 0.708903, 0.438126, 0.0},{-0.708903, 0.438126, 0.0},{ 0.708903,-0.438126, 0.0},{-0.708903,-0.438126, 0.0},
{ 0.438126, 0.0, 0.708903},{ 0.438126, 0.0,-0.708903},{-0.438126, 0.0, 0.708903},{-0.438126, 0.0,-0.708903},
{ 0.0, 0.708903, 0.438126},{ 0.0,-0.708903, 0.438126},{ 0.0, 0.708903,-0.438126},{ 0.0,-0.708903,-0.438126},
{ 0.787144,-0.072446, 0.081884},{ 0.787144, 0.072446,-0.081884},{-0.787144,-0.072446,-0.081884},{-0.787144, 0.072446, 0.081884},
{ 0.655368,-0.140772, 0.426878},{ 0.618258,-0.345709, 0.360265},{ 0.573484, 0.273263, 0.477485},{ 0.700142, 0.213218, 0.309658},
{ 0.655368, 0.140772,-0.426878},{ 0.618258, 0.345709,-0.360265},{ 0.573484,-0.273263,-0.477485},{ 0.700142,-0.213218,-0.309658},
{-0.655368, 0.140772, 0.426878},{-0.618258, 0.345709, 0.360265},{-0.573484,-0.273263, 0.477485},{-0.700142,-0.213218, 0.309658},
{-0.072446, 0.081884, 0.787144},{ 0.072446,-0.081884, 0.787144},{-0.072446,-0.081884,-0.787144},{ 0.072446, 0.081884,-0.787144},
{-0.655368,-0.140772,-0.426878},{-0.618258,-0.345709,-0.360265},{-0.573484, 0.273263,-0.477485},{-0.700142, 0.213218,-0.309658},
{ 0.309658, 0.700142, 0.213218},{ 0.477485, 0.573484, 0.273263},{-0.360265, 0.618258, 0.345709},{-0.426878, 0.655368, 0.140772},
{-0.140772, 0.426878, 0.655368},{-0.345709, 0.360265, 0.618258},{ 0.273263, 0.477485, 0.573484},{ 0.213218, 0.309658, 0.700142},
{ 0.140772,-0.426878, 0.655368},{ 0.345709,-0.360265, 0.618258},{-0.273263,-0.477485, 0.573484},{-0.213218,-0.309658, 0.700142},
{-0.309658,-0.700142, 0.213218},{-0.477485,-0.573484, 0.273263},{ 0.360265,-0.618258, 0.345709},{ 0.426878,-0.655368, 0.140772},
{ 0.140772, 0.426878,-0.655368},{ 0.345709, 0.360265,-0.618258},{-0.273263, 0.477485,-0.573484},{-0.213218, 0.309658,-0.700142},
{-0.309658, 0.700142,-0.213218},{-0.477485, 0.573484,-0.273263},{ 0.081884, 0.787144,-0.072446},{-0.081884, 0.787144, 0.072446},
{ 0.360265, 0.618258,-0.345709},{ 0.426878, 0.655368,-0.140772},{ 0.309658,-0.700142,-0.213218},{ 0.477485,-0.573484,-0.273263},
{-0.081884,-0.787144,-0.072446},{ 0.081884,-0.787144, 0.072446},{-0.360265,-0.618258,-0.345709},{-0.426878,-0.655368,-0.140772},
{-0.140772,-0.426878,-0.655368},{-0.345709,-0.360265,-0.618258},{ 0.273263,-0.477485,-0.573484},{ 0.213218,-0.309658,-0.700142},
{ 0.458793, 0.458793, 0.458793},{ 0.458793, 0.458793,-0.458793},{ 0.458793,-0.458793, 0.458793},{ 0.458793,-0.458793,-0.458793},
{-0.458793, 0.458793, 0.458793},{-0.458793, 0.458793,-0.458793},{-0.458793,-0.458793, 0.458793},{-0.458793,-0.458793,-0.458793},
{ 0.283550, 0.742344, 0.0},{-0.283550, 0.742344,-0.0},{ 0.283550,-0.742344,-0.0},{-0.283550,-0.742344, 0.0},
{ 0.742344, 0.0, 0.283550},{ 0.742344,-0.0,-0.283550},{-0.742344,-0.0, 0.283550},{-0.742344, 0.0,-0.283550},
{ 0.0, 0.283550, 0.742344},{-0.0,-0.283550, 0.742344},{-0.0, 0.283550,-0.742344},{ 0.0,-0.283550,-0.742344}};
#define NE6  300
const uint8_t e6[NE6][2] = {
{0,37},{18,19},{0,19},{37,72},{18,72},{0,19},{12,13},{0,13},{19,84},{12,84},{0,13},{20,21},{0,21},{13,85},{20,85},{0,21},{60,61},{0,61},
{21,73},{60,73},{0,61},{36,37},{0,37},{61,80},{36,80},{1,25},{38,39},{1,39},{25,76},{38,76},{1,39},{56,57},{1,57},{39,81},{56,81},{1,57},
{34,35},{1,35},{57,77},{34,77},{1,35},{14,15},{1,15},{35,87},{14,87},{1,15},{24,25},{1,25},{15,86},{24,86},{2,17},{50,51},{2,51},{17,74},
{50,74},{2,51},{62,63},{2,63},{51,82},{62,82},{2,63},{22,23},{2,23},{63,75},{22,75},{2,23},{12,13},{2,12},{23,85},{13,85},{2,12},{16,17},
{2,17},{12,84},{16,84},{3,49},{26,27},{3,27},{49,78},{26,78},{3,27},{14,15},{3,14},{27,86},{15,86},{3,14},{32,33},{3,33},{14,87},{32,87},
{3,33},{66,67},{3,67},{33,79},{66,79},{3,67},{48,49},{3,49},{67,83},{48,83},{4,18},{42,43},{4,43},{18,72},{42,72},{4,43},{28,29},{4,29},
{43,88},{28,88},{4,29},{44,45},{4,45},{29,89},{44,89},{4,45},{16,17},{4,16},{45,74},{17,74},{4,16},{18,19},{4,18},{16,84},{19,84},{5,53},
{20,21},{5,20},{53,73},{21,73},{5,20},{22,23},{5,22},{20,85},{23,85},{5,22},{70,71},{5,71},{22,75},{70,75},{5,71},{30,31},{5,31},{71,91},
{30,91},{5,31},{52,53},{5,53},{31,90},{52,90},{6,41},{24,25},{6,24},{41,76},{25,76},{6,24},{26,27},{6,26},{24,86},{27,86},{6,26},{46,47},
{6,47},{26,78},{46,78},{6,47},{28,29},{6,28},{47,89},{29,89},{6,28},{40,41},{6,41},{28,88},{40,88},{7,34},{54,55},{7,55},{34,77},{54,77},
{7,55},{30,31},{7,30},{55,90},{31,90},{7,30},{68,69},{7,69},{30,91},{68,91},{7,69},{32,33},{7,32},{69,79},{33,79},{7,32},{34,35},{7,34},
{32,87},{35,87},{8,42},{36,37},{8,36},{42,72},{37,72},{8,36},{58,59},{8,59},{36,80},{58,80},{8,59},{38,39},{8,38},{59,81},{39,81},{8,38},
{40,41},{8,40},{38,76},{41,76},{8,40},{42,43},{8,42},{40,88},{43,88},{9,50},{44,45},{9,44},{50,74},{45,74},{9,44},{46,47},{9,46},{44,89},
{47,89},{9,46},{48,49},{9,48},{46,78},{49,78},{9,48},{64,65},{9,65},{48,83},{64,83},{9,65},{50,51},{9,50},{65,82},{51,82},{10,60},{52,53},
{10,52},{60,73},{53,73},{10,52},{54,55},{10,54},{52,90},{55,90},{10,54},{56,57},{10,56},{54,77},{57,77},{10,56},{58,59},{10,58},{56,81},
{59,81},{10,58},{60,61},{10,60},{58,80},{61,80},{11,70},{62,63},{11,62},{70,75},{63,75},{11,62},{64,65},{11,64},{62,82},{65,82},{11,64},
{66,67},{11,66},{64,83},{67,83},{11,66},{68,69},{11,68},{66,79},{69,79},{11,68},{70,71},{11,70},{68,91},{71,91}};


//  Snub Dodecahedron  20+60 /3|  12 <5]

#define NP5  60
const static float p5[NP5][3] = {
{-0.660604,-0.442642,-0.340185},{-0.841316,-0.150243,-0.132925},{-0.793530,-0.227563,0.258032},{-0.583284,-0.567748,0.292398},{-0.501131,-0.700674,-0.077320},
{0.583284,-0.567748,-0.292398},{0.501131,-0.700674,0.077320},{0.660604,-0.442642,0.340185},{0.841316,-0.150243,0.132925},{0.793530,-0.227563,-0.258032},
{-0.583284,0.567748,-0.292398},{-0.501131,0.700674,0.077320},{-0.660604,0.442642,0.340185},{-0.841316,0.150243,0.132925},{-0.793530,0.227563,-0.258032},
{0.660604,0.442642,-0.340185},{0.841316,0.150243,-0.132925},{0.793530,0.227563,0.258032},{0.583284,0.567748,0.292398},{0.501131,0.700674,-0.077320},
{-0.442642,-0.340185,-0.660604},{-0.150243,-0.132925,-0.841316},{-0.227563,0.258032,-0.793530},{-0.567748,0.292398,-0.583284},{-0.700674,-0.077320,-0.501131},
{-0.567748,-0.292398,0.583284},{-0.700674,0.077320,0.501131},{-0.442642,0.340185,0.660604},{-0.150243,0.132925,0.841316},{-0.227563,-0.258032,0.793530},
{0.567748,-0.292398,-0.583284},{0.700674,0.077320,-0.501131},{0.442642,0.340185,-0.660604},{0.150243,0.132925,-0.841316},{0.227563,-0.258032,-0.793530},
{0.442642,-0.340185,0.660604},{0.150243,-0.132925,0.841316},{0.227563,0.258032,0.793530},{0.567748,0.292398,0.583284},{0.700674,-0.077320,0.501131},
{-0.340185,-0.660604,-0.442642},{-0.132925,-0.841316,-0.150243},{0.258032,-0.793530,-0.227563},{0.292398,-0.583284,-0.567748},{-0.077320,-0.501131,-0.700674},
{-0.292398,0.583284,-0.567748},{0.077320,0.501131,-0.700674},{0.340185,0.660604,-0.442642},{0.132925,0.841316,-0.150243},{-0.258032,0.793530,-0.227563},
{-0.292398,-0.583284,0.567748},{0.077320,-0.501131,0.700674},{0.340185,-0.660604,0.442642},{0.132925,-0.841316,0.150243},{-0.258032,-0.793530,0.227563},
{-0.340185,0.660604,0.442642},{-0.132925,0.841316,0.150243},{0.258032,0.793530,0.227563},{0.292398,0.583284,0.567748},{-0.077320,0.501131,0.700674}};

#define NE5  300
const uint8_t e5[NE5][2] = {
{1,2},{0,4},{0,1},{2,3},{3,4},{6,7},{5,9},{5,6},{7,8},{8,9},{11,12},{10,14},{10,11},{12,13},{13,14},{16,17},{15,19},{15,16},{17,18},
{18,19},{21,22},{20,24},{20,21},{22,23},{23,24},{26,27},{25,29},{25,26},{27,28},{28,29},{31,32},{30,34},{30,31},{32,33},{33,34},{36,37},
{35,39},{35,36},{37,38},{38,39},{41,42},{40,44},{40,41},{42,43},{43,44},{46,47},{45,49},{45,46},{47,48},{48,49},{51,52},{50,54},{50,51},
{52,53},{53,54},{56,57},{55,59},{55,56},{57,58},{58,59},{13,14},{1,13},{1,14},{2,13},{1,2},{1,13},{16,17},{8,16},{8,17},{9,16},{8,9},
{8,16},{23,24},{14,23},{14,24},{10,23},{10,14},{14,23},{20,24},{0,24},{0,20},{1,24},{0,1},{0,24},{25,26},{2,25},{2,26},{3,25},{2,3},
{2,25},{26,27},{12,26},{12,27},{13,26},{12,13},{12,26},{30,31},{9,30},{9,31},{5,30},{5,9},{9,30},{31,32},{15,31},{15,32},{16,31},{15,16},
{15,31},{33,34},{21,33},{21,34},{22,33},{21,22},{21,33},{36,37},{28,36},{28,37},{29,36},{28,29},{28,36},{38,39},{17,38},{17,39},{18,38},
{17,18},{17,38},{35,39},{7,39},{7,35},{8,39},{7,8},{7,39},{40,41},{4,40},{4,41},{0,40},{0,4},{4,40},{42,43},{5,42},{5,43},{6,42},{5,6},
{5,42},{43,44},{34,43},{34,44},{30,43},{30,34},{34,43},{40,44},{20,44},{20,40},{21,44},{20,21},{20,44},{45,46},{22,45},{22,46},{23,45},
{22,23},{22,45},{46,47},{32,46},{32,47},{33,46},{32,33},{32,46},{47,48},{19,47},{19,48},{15,47},{15,19},{19,47},{45,49},{10,49},{10,45},
{11,49},{10,11},{10,49},{50,51},{29,50},{29,51},{25,50},{25,29},{29,50},{51,52},{35,51},{35,52},{36,51},{35,36},{35,51},{52,53},{6,52},
{6,53},{7,52},{6,7},{6,52},{53,54},{41,53},{41,54},{42,53},{41,42},{41,53},{50,54},{3,54},{3,50},{4,54},{3,4},{3,54},{55,56},{11,55},
{11,56},{12,55},{11,12},{11,55},{56,57},{48,56},{48,57},{49,56},{48,49},{48,56},{57,58},{18,57},{18,58},{19,57},{18,19},{18,57},{58,59},
{37,58},{37,59},{38,58},{37,38},{37,58},{55,59},{27,59},{27,55},{28,59},{27,28},{27,59},{0,40},{20,40},{0,20},{3,25},{25,50},{3,50},
{10,23},{23,45},{10,45},{12,55},{27,55},{12,27},{5,30},{30,43},{5,43},{7,52},{35,52},{7,35},{15,47},{32,47},{15,32},{18,38},{38,58},{18,58},
{4,54},{41,54},{4,41},{6,42},{42,53},{6,53},{11,49},{49,56},{11,56},{19,57},{48,57},{19,48},{14,24},{1,14},{1,24},{2,26},{2,13},{13,26},
{9,31},{9,16},{16,31},{17,39},{8,17},{8,39},{34,44},{21,34},{21,44},{22,46},{22,33},{33,46},{29,51},{29,36},{36,51},{37,59},{28,37},{28,59}};
#define NE5b  180
const uint8_t e5b[NE5b][2] = {
{1,13},{1,14},{2,13},{1,13},{8,16},{8,17},{9,16},{8,16},{14,23},{14,24},{10,23},{14,23},{0,24},{0,20},{1,24},{0,24},{2,25},{2,26},
{3,25},{2,25},{12,26},{12,27},{13,26},{12,26},{9,30},{9,31},{5,30},{9,30},{15,31},{15,32},{16,31},{15,31},{21,33},{21,34},{22,33},{21,33},
{28,36},{28,37},{29,36},{28,36},{17,38},{17,39},{18,38},{17,38},{7,39},{7,35},{8,39},{7,39},{4,40},{4,41},{0,40},{4,40},{5,42},{5,43},
{6,42},{5,42},{34,43},{34,44},{30,43},{34,43},{20,44},{20,40},{21,44},{20,44},{22,45},{22,46},{23,45},{22,45},{32,46},{32,47},{33,46},
{32,46},{19,47},{19,48},{15,47},{19,47},{10,49},{10,45},{11,49},{10,49},{29,50},{29,51},{25,50},{29,50},{35,51},{35,52},{36,51},{35,51},
{6,52},{6,53},{7,52},{6,52},{41,53},{41,54},{42,53},{41,53},{3,54},{3,50},{4,54},{3,54},{11,55},{11,56},{12,55},{11,55},{48,56},{48,57},
{49,56},{48,56},{18,57},{18,58},{19,57},{18,57},{37,58},{37,59},{38,58},{37,58},{27,59},{27,55},{28,59},{27,59},{0,40},{20,40},{0,20},
{3,25},{25,50},{3,50},{10,23},{23,45},{10,45},{12,55},{27,55},{12,27},{5,30},{30,43},{5,43},{7,52},{35,52},{7,35},{15,47},{32,47},{15,32},
{18,38},{38,58},{18,58},{4,54},{41,54},{4,41},{6,42},{42,53},{6,53},{11,49},{49,56},{11,56},{19,57},{48,57},{19,48},{14,24},{1,14},{1,24},
{2,26},{2,13},{13,26},{9,31},{9,16},{16,31},{17,39},{8,17},{8,39},{34,44},{21,34},{21,44},{22,46},{22,33},{33,46},{29,51},{29,36},{36,51},
{37,59},{28,37},{28,59}};

#define NF5  80-20
const uint8_t f5[80][3] = {
{14,13,1},{13,2,1},{17,16,8},{16,9,8},{24,23,14},{23,10,14},{20,24,0},{24,1,0},{26,25,2},{25,3,2},
{27,26,12},{26,13,12},{31,30,9},{30,5,9},{32,31,15},{31,16,15},{34,33,21},{33,22,21},{37,36,28},{36,29,28},
{39,38,17},{38,18,17},{35,39,7},{39,8,7},{41,40,4},{40,0,4},{43,42,5},{42,6,5},{44,43,34},{43,30,34},
{40,44,20},{44,21,20},{46,45,22},{45,23,22},{47,46,32},{46,33,32},{48,47,19},{47,15,19},{45,49,10},{49,11,10},
{51,50,29},{50,25,29},{52,51,35},{51,36,35},{53,52,6},{52,7,6},{54,53,41},{53,42,41},{50,54,3},{54,4,3},
{56,55,11},{55,12,11},{57,56,48},{56,49,48},{58,57,18},{57,19,18},{59,58,37},{58,38,37},{55,59,27},{59,28,27},
{0,40,20},{3,25,50},{10,23,45},{12,55,27},{5,30,43},{7,52,35},{15,47,32},{18,38,58},{4,54,41},{6,42,53},
{11,49,56},{19,57,48},{24,14,1},{26,2,13},{31,9,16},{39,17,8},{44,34,21},{46,22,33},{51,29,36},{59,37,28}};
#define NF5b  20
const uint8_t f5b[NF5b][3] = {
{0,40,20},{3,25,50},{10,23,45},{12,55,27},{5,30,43},
{7,52,35},{15,47,32},{18,38,58},{4,54,41},{6,42,53},
{11,49,56},{19,57,48},{24,14,1},{26,2,13},{31,9,16},
{39,17,8},{44,34,21},{46,22,33},{51,29,36},{59,37,28}};


//  Rhombicosidodecahedron  20 /3|  30 [4]  12 <5]

#define NP4  60
const static float p4[NP4][3] = {
{-0.6227790,-0.5038447,-0.3114051},{-0.8152380,-0.1924396,-0.1924589},{-0.8152380,-0.1924396, 0.1924589},{-0.6227790,-0.5038447, 0.3114051},
{-0.5038328,-0.6963038,-0.0},      { 0.6227790,-0.5038447,-0.3114051},{ 0.5038328,-0.6963038,-0.0},      { 0.6227790,-0.5038447, 0.3114051},
{ 0.8152380,-0.1924396, 0.1924589},{ 0.8152380,-0.1924396,-0.1924589},{-0.6227790, 0.5038447,-0.3114051},{-0.5038328, 0.6963038,-0.0},
{-0.6227790, 0.5038447, 0.3114051},{-0.8152380, 0.1924396, 0.1924589},{-0.8152380, 0.1924396,-0.1924589},{ 0.6227790, 0.5038447,-0.3114051},
{ 0.8152380, 0.1924396,-0.1924589},{ 0.8152380, 0.1924396, 0.1924589},{ 0.6227790, 0.5038447, 0.3114051},{ 0.5038328, 0.6963038,-0.0},      
{-0.5038447,-0.3114051,-0.6227790},{-0.1924396,-0.1924589,-0.8152380},{-0.1924396, 0.1924589,-0.8152380},{-0.5038447, 0.3114051,-0.6227790},
{-0.6963038,-0.0,      -0.5038328},{-0.5038447,-0.3114051, 0.6227790},{-0.6963038,-0.0,       0.5038328},{-0.5038447, 0.3114051, 0.6227790},
{-0.1924396, 0.1924589, 0.8152380},{-0.1924396,-0.1924589, 0.8152380},{ 0.5038447,-0.3114051,-0.6227790},{ 0.6963038,-0.0,      -0.5038328},
{ 0.5038447, 0.3114051,-0.6227790},{ 0.1924396, 0.1924589,-0.8152380},{ 0.1924396,-0.1924589,-0.8152380},{ 0.5038447,-0.3114051, 0.6227790},
{ 0.1924396,-0.1924589, 0.8152380},{ 0.1924396, 0.1924589, 0.8152380},{ 0.5038447, 0.3114051, 0.6227790},{ 0.6963038,-0.0,       0.5038328},
{-0.3114051,-0.6227790,-0.5038447},{-0.1924589,-0.8152380,-0.1924396},{ 0.1924589,-0.8152380,-0.1924396},{ 0.3114051,-0.6227790,-0.5038447},
{-0.0,      -0.5038328,-0.6963038},{-0.3114051, 0.6227790,-0.5038447},{-0.0,       0.5038328,-0.6963038},{ 0.3114051, 0.6227790,-0.5038447},
{ 0.1924589, 0.8152380,-0.1924396},{-0.1924589, 0.8152380,-0.1924396},{-0.3114051,-0.6227790, 0.5038447},{-0.0,      -0.5038328, 0.6963038},
{ 0.3114051,-0.6227790, 0.5038447},{ 0.1924589,-0.8152380, 0.1924396},{-0.1924589,-0.8152380, 0.1924396},{-0.3114051, 0.6227790, 0.5038447},
{-0.1924589, 0.8152380, 0.1924396},{ 0.1924589, 0.8152380, 0.1924396},{ 0.3114051, 0.6227790, 0.5038447},{-0.0,       0.5038328, 0.6963038}};
#define NE4  240
const uint8_t e4[NE4][2] = {
{1,2},{0,4},{0,1},{2,3},{3,4},{6,7},{5,9},{5,6},{7,8},{8,9},{11,12},{10,14},{10,11},{12,13},{13,14},{16,17},{15,19},{15,16},{17,18},
{18,19},{21,22},{20,24},{20,21},{22,23},{23,24},{26,27},{25,29},{25,26},{27,28},{28,29},{31,32},{30,34},{30,31},{32,33},{33,34},{36,37},
{35,39},{35,36},{37,38},{38,39},{41,42},{40,44},{40,41},{42,43},{43,44},{46,47},{45,49},{45,46},{47,48},{48,49},{51,52},{50,54},{50,51},
{52,53},{53,54},{56,57},{55,59},{55,56},{57,58},{58,59},{13,14},{2,13},{1,2},{1,14},{16,17},{9,16},{8,9},{8,17},{23,24},{10,23},{10,14},
{14,24},{20,24},{1,24},{0,1},{0,20},{25,26},{3,25},{2,3},{2,26},{26,27},{13,26},{12,13},{12,27},{30,31},{5,30},{5,9},{9,31},{31,32},
{16,31},{15,16},{15,32},{33,34},{22,33},{21,22},{21,34},{36,37},{29,36},{28,29},{28,37},{38,39},{18,38},{17,18},{17,39},{35,39},{8,39},
{7,8},{7,35},{40,41},{0,40},{0,4},{4,41},{42,43},{6,42},{5,6},{5,43},{43,44},{30,43},{30,34},{34,44},{40,44},{21,44},{20,21},{20,40},
{45,46},{23,45},{22,23},{22,46},{46,47},{33,46},{32,33},{32,47},{47,48},{15,47},{15,19},{19,48},{45,49},{11,49},{10,11},{10,45},{50,51},
{25,50},{25,29},{29,51},{51,52},{36,51},{35,36},{35,52},{52,53},{7,52},{6,7},{6,53},{53,54},{42,53},{41,42},{41,54},{50,54},{4,54},{3,4},
{3,50},{55,56},{12,55},{11,12},{11,56},{56,57},{49,56},{48,49},{48,57},{57,58},{19,57},{18,19},{18,58},{58,59},{38,58},{37,38},{37,59},
{55,59},{28,59},{27,28},{27,55},{0,40},{20,40},{0,20},{3,25},{25,50},{3,50},{10,23},{23,45},{10,45},{12,55},{27,55},{12,27},{5,30},{30,43},
{5,43},{7,52},{35,52},{7,35},{15,47},{32,47},{15,32},{18,38},{38,58},{18,58},{4,54},{41,54},{4,41},{6,42},{42,53},{6,53},{11,49},{49,56},
{11,56},{19,57},{48,57},{19,48},{14,24},{1,14},{1,24},{2,26},{2,13},{13,26},{9,31},{9,16},{16,31},{17,39},{8,17},{8,39},{34,44},{21,34},
{21,44},{22,46},{22,33},{33,46},{29,51},{29,36},{36,51},{37,59},{28,37},{28,59}};
#define NF4  20
const uint8_t f4[NF4][3] = {
{0,40,20},{3,25,50},{10,23,45},{12,55,27},{5,30,43},
{7,52,35},{15,47,32},{18,38,58},{4,54,41},{6,42,53},
{11,49,56},{19,57,48},{24,14,1},{26,2,13},{31,9,16},
{39,17,8},{44,34,21},{46,22,33},{51,29,36},{59,37,28}};

//  Icosidodecahedron  20 /3|  12 <5]

#define NP3  30
const static float p3[NP3][3] = {
{ 0.2886751, 0.4670861, 0.7557612},{ 0.7557612, 0.2886751, 0.4670861},{ 0.4670861, 0.7557612, 0.2886751},{ 0.7557612, 0.2886751,-0.4670861},
{ 0.2886751, 0.4670861,-0.7557612},{ 0.4670861, 0.7557612,-0.2886751},{ 0.2886751,-0.4670861, 0.7557612},{ 0.4670861,-0.7557612, 0.2886751},
{ 0.7557612,-0.2886751, 0.4670861},{ 0.4670861,-0.7557612,-0.2886751},{ 0.2886751,-0.4670861,-0.7557612},{ 0.7557612,-0.2886751,-0.4670861},
{-0.4670861, 0.7557612, 0.2886751},{-0.7557612, 0.2886751, 0.4670861},{-0.2886751, 0.4670861, 0.7557612},{-0.2886751, 0.4670861,-0.7557612},
{-0.7557612, 0.2886751,-0.4670861},{-0.4670861, 0.7557612,-0.2886751},{-0.7557612,-0.2886751, 0.4670861},{-0.4670861,-0.7557612, 0.2886751},
{-0.2886751,-0.4670861, 0.7557612},{-0.2886751,-0.4670861,-0.7557612},{-0.4670861,-0.7557612,-0.2886751},{-0.7557612,-0.2886751,-0.4670861},
{ 0.0, 0.9341723, 0.0},{ 0.0,-0.9341723, 0.0},{ 0.9341723, 0.0, 0.0},
{-0.9341723, 0.0, 0.0},{ 0.0, 0.0, 0.9341723},{ 0.0, 0.0,-0.9341723}};
#define NE3  300
const uint8_t e3[NE3][2] = {
{0,1},{1,2},{0,2},{3,4},{4,5},{3,5},{6,7},{7,8},{6,8},{9,10},{10,11},{9,11},{12,13},{13,14},{12,14},{15,16},{16,17},{15,17},
{18,19},{19,20},{18,20},{21,22},{22,23},{21,23},{2,5},{5,24},{2,24},{17,24},{12,17},{12,24},{7,25},{9,25},{7,9},{19,22},{22,25},
{19,25},{1,8},{8,26},{1,26},{11,26},{3,11},{3,26},{13,27},{18,27},{13,18},{16,23},{23,27},{16,27},{14,28},{0,28},{0,14},{20,28},{6,20},
{6,28},{4,29},{15,29},{4,15},{10,21},{21,29},{10,29},{2,24},{0,14},{0,2},{12,24},{12,14},{1,26},{2,5},{1,2},{3,26},{3,5},{0,28},{1,8},
{0,1},{6,28},{6,8},{4,5},{17,24},{5,24},{4,15},{15,17},{7,8},{11,26},{8,26},{7,9},{9,11},{13,14},{20,28},{14,28},{13,18},{18,20},{16,17},
{12,13},{12,17},{16,27},{13,27},{19,25},{6,20},{19,20},{7,25},{6,7},{10,29},{3,11},{10,11},{4,29},{3,4},{16,23},{21,29},{21,23},{15,16},
{15,29},{19,22},{23,27},{22,23},{18,19},{18,27},{10,21},{22,25},{21,22},{9,10},{9,25}};
#define NF3  20
const uint8_t f3[NF3][3] = {
{0,1,2},{3,4,5},{6,7,8},{9,10,11},{12,13,14},{15,16,17},{18,19,20},{21,22,23},{2,5,24},{24,17,12},
{7,25,9},{19,22,25},{1,8,26},{26,11,3},{13,27,18},{16,23,27},{14,28,0},{28,20,6},{4,29,15},{10,21,29}};

//  Icosahedron  20 /3|

#define NP2  12
const static float p2[NP2][3] = {
{ 0.8506507, 0.5257310,0.0},{-0.8506507, 0.5257310,0.0},
{ 0.8506507,-0.5257310,0.0},{-0.8506507,-0.5257310,0.0},
{ 0.5257310,0.0, 0.8506507},{ 0.5257310,0.0,-0.8506507},
{-0.5257310,0.0, 0.8506507},{-0.5257310,0.0,-0.8506507},
{0.0, 0.8506507, 0.5257310},{0.0,-0.8506507, 0.5257310},
{0.0, 0.8506507,-0.5257310},{0.0,-0.8506507,-0.5257310}};
#define NE2  60
const uint8_t e2[NE2][2] = {
{0,8},{4,8},{0,4},{0,5},{5,10},{0,10},{2,4},{4,9},{2,9},{2,11},{5,11},{2,5},{1,6},{6,8},{1,8},{1,10},
{7,10},{1,7},{3,9},{6,9},{3,6},{3,7},{7,11},{3,11},{0,10},{8,10},{0,8},{1,8},{8,10},{1,10},{2,9},{9,11},
{2,11},{3,11},{9,11},{3,9},{2,4},{0,2},{0,4},{0,5},{0,2},{2,5},{1,6},{1,3},{3,6},{3,7},{1,3},{1,7},{6,8},
{4,6},{4,8},{4,9},{4,6},{6,9},{5,10},{5,7},{7,10},{7,11},{5,7},{5,11}};

//  Dodecahedron  12 <5]

#define NP1  20
const static float p1[NP1][3] = {
{ 0.5773502, 0.5773502, 0.5773502},{ 0.5773502, 0.5773502,-0.5773502},
{ 0.5773502,-0.5773502, 0.5773502},{ 0.5773502,-0.5773502,-0.5773502},
{-0.5773502, 0.5773502, 0.5773502},{-0.5773502, 0.5773502,-0.5773502},
{-0.5773502,-0.5773502, 0.5773502},{-0.5773502,-0.5773502,-0.5773502},
{ 0.3568221, 0.9341723,0.0},{-0.3568221, 0.9341723,0.0},
{ 0.3568221,-0.9341723,0.0},{-0.3568221,-0.9341723,0.0},
{ 0.9341723,0.0, 0.3568221},{ 0.9341723,0.0,-0.3568221},
{-0.9341723,0.0, 0.3568221},{-0.9341723,0.0,-0.3568221},
{0.0, 0.3568221, 0.9341723},{0.0,-0.3568221, 0.9341723},
{0.0, 0.3568221,-0.9341723},{0.0,-0.3568221,-0.9341723}};
#define NE1  60
const uint8_t e1[NE1][2] = {
{0,8},{4,16},{0,16},{8,9},{4,9},{0,12},{1,8},{0,8},{12,13},{1,13},{0,16},{2,12},
{0,12},{16,17},{2,17},{1,8},{5,9},{8,9},{1,18},{5,18},{2,12},{3,13},{12,13},{2,10},
{3,10},{4,16},{6,17},{16,17},{4,14},{6,14},{5,9},{4,14},{4,9},{5,15},{14,15},{6,11},
{2,17},{6,17},{10,11},{2,10},{3,19},{1,13},{3,13},{18,19},{1,18},{7,15},{18,19},{7,19},
{5,15},{5,18},{7,11},{14,15},{7,15},{6,11},{6,14},{7,19},{10,11},{7,11},{3,19},{3,10}};

//  Cube  6 [4]

#define NP0  8
const static float p0[NP0][3] = {   // points
{-1,-1,-1},{ 1,-1,-1}, {-1, 1,-1},{ 1, 1,-1},
{-1,-1, 1},{ 1,-1, 1}, {-1, 1, 1},{ 1, 1, 1} };
#define NE0  12
const uint8_t e0[NE0][2] = {   // edges
{0,1},{0,2},{0,4}, {3,1},{3,2},{3,7},
{5,1},{5,4},{5,7}, {6,2},{6,4},{6,7}};


//  ALL  --------
#define ALL  Demos::hdA
#define A(a)  &a[0][0]
const float*   ppA[ALL] = {A(p2),A(p1),A(p3),A(p3),A(p4),A(p4),A(p5),A(p5),A(p5) ,A(p5) ,A(p6)};
const uint8_t* eeA[ALL] = {A(e2),A(e1),A(e3),A(e3),A(e4),A(e4),A(e5),A(e5),A(e5b),A(e5b),A(e6)};
const uint8_t* ffA[ALL] = {    0,    0,    0,A(f3),    0,A(f4),    0,A(f5),    0 ,A(f5b),    0};
const int      NPa[ALL] = {  NP2,  NP1,  NP3,  NP3,  NP4,  NP4,  NP5,  NP5,  NP5 ,  NP5 ,  NP6};
const int      NEa[ALL] = {  NE2,  NE1,  NE3,  NE3,  NE4,  NE4,  NE5,  NE5,  NE5b,  NE5b,  NE6};
const int      NFa[ALL] = {    0,    0,    0,  NF3,    0,  NF4,    0,  NF5,    0 ,  NF5b,    0};
const float    SCa[ALL] = { 1.18, 1.18, 1.26, 1.26, 1.35, 1.35, 1.35, 1.35,  1.35,  1.35, 1.45};
#undef ALL


///  Draw 3D
void Demos::Hedrons(Adafruit_SSD1306& d)
{
	if (hdtOn)
	{	++hdt;
		if (hdt >= hdtMax)
		{	hdt = 0;  ++hdCur;  // next type
			if (hdCur >= hdA)  hdCur = 0;
	}	}
	const int u = hdCur;

	const float SC = SCa[u];  // scale
	float rx=0.f,ry=0.f,rz=0.f, s = SC, ss=1.f;
	switch (hdRot)  //par rot speed
	{	case 0:  rx = t*0.0055f;  ry = t*0.0065f;  rz = t*0.0075f;  break;
		case 1:  rx = t*0.0061f;  ry = t*0.0084f;  rz = t*0.0077f;  break;
		case 2:  rx = t*0.0067f;  ry = t*0.0098f;  rz = t*0.0083f;
			ss = 1.0f - 0.4f * (abs(cos(t*0.0125f)));  break;
		case 3:  ry = t*0.0060f;  rz = t*0.0020f;  break;
	}

	const int NP = NPa[u], NE = NEa[u], NF = NFa[u];
	const float* pp = ppA[u];
	const uint8_t* ee = eeA[u], *ff = ffA[u];

	const float  ///par  z pos  screen scale
		fovy = 30.f, zz = 3.f, se = 2.f,
		fv = 1.f/tan(fovy*0.5f),  // const
		cx = cos(rx), sx = sin(rx), cy = cos(ry), sy = sin(ry),
		cz = cos(rz), sz = sin(rz),
		mx[3][3] = {{1.f,0.f,0.f},{0.f,cx,-sx},{0.f,sx,cx}},  // rot x
		my[3][3] = {{cy,0.f,sy},{0.f,1.f,0.f},{-sy,0.f,cy}},  // y
		mz[3][3] = {{cz,-sz,0.f},{sz,cz,0.f},{0.f,0.f,1.f}};  // z


	//  transform all points
	int16_t px[NP], py[NP];  int8_t c[NP];
	int i, a;
	for (i=0,a=0; i < NP; ++i)
	{
		float x = pp[a++], y = pp[a++], z = pp[a++];
		float v[3];
		v[0] = mx[0][0]*x +mx[0][1]*y +mx[0][2]*z;  // rotate
		v[1] = mx[1][0]*x +mx[1][1]*y +mx[1][2]*z;
		v[2] = mx[2][0]*x +mx[2][1]*y +mx[2][2]*z;
		x = v[0];  y = v[1];  z = v[2];
		v[0] = my[0][0]*x +my[0][1]*y +my[0][2]*z;
		v[1] = my[1][0]*x +my[1][1]*y +my[1][2]*z;
		v[2] = my[2][0]*x +my[2][1]*y +my[2][2]*z;
		x = v[0];  y = v[1];  z = v[2];
		v[0] = mz[0][0]*x +mz[0][1]*y +mz[0][2]*z;
		v[1] = mz[1][0]*x +mz[1][1]*y +mz[1][2]*z;
		v[2] = mz[2][0]*x +mz[2][1]*y +mz[2][2]*z;
		x = v[0] * s;  y = v[1] * s;  z = v[2] * s + ss*zz;

		px[i] = H/2 * se * (fv * x / -z) +W/2;  // pos 3d to 2d
		py[i] = H/2 * se * (fv * y / -z) +H/2;

		c[i] = z < 0.9f ? -1 : (z < zz ? 1 : 0);
		if (px[i]<0 || px[i]>=W || py[i]<0 || py[i]>=H)
			c[i] = -1;  // outside
		//c[i] = z < zz ? 1 : 0;
		//d.drawPixel(px[i], py[i],WHITE);
	}
	
	//  draw far/covered edges  --
	for (i=0,a=0; i < NE; ++i)
	{
		int e0 = ee[a++], e1 = ee[a++];
		int8_t cc = max(c[e0],c[e1]);
		if (cc==0)
			d.drawLine( px[e0],py[e0], px[e1],py[e1], WHITE);
	}

	//  draw far faces  /|
	if (ff)
	for (i=0,a=0; i < NF; ++i)
	{
		int f0 = ff[a++], f1 = ff[a++], f2 = ff[a++];
		int8_t cc = min(c[f0],c[f1]);  cc = min(cc,c[f2]);
		if (cc==0)
			d.fillTriangle( px[f0],py[f0], px[f1],py[f1], px[f2],py[f2], WHITE);
	}

	//  dither 2 full screen  --------
	uint8_t* buf = d.getBuffer();
	for (i=0; i < W*H/8; i+=2)  buf[i] &= 0xAA;
	for (i=1; i < W*H/8; i+=2)  buf[i] &= 0x55;

	//  draw near/visible edges  --
	for (i=0,a=0; i < NE; ++i)
	{
		int e0 = ee[a++], e1 = ee[a++];
		int8_t cc = max(c[e0],c[e1]);
		if (cc>0)
			d.drawLine( px[e0],py[e0], px[e1],py[e1], WHITE);
	}
	
	//  draw near faces  /|
	if (ff)
	for (i=0,a=0; i < NF; ++i)
	{
		int f0 = ff[a++], f1 = ff[a++], f2 = ff[a++];
		int8_t cc = min(c[f0],c[f1]);  cc = min(cc,c[f2]);
		if (cc>0)
			d.fillTriangle( px[f0],py[f0], px[f1],py[f1], px[f2],py[f2], WHITE);
	}

	if (iInfo > 0)
	{
		d.setCursor(0,8);
		d.print("Cur ");  d.println(hdCur);
		d.print("Rot ");  d.println(hdRot);
		if (hdtOn)  d.println("On");
	}
	++t;
	delay(8);
}
