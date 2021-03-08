
/*
A simple "hello world" example.
Set the screen background color and palette colors.
Then write a message to the nametable.
Finally, turn on the PPU to display video.
*/

#include "neslib.h"
#include "vrambuf.h"
#include "bcd.h"


// discuss why making this const and a global is a good thing
/*{pal:"nes",layout:"nes"}*/
const char PALETTE[32] =
{
  0x03, // screen color
  
  0x24, 0x16, 0x20, 0x0, // background palette 0
  0x1c, 0x20, 0x2c, 0x0, // background palette 1
  0x00, 0x1a, 0x20, 0x0, // background palette 2
  0x00, 0x1a, 0x20, 0x0, // background palette 3
  
  0x23, 0x31, 0x41, 0x0, // sprite palette 0
  0x00, 0x37, 0x25, 0x0, // sprite palette 1
  0x36, 0x21, 0x19, 0x0, // sprite palette 2
  0x1d, 0x37, 0x2b, // sprite palette 3
};

///// METASPRITES

#define TILE 0xd8
#define ATTR 0x00
// define a 2x2 metasprite
unsigned char metasprite[]={
        0,      0,      TILE+0,   ATTR, 
        0,      8,      TILE+1,   ATTR, 
        8,      0,      TILE+2,   ATTR, 
        8,      8,      TILE+3,   ATTR, 
        128};

#define TILEDOOR 0xd8
#define ATTRDOOR 0x00
unsigned char doorsprite[]={
        0,      0,      TILE+0,   ATTR, 
        0,      8,      TILE+1,   ATTR, 
        8,      0,      TILE+2,   ATTR, 
        8,      8,      TILE+3,   ATTR, 
        128};

// link the pattern table into CHR ROM
//#link "chr_generic.s"
//#link "vrambuf.c"

// main function, run after console reset
void main(void) 
{
  // x position
  unsigned char sprite_x = 20;
  
  // sprite direction
  bool sprite_dir = false;
  
  // sprite atribute
  char atri = 0x00;
  
  int i;
 
  
  pal_all(PALETTE); // generally before game loop (in main)

  // write text to name table
  vram_adr(NTADR_A(2,2));		// set address
  vram_write("This is", 7);	// write bytes to video RAM
  
  // write text to name table
  vram_adr(NTADR_A(2,3));		// set address
  vram_write("Michael Shoemaker's", 19);	// write bytes to video RAM

  // write text to name table
  vram_adr(NTADR_A(2,4));		// set address
  vram_write("first NES 'Game'!", 17);	// write bytes to video RAM
  
   // write door table
  vram_adr(NTADR_A(1,20));		// set address
  vram_put(0xd8 + 0x01);

  
   // write background table
  vram_adr(NTADR_A(1,21));		// set address
  for(i = 0; i < 30; i++)
  {
    vram_put(0x0c);
  }
  
  // enable PPU rendering (turn on screen)
  ppu_on_all();

  // infinite loop
  while (1)
  {
    // do this at the start of each frame
    int oam_id = 0;
    
    //move the char back and forth
    if(sprite_dir)
    {
      sprite_x += 1;
      if(sprite_x > 225)
      {
        sprite_dir = false;
        
      }
    }
    else
    {
      sprite_x -= 1;
      if(sprite_x < 20)
      {
        sprite_dir = true;
      }
    }
    
    ppu_wait_frame();
    // Do this when "drawing" of character sprite
    oam_id = oam_meta_spr(sprite_x, 150, oam_id, metasprite);
    
    // Do this to "hide" any remaining sprites
    oam_hide_rest(oam_id);
  }
}
