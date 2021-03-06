int map;
int mapIndex;
int startx;
int starty;
bool secretUnlocked;
bool haveLader;
int health;
int bombs;
int iframes;
int vbright;

int numEnemies;

typedef struct {
  unsigned char xpos;
  unsigned char ypos;
  unsigned char dx;
  unsigned char dy;
  int health;
} enemy;

enemy Enemies[6];

byte iabs(int x) {
  return x >= 0 ? x : -x;
}

void spawnEnemy(unsigned char x, unsigned char y, unsigned char dx, unsigned char dy)
{
  Enemies[numEnemies].xpos = x;
  Enemies[numEnemies].ypos = y;
  Enemies[numEnemies].dx = dx;
  Enemies[numEnemies].dy = dy;
  Enemies[numEnemies].health = 1;
  numEnemies++;
}


void pal_fade_to(unsigned to)
{
  while(bright!=to)
  {
    delay(4);
    if(bright<to) ++bright; else --bright;
    pal_bright(bright);
  }

  if(!bright)
  {
    ppu_off();
    set_vram_update(NULL);
    scroll(0,0);
  }
}

// Helper Functions
void put_str(unsigned int adr, const char *str) {
  vram_adr(adr);        // set PPU read/write address
  vram_write(str, strlen(str)); // write bytes to PPU
}

