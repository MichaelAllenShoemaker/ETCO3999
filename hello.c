
#include "sprites.h"
#include "neslib.h"
#include "vrambuf.h"
#include "bcd.h"
#include <string.h>
// link the pattern table into CHR ROM
//#link "chr_generic.s"
//#link "vrambuf.c"
#define NES_MIRRORING 1 ("vertical", 0 = "horizontal")

#include "helpers.h"
#include "Maps.h"

typedef struct {
  byte xpos;
  byte ypos;
  signed char dx;
  signed char dy;
} bullet;

#define COLOR_MISSILE		3
#define COLOR_BOMB		2
#define NAME_MISSILE	100
#define YOFFSCREEN 240	// offscreen y position (hidden)
#define xOFFSCREEN 240	// offscreen x position (hidden)
#define PLAYERBULLET 0 // index of the players bullet in the array
// character position
unsigned char player_x = 128;
unsigned char player_y = 55;
//int for direction 0 = right, 1 = down, 2 = left, 3 = up
int playerDirection = 0;
int playerSprite = 0;
int playerFramesToMove = 0;
int framesBetweenChange = 10;

bullet Bullets[10];


// Move Function
void move_player(char pad_result)
{
      //moving the player
    if((pad_result >> 7) & 0x01)
    {
      // moving to the right
      if(player_x+1 < 220)
          player_x += 1;
      playerDirection = 0;
      playerFramesToMove++;
      if(playerFramesToMove == framesBetweenChange)
      {
        playerSprite++;
        playerFramesToMove = 0;
        if(playerSprite == 3)
        	playerSprite = 0;
      }
    }
    
    else if((pad_result >> 6) & 0x01)
    {
      if(player_x-1 > 20)
          player_x -= 1;
      playerDirection = 2;
      playerFramesToMove++;
      if(playerFramesToMove == framesBetweenChange)
      {
        playerSprite++;
        playerFramesToMove = 0;
        if(playerSprite == 3)
        	playerSprite = 0;
      }
    }
    else if((pad_result >> 4) & 0x01)
    {
      //moving up
      if(player_y-1 > 42)
      	player_y -= 1;
      playerDirection = 1;
      playerFramesToMove++;
      if(playerFramesToMove == framesBetweenChange)
      {
        playerSprite++;
        playerFramesToMove = 0;
        if(playerSprite == 3)
        	playerSprite = 0;
      }
    }
    else if((pad_result >> 5) & 0x01)
    {
      //moving down
      if(player_y+1 < 190)
      	player_y += 1;
      playerDirection = 3;
      playerFramesToMove++;
      if(playerFramesToMove == framesBetweenChange)
      {
        playerSprite++;
        playerFramesToMove = 0;
        if(playerSprite == 3)
        	playerSprite = 0;
      }
    }
    else
    {
      	playerSprite = 0;
    }
    //If player presses space shoot a bullet in the direction they are walking
    if(pad_result & 0x01 && Bullets[PLAYERBULLET].ypos == YOFFSCREEN)
    {
      Bullets[PLAYERBULLET].ypos = player_y-8; // must be multiple of missile speed
      Bullets[PLAYERBULLET].xpos = player_x+4; // player X position
      //int for direction 0 = right, 1 = down, 2 = left, 3 = up
      if(playerDirection == 0)
      {
      	Bullets[PLAYERBULLET].dy = 0; // player missile speed
     	Bullets[PLAYERBULLET].dx = 4; // player missile speed
      }
      else if(playerDirection == 1)
      {
      	Bullets[PLAYERBULLET].dy = -4; // player missile speed
     	Bullets[PLAYERBULLET].dx = 0; // player missile speed
      }
      else if(playerDirection == 2)
      {
      	Bullets[PLAYERBULLET].dy = 0; // player missile speed
     	Bullets[PLAYERBULLET].dx = -4; // player missile speed
      }
      else
      {
        Bullets[PLAYERBULLET].dy = 4; // player missile speed
     	Bullets[PLAYERBULLET].dx = 0; // player missile speed
      }
    }
}

void move_bullets() {
  byte i;
  for (i=0; i<10; i++) { 
    if (Bullets[i].ypos != YOFFSCREEN && Bullets[i].xpos != xOFFSCREEN) {
      // hit the bottom or top?
      if ((byte)(Bullets[i].ypos += Bullets[i].dy) < 35) {
        Bullets[i].ypos = YOFFSCREEN;
      }
      if ((byte)(Bullets[i].xpos += Bullets[i].dx) < 10 || Bullets[i].xpos > 235) {
        Bullets[i].xpos = xOFFSCREEN;
        Bullets[i].ypos = YOFFSCREEN;
      }
    }
  }
}

void fade_in() {
  byte vb;
  for (vb=0; vb<=4; vb++) {
    // set virtual bright value
    pal_bright(vb);
    // wait for 4/60 sec
    ppu_wait_frame();
    ppu_wait_frame();
    ppu_wait_frame();
    ppu_wait_frame();
    ppu_wait_frame();
    ppu_wait_frame();
  }
}


#include "setup.h"
// main function, run after console reset
void main(void) 
{
  setup();
  fade_in();
  
  // game loop
  while (1)
  {
    // do this at the start of each frame
    int oam_id = 0;
    byte i = 0;
    // getting player input
    char pad_result = pad_poll(0); 
    ppu_wait_frame();
    // set sprite 0
    oam_id = oam_spr(1, 38, 0xa4, 0, 0);
    move_player(pad_result);
    
    if(playerDirection == 2)
    	oam_id = oam_meta_spr(player_x, player_y, oam_id, PlayerSprite[playerSprite+4]);
    else
      oam_id = oam_meta_spr(player_x, player_y, oam_id, PlayerSprite[playerSprite]);
     move_bullets();
    for (i=0; i<10; i++) {
    	bullet* mis = &Bullets[i];
    	if (mis->ypos != YOFFSCREEN) {
      		oam_id = oam_spr(mis->xpos, mis->ypos, 0x16,
                      0x02,
                      oam_id);
    }
  }
    
    // Do this to "hide" any remaining sprites
    oam_hide_rest(oam_id);
  }
}
