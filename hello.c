#include "neslib.h"
#include "vrambuf.h"
#include "bcd.h"
// link the pattern table into CHR ROM
//#link "chr_generic.s"
//#link "vrambuf.c"

/*{pal:"nes",layout:"nes"}*/
const char PALETTE[32] =
{
  0x03, // screen color
  
  0x0F, 0x30, 0x28, 0x00, // background palette 0
  0x16, 0x20, 0x2C, 0x00, // background palette 1
  0x00, 0x1A, 0x20, 0x00, // background palette 2
  0x00, 0x1A, 0x20, 0x00, // background palette 3
  
  0x07, 0x27, 0x1C, 0x00, // sprite palette 0
  0x00, 0x37, 0x25, 0x00, // sprite palette 1
  0x36, 0x21, 0x19, 0x00, // sprite palette 2
  0x1D, 0x37, 0x2B, // sprite palette 3
};

///// METASPRITES
#define TILE 0xd8
// define a 2x2 metasprite
unsigned char metasprite[]={
        0,      0,      TILE+0,   0x00, 
        0,      8,      TILE+1,   0x00, 
        8,      0,      TILE+2,   0x00, 
        8,      8,      TILE+3,   0x00, 
        128};

unsigned char flippedmetasprite[]={
        0,      0,      TILE+2,   0x40, 
        0,      8,      TILE+3,   0x40, 
        8,      0,      TILE+0,   0x40, 
        8,      8,      TILE+1,   0x40, 
        128};

#define ATTRDOOR 0x00
const unsigned char doorsprite[]={
        0,      0,      0xc4,   ATTRDOOR, 
        0,      8,      0xc5,   ATTRDOOR, 
        8,      0,      0xc6,   ATTRDOOR, 
        8,      8,      0xc7,   ATTRDOOR, 
        128};


// main function, run after console reset
void main(void) 
{
  // x position
  unsigned char sprite_x = 20;
  
  // sprite direction
  bool sprite_dir = false;
  
  // sprite atribute
  char atri = 0x00;
  
  // for a for loop
  int i;

  // write text to name table
  vram_adr(NTADR_A(2,2));		// set address
  vram_write("This is", 7);	// write bytes to video RAM
  
  // write text to name table
  vram_adr(NTADR_A(2,3));		// set address
  vram_write("Michael Shoemaker's", 19);	// write bytes to video RAM

  // write text to name table
  vram_adr(NTADR_A(2,4));		// set address
  vram_write("first NES 'Game'!", 17);	// write bytes to video RAM
  
  
  // write background table
  vram_adr(NTADR_A(1,21));		// set address
  for(i = 0; i < 30; i++)
  {
    vram_put(0xc1);
  }
  
  // enable PPU rendering (turn on screen)
  ppu_on_all();
  pal_all(PALETTE); // generally before game loop (in main)
  
  vrambuf_clear();
  set_vram_update(updbuf); // updbuf = 0x100 -- start of stack RAM

  
  // infinite loop
  while (1)
  {
    // do this at the start of each frame
    int oam_id = 0;
    
    //move the char back and forth
    if(sprite_dir)
    {
      sprite_x += 1;
      if(sprite_x > 235)
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
    
    // Do this when "drawing" of character sprite
    if(sprite_dir)
    	oam_id = oam_meta_spr(sprite_x, 150, oam_id, metasprite);
    else
      oam_id = oam_meta_spr(sprite_x, 150, oam_id, flippedmetasprite);
    oam_id = oam_meta_spr(232, 150, oam_id, doorsprite);
    
    //on the door test
    if(sprite_x >= 225)
    	vrambuf_put(NTADR_A(2, 5), "On the door!", 12);
    else
        vrambuf_put(NTADR_A(2, 5), "           ", 12);
    vrambuf_flush();
    // Do this to "hide" any remaining sprites
    oam_hide_rest(oam_id);
  }
}
