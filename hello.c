
#include "sprites.h"
#include "neslib.h"
#include "vrambuf.h"
#include "bcd.h"
#include <string.h>


// link the pattern table into CHR ROM
//#link "chr_generic.s"
//#link "vrambuf.c"
#define NES_MIRRORING 0 ("vertical", 0 = "horizontal")

// famitone2 library
//#link "famitone2.s"

// music and sfx
//#link "MySongDump.s"
extern char MySongDump_music_data[];
//#link "demosounds.s"
extern char demo_sounds[];

// indices of sound effects (0..3)
typedef enum { SND_START, SND_HIT, SND_COIN, SND_JUMP } SFXIndex;


typedef struct {
  byte xpos;
  byte ypos;
  signed char dx;
  signed char dy;
} bullet;

typedef struct {
  byte xpos;
  byte ypos;
  bool current;
} mapPiece;

typedef struct
{
  byte xpos;
  byte ypos;
} bomb;

#define COLOR_MISSILE		3
#define COLOR_BOMB		2
#define NAME_MISSILE	100
#define YOFFSCREEN 240	// offscreen y position (hidden)
#define xOFFSCREEN 240	// offscreen x position (hidden)
#define PLAYERBULLET 0 // index of the players bullet in the array

// character position
unsigned char player_x = 120;
unsigned char player_y = 120;

//int for direction 0 = right, 1 = down, 2 = left, 3 = up
int playerDirection = 0;
int playerSprite = 0;
int playerFramesToMove = 0;
int framesBetweenChange = 10;

//GameOver Stuff
static int iy,dy;
static unsigned char frame_cnt;
static unsigned char bright;
static unsigned char wait;
static unsigned char i;

bool titleScreen = true;
bool canGoRight = true;
bool canGoUp = false;
bool canGoLeft = false;
bool canGoDown = false;
bool canMove = true;
bool dead = false;
bool canChangeMap = true;
int numberTillDrop = 10;

bullet Bullets[10];
mapPiece Map[9];
bomb Bombs[1];


#include "Maps.h"
#include "helpers.h"
#include "CollisionHelper.h"
#include "setup.h"

void setup_sounds() {
  famitone_init(MySongDump_music_data);
  sfx_init(demo_sounds);
  nmi_set_callback(famitone_update);
}

// Move Function
void move_player(char pad_result)
{
      //moving the player
    if((pad_result >> 7) & 0x01)
    {
      // moving to the right
      if(player_x+1 < 220 && checkCollision(player_x+1, player_y))
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
      
      if(canChangeMap && canGoRight && player_x > 218)
      {
        fade_out();
        change_Map(0);
        fade_in();
      }
    }
    
    else if((pad_result >> 6) & 0x01)
    {
      if(player_x-1 > 20 && checkCollision(player_x-1, player_y))
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
      
      if(canChangeMap && canGoLeft && player_x < 22)
      {
        fade_out();
        change_Map(2);
        fade_in();;
      }
    }
    else if((pad_result >> 4) & 0x01)
    {
      //moving up
      if(player_y-1 > 42 && checkCollision(player_x, player_y-1))
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
      
      if(canChangeMap && canGoUp && player_y < 44)
      {
        fade_out();
        change_Map(1);
        fade_in();
      }
    }
    else if((pad_result >> 5) & 0x01)
    {
      //moving down
      if(player_y+1 < 190 && checkCollision(player_x, player_y + 1))
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
      
      if(canChangeMap && canGoDown && player_y > 188)
      {
        fade_out();
        change_Map(3);
        fade_in();
      }
    }
    else
    {
      	playerSprite = 0;
    }
    //If player presses space shoot a bullet in the direction they are walking
    if(pad_result & 0x01 && Bullets[PLAYERBULLET].ypos == YOFFSCREEN)
    {
      Bullets[PLAYERBULLET].ypos = player_y+6; // must be multiple of missile speed
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
  
    //Bombs
     if(bombs > 0 && (pad_result >> 1) & 0x01)
     {
       bombs--;
       reset_enemies();
       bombScreen();
     }
    //Hit triggers
    if(!secretUnlocked && map == 9 && player_x > 112 && player_x < 128 && player_y > 104 && player_y < 117)
    {
      secretUnlocked = true;
    }
    if(!haveLader && map == 13 && player_x > 112 && player_x < 128 && player_y > 104 && player_y < 117)
    {
      haveLader = true;
    }
}

void move_bullets() {
  byte i;
  byte j;
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
      for (j=0; j<6; j++)
      {
         if (Enemies[j].xpos != xOFFSCREEN && iabs(Bullets[i].ypos - Enemies[j].ypos) < 16 &&  iabs(Bullets[i].xpos - Enemies[j].xpos < 16))
         {
           // We killed an enemy
           numEnemies--;
           Enemies[j].xpos = xOFFSCREEN;
           Enemies[j].ypos = YOFFSCREEN;
           Enemies[j].dx = 0;
           Enemies[j].dy = 0;
           Enemies[j].health = 0;
           numberTillDrop--;
           if(numberTillDrop == 0)
           {
             // We drop a bomb
             numberTillDrop = 10;
             Bombs[0].xpos = 120;
             Bombs[0].ypos = 120;
           }
           
         }
      }
    }
  }
}

