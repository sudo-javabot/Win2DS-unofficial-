//Heavily modified soft keyboard by Sintax
//Original created by headspin

//Note that this may be difficult to use in your own project, as I moved around the tiles and tilemaps
//In order to make everything fit into VRAM. It also uses the palette in slot 1.

//It is modified to work better with the touchscreen, including an attempt to scan the last
//10 keys touched (over 10 frames) and find which one was used most in order to find the right one and ignore jumps.

#include <stdio.h>
#include <string.h>
#include <nds.h>
#include <nds/arm9/console.h>	//basic print funcionality
#include <nds/arm9/sound.h>	// sound functions

#include "keyboard.def.h"
#include "keyboard.raw.h"
#include "keyboard.map.h"
#include "keyboard.pal.h"
#include "keyboard.hit.h"

#include "clickdown_raw.h"
#include "clickup_raw.h"

#define PEN_DOWN (~IPC->buttons & (1 << 6))

#define KB_NORMAL 0
#define KB_CAPS   1
#define KB_SHIFT  2

#define ECHO_ON	 0
#define ECHO_OFF 1

/*#define BSP	0x8 // Backspace
#define CAP	0x2 // Caps
#define RET	'\n' // Enter
#define SHF	0x4 // Shift*/

static int g_dx=0;
static int g_dy=0;
static u16 g_MouseDown = false;
static int g_Mode = KB_NORMAL;
static int g_col = 0;
//static int g_delay = 0;

static unsigned int lasttilex=0, lasttiley=0;
static char lastkey = 0x0;
char lastKeys[10] = {0,0,0,0,0,0,0,0,0,0};
char findKey;
u8 keyCount = 0;

void setTile(uint16 *map, int x, int y, int pal)
{
	char c;
	int x2, y2;
	
	pal++;

	c = keyboard_Hit[(y*32)+x];

	if(!c) return;

	map[(y*32)+x] &= ~(3 << 12);
	map[(y*32)+x] |= (pal << 12);

	x2 = x; y2 = y;
	while(keyboard_Hit[(y2*32)+x2]==c)
	{
		map[(y2*32)+x2] &= ~(3 << 12);
		map[(y2*32)+x2] |= (pal << 12);

		x2 = x;
		while(keyboard_Hit[(y2*32)+x2]==c) { map[(y2*32)+x2] &= ~(3 << 12); map[(y2*32)+x2] |= (pal << 12); x2++; }
		x2 = x;
		while(keyboard_Hit[(y2*32)+x2]==c) { map[(y2*32)+x2] &= ~(3 << 12); map[(y2*32)+x2] |= (pal << 12); x2--; }

		x2 = x;
		y2++;
	}

	x2 = x; y2 = y;
	while(keyboard_Hit[(y2*32)+x2]==c)
	{
		map[(y2*32)+x2] &= ~(3 << 12);
		map[(y2*32)+x2] |= (pal << 12);

		x2 = x;
		while(keyboard_Hit[(y2*32)+x2]==c) { map[(y2*32)+x2] &= ~(3 << 12); map[(y2*32)+x2] |= (pal << 12); x2++; }
		x2 = x;
		while(keyboard_Hit[(y2*32)+x2]==c) { map[(y2*32)+x2] &= ~(3 << 12); map[(y2*32)+x2] |= (pal << 12); x2--; }

		x2 = x;
		y2--;
	}
}

void initKeyboard()
{
	//set the mode for 2 text layers and two extended background layers
	//videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE);

//	BG0_CR = BG_COLOR_16 | BG_32x32 | (29 << SCREEN_SHIFT) | (1 << CHAR_SHIFT);
	// SUB_BG0_CR = BG_COLOR_256 | BG_32x32 | (0 << SCREEN_SHIFT) | (1 << CHAR_SHIFT);	

	dmaCopy((uint16 *)keyboard_Palette, (uint16 *)BG_PALETTE+16, 32);
	dmaCopy((uint16 *)keyboard_Palette_Hilight, (uint16 *)BG_PALETTE+32, 32);
	dmaCopy((uint16 *)keyboard_Tiles, (uint16 *)CHAR_BASE_BLOCK(1), 13760);
	
	g_dx=0;
	g_dy=0;
	g_MouseDown = false;
	g_Mode = KB_NORMAL;
	g_col = 0;

	lasttilex=0, lasttiley=0;
	lastkey = 0x0;
}

