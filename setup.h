/*{pal:"nes",layout:"nes"}*/
const char PALETTE[32] =
{
  0x0F, // screen color
  
  0x05, 0x17, 0x27, 0x00, // background palette 0
  0x16, 0x20, 0x0F, 0x00, // background palette 1
  0x00, 0x1A, 0x20, 0x00, // background palette 2
  0x00, 0x1A, 0x20, 0x00, // background palette 3
  
  0x05, 0x17, 0x27, 0x00, // sprite palette 0
  0x16, 0x37, 0x16, 0x00, // sprite palette 1
  0x17, 0x21, 0x11, 0x00, // sprite palette 2
  0x0F, 0x20, 0x27, // sprite palette 3
};

void reset_bullets() {
  byte i;
  for (i=0; i<10; i++) { 
    Bullets[i].ypos = YOFFSCREEN;
    Bullets[i].xpos = xOFFSCREEN;
    Bullets[i].dx = 0;
    Bullets[i].dy = 0;
  }
}

void reset_enemies() {
  byte i;
  for (i=0; i<6; i++) { 
    Enemies[i].ypos = YOFFSCREEN;
    Enemies[i].xpos = xOFFSCREEN;
    Enemies[i].dx = 0;
    Enemies[i].dy = 0;
    Enemies[i].health = 0;
  }
}

void reset_map() {
  byte i;
  for (i=0; i<9; i++) { 
    Map[i].ypos = YOFFSCREEN;
    Map[i].xpos = xOFFSCREEN;
    Map[i].current = false;
  }
}

void setup()
{
    
  byte i = 0;
  
  reset_bullets();
  reset_enemies();
  reset_map();
  
  //reset all variables
  player_x = 120;
  player_y = 120;

  //int for direction 0 = right, 1 = down, 2 = left, 3 = up
  playerDirection = 0;
  playerSprite = 0;
  playerFramesToMove = 0;
  framesBetweenChange = 10;
  Bombs[0].xpos = xOFFSCREEN;
  Bombs[0].ypos = YOFFSCREEN;

  titleScreen = true;
  canGoRight = true;
  canGoUp = false;
  canGoLeft = false;
  canGoDown = false;
  canMove = true;
  dead = false;

  numberTillDrop = 3;
  map = 11;
  mapIndex = 0;
  startx = 50;
  starty = 28;
  secretUnlocked = false;
  haveLader = false;
  health = 8;
  bombs = 1;
  iframes = 0;
  vbright = 4;

  numEnemies = 0;
  
  //DONE
  
  ppu_off();
  // Setting Up Status Bar
  vram_adr(NTADR_A(0, 1));
  vram_unrle(TitleBar);
  
  //Filling in the maps
  vram_adr(NTADR_A(1, 5)); // Zelda probably started at 0x28d0 (8 rows below stats area)
  vram_unrle(Map11); // my map01 is an array of 274 unsigned char's
  Map[0].xpos = 50;
  Map[0].ypos = 28;
  Map[0].current = true;
  mapIndex = 0;
 
  
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

void title()
{
  titleScreen = true;
  ppu_off();
  // Setting Up Status Bar
  vram_adr(NTADR_A(0, 0));
  vram_unrle(TitleScreen);
  put_str(NTADR_A(4,4), "the legend of");
  put_str(NTADR_A(5,18), "a game by:");
  put_str(NTADR_A(7,20), "michael shoemaker");
  put_str(NTADR_A(5,25), "press start to play.");
  
  
  // enable PPU rendering (turn on screen)
  ppu_on_all();
  pal_all(PALETTE); // generally before game loop (in main)
  
  vrambuf_clear();
  set_vram_update(updbuf); // updbuf = 0x100 -- start of stack RAM

      
  // set sprite 0
  oam_clear();
}