void moveEnemies()
{
  byte i;
  canChangeMap = true;
  for (i=0; i<6; i++) { 
    if (Enemies[i].ypos != YOFFSCREEN && Enemies[i].xpos != xOFFSCREEN) {
      canChangeMap = false;
      Enemies[i].xpos += Enemies[i].dx;
      Enemies[i].ypos += Enemies[i].dy;
      if(Enemies[i].xpos > 220 || Enemies[i].xpos < 20)
      {
        Enemies[i].dx = -Enemies[i].dx;
      }
      if(Enemies[i].ypos > 190 || Enemies[i].ypos < 60)
      {
        Enemies[i].dy = -Enemies[i].dy;
      }
      enemyCollision(Enemies[i].xpos, Enemies[i].ypos);
    }
  }
}

void runGame()
{
  setup_sounds();		// init famitone library
  sfx_play(SND_START,0);	// play starting sound
  music_play(0);		// start the music
  title();
  fade_in();
  while(titleScreen)
  {   
    char pad_result = pad_poll(0); 
    if((pad_result >> 3) & 0x01)
    {
      fade_out();
      titleScreen = false;
    }
  }
  
  setup();
  fade_in();
  
  
  // game loop
  while (1)
  {
    if(!dead)
    {
    // do this at the start of each frame
    int oam_id = 0;
    byte i = 0;
    // getting player input
    char pad_result = pad_poll(0); 
    // set sprite 0
    oam_id = oam_spr(1, 38, 0xa4, 0, 0);
    
    if(iframes > 0)
      iframes--;
    
    // flash effect
    if (vbright > 4) {
      pal_bright(--vbright);
    }
    
    if(canMove)
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
    
    moveEnemies();
    for (i=0; i<6; i++) {
    	enemy* ene = &Enemies[i];
    	if (ene->ypos != YOFFSCREEN) {
      		oam_id = oam_spr(ene->xpos, ene->ypos, 0xB6, 0x00,oam_id);
        }
    }
    if(Bombs[0].xpos != xOFFSCREEN)
    {
      oam_id = oam_spr(Bombs[0].xpos, Bombs[0].ypos, 0xB2, 0x00, oam_id);
      if (iabs(Bombs[0].ypos - player_y+8) < 16 &&  iabs(Bombs[0].xpos - player_x+8) < 16)
      {
        //We picked up the bomb
        if(bombs != 9)
        	bombs ++;
        Bombs[0].xpos = xOFFSCREEN;
        Bombs[0].ypos = YOFFSCREEN;
      }
    }
    for (i=0; i<9; i++) {
    	mapPiece* map = &Map[i];
        if (map->ypos != YOFFSCREEN) {
          if(map->current)
          	oam_id = oam_spr(map->xpos, map->ypos, 0x01, 0x01, oam_id);
          else if(i == 1 && secretUnlocked && !haveLader)
            	oam_id = oam_spr(map->xpos, map->ypos, 0x01, 0x02, oam_id);
          else if(i == 6 && haveLader)
            	oam_id = oam_spr(map->xpos, map->ypos, 0x01, 0x02, oam_id);
          else
            	oam_id = oam_spr(map->xpos, map->ypos, 0x01, 0x00, oam_id);
    		}
  	}
    extraSprites(oam_id);
    
    // Do this to "hide" any remaining sprites
    vrambuf_flush();
    oam_hide_rest(oam_id);
      if(health == 0)
      {
        dead = true;
        fade_out();
        break;
      }
    }
  }
      //show title screen
      scroll(0,240);//title is aligned to the color attributes, so shift it a bit to the right
      ppu_off();
      vram_adr(NAMETABLE_A); // Zelda probably started at 0x28d0 (8 rows below stats area)
      vram_unrle(GameOver);
  
      vram_adr(NAMETABLE_C); // Zelda probably started at 0x28d0 (8 rows below stats area)
      vram_fill(0x01,900);

      
      pal_bright(4);
      ppu_on_bg();
      delay(20);//delay just to make it look better

      iy=240<<4;
      dy=-8<<4;
      frame_cnt=0;
      wait=160;
      bright=4;

      while(1)
      {
        ppu_wait_frame();

        scroll(0,iy>>4);

        if(pad_trigger(0)&PAD_START) break;

        iy+=dy;

        if(iy<0)
        {
          iy=0;
          dy=-dy>>1;
        }

        if(dy>(-8<<4)) dy-=2;

        if(wait)
        {
          --wait;
        }
        else
        {
          pal_col(2,(frame_cnt&32)?0x0f:0x20);//blinking press start text
          ++frame_cnt;
        }
      }

      scroll(0,0);//if start is pressed, show the title at whole
      for(i=0;i<16;++i)//and blink the text faster
      {
        pal_col(2,i&1?0x0f:0x20);
        delay(4);
      }
      pal_fade_to(4);
      runGame();
}


// main function, run after console reset
void main(void) 
{
  runGame();
}