void fade_out() {
  byte vb;
  for (vb=4; vb>0; vb--) {
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

void bombScreen()
{
  byte vb;
  pal_bright(8);
  for (vb=0; vb<=20; vb++) {
    ppu_wait_frame();
  }
  pal_bright(4);
}


void change_Map(int dir)
{
  // Move 9 for one tile
  int direction = dir;
  canMove = false;
  //Update the old Minimap
  Map[mapIndex].current = false;
  
  sfx_play(SND_START,0);	// play starting sound
  
  //Get new map direction 0 = right, 1 = down, 2 = left, 3 = up
  if(dir == 0)
  {
    map++;
    player_x = 24;
    player_y = 118;
  }
  else if(dir == 1)
  {
    map -= 5;
    player_x = 120;
    player_y = 188;
  }
  else if(dir == 2)
  {
    map--;
    player_x = 218;
    player_y = 118;
  }
  else
  {
    map += 5;
    player_x = 120;
    player_y = 55;
  }
  ppu_off();
  //Load the correct map
  vram_adr(NTADR_A(1, 5)); // Zelda probably started at 0x28d0 (8 rows below stats area)
  if(dir == 10)
  {
      vram_adr(NAMETABLE_A);
      vram_unrle(GameOver);

      vram_adr(NAMETABLE_C);//clear second nametable, as it is visible in the jumping effect
      vram_fill(0,1024);
  }
  else if(map == 11)
  {
      	vram_unrle(Map11);
      	Map[0].current = true;
    	mapIndex = 0;
        canGoRight = true;
        canGoUp = false;
        canGoLeft = false;
        canGoDown = false;
  }
  else if(map == 12)
  {
        spawnEnemy(125, 125, 1, 0);
    	if(secretUnlocked)
        {
          vram_unrle(Map12Unlocked);
          canGoRight = true;
          canGoUp = true;
          canGoLeft = true;
          canGoDown = false;
        }
    	else
        {
          vram_unrle(Map12);
          canGoRight = false;
          canGoUp = true;
          canGoLeft = true;
          canGoDown = false;
        }
    	Map[1].xpos = startx + 9;
    	Map[1].ypos = starty;
      	Map[1].current = true;
    	mapIndex = 1;
    
  }
  else if(map == 13)
  {
      	vram_unrle(Map13);
    	Map[2].xpos = startx + 18;
    	Map[2].ypos = starty;
      	Map[2].current = true;
    	mapIndex = 2;
    
        canGoRight = false;
        canGoUp = false;
        canGoLeft = true;
        canGoDown = false;
  }
  else if(map == 7)
  {
    	spawnEnemy(90, 90, 1, 2);
        spawnEnemy(150, 90, -1, 2);
      	vram_unrle(Map7);
    	Map[3].xpos = startx + 9;
    	Map[3].ypos = starty - 9;
      	Map[3].current = true;
    	mapIndex = 3;
    
        canGoRight = false;
        canGoUp = true;
        canGoLeft = false;
        canGoDown = true;
  }
  else if(map == 2)
  {
        spawnEnemy(45, 130, -2, 1);
        spawnEnemy(170, 64, 1, 1);
        spawnEnemy(60, 64, 1, -1);
      	vram_unrle(Map2);
        Map[4].xpos = startx + 9;
    	Map[4].ypos = starty - 18;
      	Map[4].current = true;
    	mapIndex = 4;
    
        canGoRight = true;
        canGoUp = false;
        canGoLeft = false;
        canGoDown = true;
  }
  else if(map == 3)
  {
        spawnEnemy(65, 64, 0, 1);
        spawnEnemy(125, 64, 0, 1);
        spawnEnemy(185, 64, 0, 1);
        spawnEnemy(85, 180, 0, -1);
        spawnEnemy(145, 180, 0, -1);
        spawnEnemy(205, 180, 0, -1);
      	vram_unrle(Map3);
    	Map[5].xpos = startx + 18;
    	Map[5].ypos = starty - 18;
      	Map[5].current = true;
    	mapIndex = 5;

        canGoRight = true;
        canGoUp = false;
        canGoLeft = true;
        canGoDown = false;
  }
  else if(map == 4)
  {
      	vram_unrle(Map4);
    	Map[6].xpos = startx + 27;
    	Map[6].ypos = starty - 18;
      	Map[6].current = true;
    	mapIndex = 6;

        canGoRight = true;
        canGoUp = false;
        canGoLeft = true;
        canGoDown = true;
  }
  else if(map == 5)
  {
        music_play(1);		// start the music
      	vram_unrle(Map5);
    	Map[7].xpos = startx + 36;
    	Map[7].ypos = starty - 18;
      	Map[7].current = true;
    	mapIndex = 7;
    
        canGoRight = false;
        canGoUp = false;
        canGoLeft = false;
        canGoDown = false;
  }
  else
  {
        spawnEnemy(45, 75, 1, 1);
        spawnEnemy(205, 75, -1, 1);
      	vram_unrle(Map9);
    	Map[8].xpos = startx + 27;
    	Map[8].ypos = starty - 9;
      	Map[8].current = true;
    	mapIndex = 8;
    
        canGoRight = false;
        canGoUp = true;
        canGoLeft = false;
        canGoDown = false;
  }
  ppu_on_all();
  canMove = true;
}


int extraSprites(int oamId)
{
  if(map==9 && secretUnlocked)
  {
    int x = 120;
    int y = 119;
    oamId = oam_spr(x, y, 0xA7, 0x02, oamId);
    oamId = oam_spr(x+8, y, 0xA8, 0x02, oamId);
    oamId = oam_spr(x+8, y+8, 0xA6, 0x02, oamId);
    oamId = oam_spr(x, y+8, 0xA5, 0x02, oamId);
  }
  if(map==13 && haveLader)
  {
    int x = 120;
    int y = 111;
    oamId = oam_spr(x, y, 0x01, 0x00, oamId);
    oamId = oam_spr(x+8, y, 0x01, 0x00, oamId);
    oamId = oam_spr(x+8, y+8, 0x01, 0x00, oamId);
    oamId = oam_spr(x, y+8, 0x01, 0x00, oamId);
  }
  if(map==4 && haveLader)
  {
    int x = 152;
    int y = 126;
    oamId = oam_spr(x, y, 0xD6, 0x00, oamId);
    oamId = oam_spr(x+8, y, 0xD6, 0x00, oamId);
    oamId = oam_spr(x+8, y+8, 0xD7, 0x00, oamId);
    oamId = oam_spr(x, y+8, 0xD7, 0x00, oamId);
  }
  //Heart1
  if(health > 1)
  	oamId = oam_spr(208, 23, 0xB5, 0x00, oamId);
  else if (health == 1)
    	oamId = oam_spr(208, 23, 0xB3, 0x00, oamId);
  else
    	oamId = oam_spr(208, 23, 0xB4, 0x00, oamId);
  
  //Heart2
  if(health > 3)
  	oamId = oam_spr(216, 23, 0xB5, 0x00, oamId);
  else if (health == 3)
    	oamId = oam_spr(216, 23, 0xB3, 0x00, oamId);
  else
    	oamId = oam_spr(216, 23, 0xB4, 0x00, oamId);
  
  
  //Heart3
  if(health > 5)
  	oamId = oam_spr(224, 23, 0xB5, 0x00, oamId);
  else if (health == 5)
    	oamId = oam_spr(224, 23, 0xB3, 0x00, oamId);
  else
    	oamId = oam_spr(224, 23, 0xB4, 0x00, oamId);
  
  //Heart4
  if(health > 7)
  	oamId = oam_spr(232, 23, 0xB5, 0x00, oamId);
  else if (health == 7)
    	oamId = oam_spr(232, 23, 0xB3, 0x00, oamId);
  else
    	oamId = oam_spr(232, 23, 0xB4, 0x00, oamId);
  
  if(bombs == 0)
    	oamId = oam_spr(144, 15, 0x30, 0x00, oamId);
  if(bombs == 1)
    	oamId = oam_spr(144, 15, 0x31, 0x00, oamId);
  if(bombs == 2)
    	oamId = oam_spr(144, 15, 0x32, 0x00, oamId);
  if(bombs == 3)
    	oamId = oam_spr(144, 15, 0x33, 0x00, oamId);
  if(bombs == 4)
    	oamId = oam_spr(144, 15, 0x34, 0x00, oamId);
  if(bombs == 5)
    	oamId = oam_spr(144, 15, 0x35, 0x00, oamId);
  if(bombs == 6)
    	oamId = oam_spr(144, 15, 0x36, 0x00, oamId);
  if(bombs == 7)
    	oamId = oam_spr(144, 15, 0x37, 0x00, oamId);
  if(bombs == 8)
    	oamId = oam_spr(144, 15, 0x38, 0x00, oamId);
  if(bombs == 9)
    	oamId = oam_spr(144, 15, 0x39, 0x00, oamId);
  
  if(!canChangeMap)
  {
    if(canGoLeft)
    {
      if(map == 12)
      {
        int topx = 8;
        int topy = 119;
        oamId = oam_spr(topx, topy, 0xC1, 0x02, oamId);
        oamId = oam_spr(topx+8, topy, 0xC1, 0x02, oamId);
        oamId = oam_spr(topx+8, topy+8, 0xC1, 0x02, oamId);
        oamId = oam_spr(topx, topy+8, 0xC1, 0x02, oamId);
      }
      else
      {
        int topx = 8;
        int topy = 127;
        oamId = oam_spr(topx, topy, 0xC1, 0x02, oamId);
        oamId = oam_spr(topx+8, topy, 0xC1, 0x02, oamId);
        oamId = oam_spr(topx+8, topy+8, 0xC1, 0x02, oamId);
        oamId = oam_spr(topx, topy+8, 0xC1, 0x02, oamId);
      }
    }
    if(canGoRight)
    {
      if(map == 12)
      {
        int topx = 231;
        int topy = 119;
        oamId = oam_spr(topx, topy, 0xC1, 0x02, oamId);
        oamId = oam_spr(topx+8, topy, 0xC1, 0x02, oamId);
        oamId = oam_spr(topx+8, topy+8, 0xC1, 0x02, oamId);
        oamId = oam_spr(topx, topy+8, 0xC1, 0x02, oamId);
      }
      else
      {
      int topx = 231;
      int topy = 127;
      oamId = oam_spr(topx, topy, 0xC1, 0x02, oamId);
      oamId = oam_spr(topx+8, topy, 0xC1, 0x02, oamId);
      oamId = oam_spr(topx+8, topy+8, 0xC1, 0x02, oamId);
      oamId = oam_spr(topx, topy+8, 0xC1, 0x02, oamId);
      }
    }
    if(canGoUp)
    {
      int topx = 119;
      int topy = 39;
      oamId = oam_spr(topx, topy, 0xC1, 0x02, oamId);
      oamId = oam_spr(topx+8, topy, 0xC1, 0x02, oamId);
      oamId = oam_spr(topx+8, topy+8, 0xC1, 0x02, oamId);
      oamId = oam_spr(topx, topy+8, 0xC1, 0x02, oamId);
    }
    if(canGoDown)
    {
      int topx = 119;
      int topy = 207;
      oamId = oam_spr(topx, topy, 0xC1, 0x02, oamId);
      oamId = oam_spr(topx+8, topy, 0xC1, 0x02, oamId);
      oamId = oam_spr(topx+8, topy+8, 0xC1, 0x02, oamId);
      oamId = oam_spr(topx, topy+8, 0xC1, 0x02, oamId);
    }
  }
  
  return oamId;
}

void enemyCollision(unsigned char enemyx, unsigned char enemyy)
{
  if (iframes <= 0 && iabs(player_y+8 - enemyy) < 16 &&  iabs(player_x+8 - enemyx) < 16)
  {
   health--;
   sfx_play(SND_BOOP,0);	// play starting sound
   iframes = 30;
   vbright = 8;
  }
}