void showKeyboard (bool show) {
	uint16 * map = (uint16 *) SCREEN_BASE_BLOCK(5)+128; 
	int i = -1;

	if (show)
		while(i++ <= (32 * 14)) map[i] = keyboard_Map[i]|(1<<12);
	else
		while(i++ <= (32 * 14)) map[i] = 0;
}

// returns - last key pressed
// str - pointer to string to store output
// max - maximum number of characters
// echo - echo input to console
char processKeyboard(char* str, unsigned int max, unsigned int echo)
{
	int i, j;
	touchPosition touchXY;
	
	//get the map
	uint16 * map = (uint16 *) SCREEN_BASE_BLOCK(5)+128; 

	touchXY=touchReadXY();

	if(PEN_DOWN && !g_MouseDown)
	{
/*		g_delay++;
		if (g_delay < 2)
			return 0;

		g_delay = 0;*/

		g_dx = touchXY.px;
		g_dy = touchXY.py;
		g_MouseDown = TRUE;

		for (i = 0; i < 10; i++)
			lastKeys[i] = 0;

	}
	else if(PEN_DOWN && g_MouseDown)
	{
		int z1, z2;
		
		i = touchXY.px;
		j = touchXY.py-32;
		if (j < 0)
			j = 0;
		
		z1 = IPC->touchZ1;
		z2 = IPC->touchZ2;

		// This is the old method I used to check for pen jumps
		//if(ABS(i-g_dx)<MOVE_MAX && ABS(j-g_dy)<MOVE_MAX)

		// This is checking z registers are not zero for pen jumping
		if(z1!=0 && z2!=0)
		{
			g_dx = i;
			g_dy = j;

			unsigned int tilex, tiley;

			tilex = g_dx/8;
			tiley = g_dy/8;

			if(tilex>=2 && tilex<=29 && tiley<=12)
			{
				char c;

				if(g_Mode==KB_NORMAL)
					c = keyboard_Hit[tilex+(tiley*32)];
				else
					c = keyboard_Hit_Shift[tilex+(tiley*32)];

//				if(lastkey != c)				
//					playGenericSound(clickdown_raw, clickdown_raw_size);
				
				if (lasttilex != tilex || lasttiley != tiley) {
					setTile(map, lasttilex, lasttiley, 0);
					setTile(map, tilex, tiley, 1);
				}
				
				lastkey = c; lasttilex = tilex; lasttiley = tiley;

				for (i = 0; i < 9; i++)
					lastKeys[i] = lastKeys[i+1];
				lastKeys[9] = c;

			}
		} else g_MouseDown = FALSE;
	} else if((!PEN_DOWN && g_MouseDown) || ((!PEN_DOWN && !g_MouseDown) && lastkey != 0)) {
		char c, buf[2];

		unsigned int tilex, tiley;

		g_MouseDown = FALSE;
		
		tilex = g_dx/8;
		tiley = g_dy/8;

		if(tilex>=2 && tilex<=29 && tiley<=12)
		{
			if(g_Mode==KB_NORMAL)
				c = keyboard_Hit[tilex+(tiley*32)];
			else
				c = keyboard_Hit_Shift[tilex+(tiley*32)];

			playGenericSound(clickup_raw, clickup_raw_size);
			setTile(map, lasttilex, lasttiley, 0);

			lastkey = 0; lasttilex = 0; lasttiley = 0;

			buf[0] = c;
			buf[1] = (char) NULL;

			if(c==RET) // Return
			{
				if(echo==ECHO_ON)
				{
					iprintf("\n");
					g_col = 0;
				}
			} else
			if(c==BSP) // Backspace
			{
				if(strlen(str)>0)
				{
					if(echo==ECHO_ON)
					{
						if(g_col == 0)
						{
							g_col = 31;
							iprintf("\x1b[1A\x1b[31C \x1b[1D");
						} else { g_col--; iprintf("\x1b[1D \x1b[1D"); }
					}
					str[strlen(str)-1] = (char) NULL;
					//iprintf("\x1b[s\x1b[0;0f%02d:%02d\x1b[u",g_col,strlen(str));
				}
				
			} else
			if(c==CAP) // Caps
			{
				lasttilex = 0; lasttiley = 0;
				if(g_Mode==KB_NORMAL) {
					//dmaCopy((uint16 *)keyboard_Map+512,(uint16 *)map, 1024);
					i = 0; while(i++ <= (32 * 14)) map[i] = keyboard_Map[i+512]|(1<<12);

					map[(7*32)+1] |= (1 << 12);
					map[(7*32)+2] |= (1 << 12);
					map[(8*32)+1] |= (1 << 12);
					map[(8*32)+2] |= (1 << 12);

					g_Mode = KB_CAPS;
				} else {
					//dmaCopy((uint16 *)keyboard_Map,(uint16 *)map, 1024);
					i = 0; while(i++ <= (32 * 14)) map[i] = keyboard_Map[i]|(1<<12);
					g_Mode = KB_NORMAL;
				}
			} else
			if(c==SHF) // Shift
			{
				lasttilex = 0; lasttiley = 0;
				if(g_Mode==KB_NORMAL) {
					//dmaCopy((uint16 *)keyboard_Map+512,(uint16 *)map, 1024);
					i = 0; while(i++ <= (32 * 14)) map[i] = keyboard_Map[i+512]|(1<<12);

					map[(9*32)+1] |= (1 << 12);
					map[(9*32)+2] |= (1 << 12);
					map[(9*32)+3] |= (1 << 12);
					map[(10*32)+1] |= (1 << 12);
					map[(10*32)+2] |= (1 << 12);
					map[(10*32)+3] |= (1 << 12);

					map[(9*32)+24] |= (1 << 12);
					map[(9*32)+25] |= (1 << 12);
					map[(9*32)+26] |= (1 << 12);
					map[(10*32)+24] |= (1 << 12);
					map[(10*32)+25] |= (1 << 12);
					map[(10*32)+26] |= (1 << 12);
					g_Mode = KB_SHIFT;
				} else {
					//dmaCopy((uint16 *)keyboard_Map,(uint16 *)map, 1024);
					i = 0; while(i++ <= (32 * 14)) map[i] = keyboard_Map[i]|(1<<12);
					g_Mode = KB_NORMAL;
				}
			} else 
			{
				if(strlen(str)<max-1 && (c>=32 && c<=126)) {
					strcat(str, buf);
					if(echo==ECHO_ON)
					{
						iprintf("%c",c);
						g_col++;
						if(g_col == 33) g_col = 1;
					}
					//iprintf("\x1b[s\x1b[0;0f%02d:%02d\x1b[u",g_col,strlen(str));
				}
				if(g_Mode == KB_SHIFT) // Undo Shift
				{
					//dmaCopy((uint16 *)keyboard_Map,(uint16 *)map, 1024);
					i = 0; while(i++ <= (32 * 14)) map[i] = keyboard_Map[i]|(1<<12);
					g_Mode = KB_NORMAL;
				}
			}

			findKey = c;
			for (i = 9; i  >= 0 && keyCount < 2; i--) {
				if (lastKeys[i] != findKey) {
					findKey = lastKeys[i];
					keyCount = 0;
				} else {
					keyCount++;
				}
			}
			
			if (keyCount >= 2 && findKey != 0)
				c = findKey;
			

			return c;
		} else
		{
			setTile(map, lasttilex, lasttiley, 0);

			lastkey = 0; lasttilex = 0; lasttiley = 0;
		}
	}
	
	return 0;
}
