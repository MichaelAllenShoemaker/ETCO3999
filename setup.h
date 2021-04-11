/*{pal:"nes",layout:"nes"}*/
const char PALETTE[32] =
{
  0x0F, // screen color
  
  0x05, 0x17, 0x27, 0x00, // background palette 0
  0x16, 0x20, 0x0F, 0x00, // background palette 1
  0x00, 0x1A, 0x20, 0x00, // background palette 2
  0x00, 0x1A, 0x20, 0x00, // background palette 3
  
  0x0F, 0x20, 0x33, 0x00, // sprite palette 0
  0x00, 0x37, 0x25, 0x00, // sprite palette 1
  0x36, 0x21, 0x19, 0x00, // sprite palette 2
  0x1D, 0x37, 0x2B, // sprite palette 3
};

void setup()
{
  byte i = 0;
  // Setting Up Status Bar
  vram_adr(NTADR_A(0, 1));
  vram_unrle(TitleBar);
  put_str(NTADR_A(26,2), "LIFE");
  put_str(NTADR_A(17,2), "x1");
  
  //Filling in the maps
  vram_adr(NTADR_A(1, 5)); // Zelda probably started at 0x28d0 (8 rows below stats area)
  vram_unrle(Map11); // my map01 is an array of 274 unsigned char's
 
  
  // enable PPU rendering (turn on screen)
  ppu_on_all();
  pal_all(PALETTE); // generally before game loop (in main)
  
  vrambuf_clear();
  set_vram_update(updbuf); // updbuf = 0x100 -- start of stack RAM

      
  // set sprite 0
  oam_clear();
  oam_spr(1, 38, 0xa4, 0, 0);
  for (i=0; i<10; i++) {
    Bullets[i].ypos = YOFFSCREEN;
  }
}


