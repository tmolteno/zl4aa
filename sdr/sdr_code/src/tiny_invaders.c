/*  Space Invaders U8g2 - v1.0 
 *    Uses U8g2 library
 *    Modified by Ricardo Moreno
 *    April 03, 2018
 * 
 *  Original Adafruit_SSD1306.h version by
 *    XTronical
 *    http://www.xtronical.com 
 *    Modified ToneAC library
 *    http://www.xtronical.com/projects/space-invaders/parts-8-15/part-11-adding-sound/
 *    
 *  U8g2lib.h
 *  https://github.com/olikraus/U8g2_Arduino
 *  
 *  History:
 *  04/03/2018 v1.0 - Initial release.
 */

/* **********************************************************
/*                      Libraries                           *
/* ******************************************************** */
#include <u8g2.h>  /* Monochrome graphics Library */ 
#include <stdbool.h>
#include <debug.h>
#include <ch32v30x.h>
#include <ch32v30x_rng.h>
#include "hardware.h"

//#include <toneAC2.h>
 
/* **********************************************************
/*                      Global Constants                    *
/* ******************************************************** */
  /* When and why use #define or const
  
    Type safety:  const variables are usually preferable (where possible) becaus they get defined a data type
                  #define (preprocessor macro) directly copies the literal value into each location in code, making every usage independent or a varient.
                  This can hypothetically result in ambiguities, because the type may end up being resolved differently depending on how/where it's used.
                  A const variable is only ever one type, which is determined by its declaration, and resolved during initialisation. 
                  A const require an explicit cast before it will behave differently (can be safely implicitly type-promoted).
                  The compiler can emit a more reliable warning when a type issue occurs.
                  A possible workaround is to include an explicit cast or a type-suffix within a #define. For example:
                     #define THE_ANSWER (int8_t)42
                     #define NOT_QUITE_PI 3.14f
                  This approach can potentially cause syntax problems in some cases
  
    Memory usage:  Using a const variable vs. a #define can affect where the data is stored in memory
                   const variables will (usually) be stored in SRAM, along with all other variables.
                   Literal values used in #define will often be stored in program space (Flash memory), alongside the sketch itself.
                   (Note that there are various things which can affect exactly where something is stored)
                   SRAM and Flash have different limitations (e.g. 2 KB and 32 KB respectively for the Uno).
                   For some applications, it's quite easy to run out of SRAM, so it can be helpful to shift some things into Flash.
                   The reverse is also possible, although probably less common.
  
    PROGMEM:  It's possible to get the benefits of type-safety while also storing the data in program space (Flash). This is done using the PROGMEM keyword.
              It doesn't work for all types, but it's commonly used for arrays of integers or strings.
              https://www.arduino.cc/reference/en/language/variables/utilities/progmem/
  */

#define PROGMEM

extern u8g2_t u8g2; // a structure which will contain all the data for one display

/* **************** DISPLAY SETTINGS **************** */

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
 
/* **************** INPUT SETTINGS **************** */
//#define FIRE_BUT 6
//#define RIGHT_BUT 5
//#define LEFT_BUT 4

/* **************** Game Constants **************** */
// Mothership
#define MOTHERSHIP_HEIGHT 4
#define MOTHERSHIP_WIDTH 16
#define MOTHERSHIP_SPEED 2
#define MOTHERSHIP_SPAWN_CHANCE 250         //HIGHER IS LESS CHANCE OF SPAWN
#define DISPLAY_MOTHERSHIP_BONUS_TIME 20    // how long bonus stays on screen for displaying mothership

// Alien Settings
#define ALIEN_HEIGHT 8  
#define NUM_ALIEN_COLUMNS 7
#define NUM_ALIEN_ROWS 3
#define X_START_OFFSET 6
#define SPACE_BETWEEN_ALIEN_COLUMNS 5
#define LARGEST_ALIEN_WIDTH 11
#define SPACE_BETWEEN_ROWS 9
#define INVADERS_DROP_BY 4                  // pixel amount that invaders move down by
#define INVADERS_SPEED 24                   // speed of movement, lower=faster.
#define INVADER_HEIGHT 8
#define EXPLOSION_GFX_TIME 7                // How long an ExplosionGfx remains on screen before dissapearing
#define INVADERS_Y_START MOTHERSHIP_HEIGHT-1
#define AMOUNT_TO_DROP_BY_PER_LEVEL 4       //NEW How much farther down aliens start per new level
#define LEVEL_TO_RESET_TO_START_HEIGHT 4    // EVERY MULTIPLE OF THIS LEVEL THE ALIEN y START POSITION WILL RESET TO TOP
#define ALIEN_X_MOVE_AMOUNT 1               //number of pixels moved at start of wave
#define CHANCEOFBOMBDROPPING 80             // Higher the number the rarer the bomb drop, 
#define BOMB_HEIGHT 4
#define BOMB_WIDTH 2
#define MAXBOMBS 3                          // Max number of bombs allowed to drop at a time
// These two lines are for when bombs collide with the bases
#define CHANCE_OF_BOMB_DAMAGE_TO_LEFT_OR_RIGHT 20    // higher more chance
#define CHANCE_OF_BOMB_PENETRATING_DOWN 1           // higher more chance

// Player settings
#define TANKGFX_WIDTH 13
#define TANKGFX_HEIGHT 8
#define PLAYER_X_MOVE_AMOUNT 2
#define LIVES 3                             // NEW
#define PLAYER_EXPLOSION_TIME 10            // How long an ExplosionGfx remains on screen before dissapearing
#define PLAYER_Y_START 56
#define PLAYER_X_START 0
#define BASE_WIDTH 16               
#define BASE_WIDTH_IN_u8S 2
#define BASE_HEIGHT 8
#define BASE_Y 46
#define NUM_BASES 3
#define BASE_MARGINS 10   

#define MISSILE_HEIGHT 4
#define MISSILE_WIDTH 1
#define MISSILE_SPEED 4

// Status of a game object constants
#define ACTIVE 0
#define EXPLODING 1
#define DESTROYED 2

// background dah dah dah sound setting
#define NOTELENGTH 1                        // larger means play note longer

/* **********************************************************
/*                      Global Constants                    *
/*                      Graphics - Aliens                   *
/* ******************************************************** */
// I wasn't able to get the binary bitmaps to work properly, so I converted all the graphics to hex
// I also combined the two alien versions into a single array
const uint8_t MotherShipGfx[][8] PROGMEM = {
{ 0xfc, 0x3f, 0xb6, 0x6d, 0xff, 0xff, 0x9c, 0x39},
{ 0xfc, 0x00, 0x4a, 0x01, 0xff, 0x03, 0xb5, 0x02}
};

const uint8_t InvaderTopGfx[][8] PROGMEM = {
{ 0x18, 0x3c, 0x7e, 0xdb, 0xff, 0x24, 0x5a, 0xa5},
{ 0x18, 0x3c, 0x7e, 0xdb, 0xff, 0x5a, 0x81, 0x42}
};

const uint8_t InvaderMiddleGfx[][16] PROGMEM = {
{ 0x04, 0x01, 0x88, 0x00, 0xfc, 0x01, 0x76, 0x03, 0xff, 0x07, 0xfd, 0x05, 0x05, 0x05, 0xd8, 0x00},
{ 0x04, 0x01, 0x88, 0x00, 0xfd, 0x05, 0x75, 0x05, 0xff, 0x07, 0xfc, 0x01, 0x04, 0x01, 0x02, 0x02}
};
 
const uint8_t InvaderBottomGfx[][16] PROGMEM = {
{ 0xf0, 0x00, 0xfe, 0x07, 0xff, 0x0f, 0x67, 0x0e, 0xff, 0x0f, 0x9c, 0x03, 0x06, 0x06, 0x0c, 0x03},
{ 0xf0, 0x00, 0xfe, 0x07, 0xff, 0x0f, 0x67, 0x0e, 0xff, 0x0f, 0x9c, 0x03, 0x62, 0x04, 0x01, 0x08}
};

/* **********************************************************
/*                      Global Constants                    *
/*                      Graphics - Player                   *
/* ******************************************************** */
const uint8_t TankGfx[] PROGMEM = {
  0x40, 0x00, 0xe0, 0x00, 0xe0, 0x00, 0xfe, 0x0f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f
};

const uint8_t MissileGfx[] PROGMEM = {
   0x01, 0x01, 0x01, 0x01   
};

const uint8_t AlienBombGfx[] PROGMEM = {
   0x01, 0x02, 0x01, 0x02
};

const uint8_t BaseGfx[] PROGMEM = {
  0xf8, 0x1f, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xf8, 0x07, 0xe0, 0x07, 0xe0
};

const uint8_t ExplosionGfx[] PROGMEM = {
  0x10, 0x01, 0xa2, 0x08, 0x04, 0x04, 0x08, 0x02, 0x03, 0x18, 0x08, 0x02, 0xa4, 0x04, 0x12, 0x09 
};

/* **********************************************************
/*                      Global Classes                      *
/*                     Game structures                      *
/* ******************************************************** */ 
typedef struct GameObjectStruct  {
  // base object which most other objects will include
    signed int X;
    signed int Y;  
    uint8_t Status;              //0 active, 1 exploding, 2 destroyed
} GameObjectStruct;

typedef struct BaseStruct {
    GameObjectStruct Ord;
    uint8_t Gfx[16];
} BaseStruct;

typedef struct AlienStruct  {
  GameObjectStruct Ord;
  uint8_t ExplosionGfxCounter;   // how long we want the ExplosionGfx to last
} AlienStruct;
 
typedef struct PlayerStruct  {
    GameObjectStruct Ord;
    unsigned int Score;
    uint8_t Lives;
    uint8_t Level;
    uint8_t AliensDestroyed;     // count of how many killed so far
    uint8_t AlienSpeed;          // higher the number slower they go, calculated when ever alien destroyed
    uint8_t ExplosionGfxCounter; // how long we want the ExplosionGfx to last
} PlayerStruct;

/* **********************************************************
/*                    Global Variables                      *
/* ******************************************************** */ 
//                    Aliens & screen
//          The array of aliens across the screen
AlienStruct Alien[NUM_ALIEN_COLUMNS][NUM_ALIEN_ROWS];
AlienStruct MotherShip;
GameObjectStruct AlienBomb[MAXBOMBS];
BaseStruct Base[NUM_BASES];

static const uint8_t TOTAL_ALIENS PROGMEM = NUM_ALIEN_COLUMNS*NUM_ALIEN_ROWS;  //NEW
 
// widths of aliens
// as aliens are the same type per row we do not need to store their graphic width per alien in the structure above
// that would take a uint8_t per alien rather than just three entries here, 1 per row, saving significant memory
uint8_t AlienWidth[]={8,11,12};                // top, middle ,bottom widths
 
char AlienXMoveAmount=2;  
signed char InvadersMoveCounter;            // counts down, when 0 move invaders, set according to how many aliens on screen
bool AnimationFrame=false;                  // two frames of animation, if true show one if false show the other

// Mothership
signed char MotherShipSpeed;
unsigned int MotherShipBonus;
signed int MotherShipBonusXPos;             // pos to display bonus at
uint8_t MotherShipBonusCounter;                // how long bonus amount left on screen
uint8_t MotherShipType;                        // which mothership to display

// Player global variables
PlayerStruct Player;
GameObjectStruct Missile;
 
// game variables
bool GameInPlay=false;
uint8_t FONT_Ascent;
uint8_t FONT_Descent;
/* **********************************************************
/*                    Global Variables                      *
/*                     Sound settings                       *
/* ******************************************************** */
// music (dah dah dah sound) control, these are the "pitch" of the four basic notes
const uint8_t Music[] PROGMEM = {
  160,100,80,62
};

uint8_t MusicIndex;                            // index pointer to next note to play
uint8_t MusicCounter;                          // how long note plays for countdown timer, is set to
                                            // NOTELENGTH define above

bool PlayerExplosionNoiseCompleted = false; // flag to indicate when noise has completed
bool ShootCompleted = true;                 // stops music when this is false, so we can here shoot sound




void InitAliens(int YStart);
bool Collision(GameObjectStruct Obj1,uint8_t Width1,uint8_t Height1,GameObjectStruct Obj2,uint8_t Width2,uint8_t Height2);
void InitPlayer(void);
void InitBases(void);

void Physics(void);
uint8_t GetScoreForAlien(int RowNumber);
void MotherShipPhysics(void);
void AlienControl(void);
void PlayerControl(void);
void MissileControl(void);
void CheckCollisions(void);
void UpdateDisplay(void);
void NewGame(void);
void ResetGame(void);
void CenterText(const char *Text,uint8_t RowValue);
void GameOver(void);
void ResetAliens(void);
void ResetMotherShip(void);
void ResetPlayer(void);
void ResetMissiles(void);
void ResetMissile(uint8_t MissileNumber);
void ResetAlien(uint8_t AlienNumber);
void ResetAlienMissile(uint8_t MissileNumber);
void ResetMotherShipMissile(void);
void ResetMotherShip(void);

void PlayerHit(void);
void DropBomb(void);
void MoveBombs(void);

void AttractScreen(void);
void LoseLife(void);
void NextLevel(PlayerStruct *Player);
void PlayRewardMusic(void);

void MissileAndAlienCollisions(void);

void DrawAlien(GameObjectStruct *Alien);
void LoseLife(void);
void DisplayPlayerAndLives(PlayerStruct *Player);

void DestroyBase(GameObjectStruct *Alien,BaseStruct *Base,char Mask,int Baseu8Offset);
void AlienAndBaseCollisions(void);
void BombAndBasesCollision(GameObjectStruct *Bomb);
void MissileAndBasesCollision(void);
void MotherShipCollisions(void);

int RightMostPos(void);
int LeftMostPos(void);

/**
 * Compatibility functions
*/
unsigned int HiScore;

int getHighScore(void) {
  // Read HighScore from EPROM and return it
  return HiScore;
}

int setHighScore(int Hs) {
  // Write HighScore to EPROM
  HiScore = Hs;
  return HiScore;
}

bool fireButtonPressed(void) {
  // Return true if fire button is pressed
  return PTT_Pressed();
}

bool fire2ButtonPressed(void) {
  // Return true if fire button is pressed
  return false; // !digitalRead(FIRE2_BUT);
}

bool leftButtonPressed(void) {
  // Return true if left button is pressed
  return false; // !digitalRead(X_pin);
}

bool rightButtonPressed(void) {
  // Return true if right button is pressed
  return false; // !digitalRead(Y_pin);
}

bool downButtonPressed(void) {
  // Return true if down button is pressed
  return false; // !digitalRead(Y_pin);
}

int getDigits(uint16_t num) {
  if (num<10){
    return 1;
  } else if (num <100) {
    return 2;
  } else if (num <1000) {
    return 3;
  } else if (num <10000) {
    return 4;
  } else {
    return 5;
  }
}
void printNum(int num, int ColPosition, int RowHeight) { 
  char str[8];
  sprintf(str, "%d", num);
  u8g2_DrawStr(&u8g2, ColPosition, RowHeight, str);
}

int random(int max) {
  while(RNG_GetFlagStatus(RNG_FLAG_DRDY)==RESET) {
  }
  int rand=RNG_GetRandomNumber();
  return rand % (max);
}

void noiseAC(int ms, int amp, bool noise_completed) {
  // Play noise for ms milliseconds
  // amp is the amplitude of the noise
  // snd is the sound to play 
}

void toneAC(int freq, int amp, int duration, bool tone_completed) {
  // Play noise for ms milliseconds
  // amp is the amplitude of the noise
  // snd is the sound to play 
}

void noToneAC(void) {

}


/* **********************************************************
/*                       Void Setup                         *
/* ******************************************************** */
void tiny_invaders_setup(){
  //OLED Diplay
  /* U8g2 Project: SSD1306 or SH1106 OLED SPI Board */
  u8g2_ClearDisplay(&u8g2);
  u8g2_SetBitmapMode(&u8g2,1);
  //display.begin(SSD1306_SWITCHCAPVCC,OLED_ADDRESS);
  InitAliens(0); 
  InitPlayer();

  //pinMode(RIGHT_BUT, INPUT_PULLUP);
  //pinMode(LEFT_BUT, INPUT_PULLUP);
  //pinMode(FIRE_BUT, INPUT_PULLUP);

  u8g2_SetFont(&u8g2,u8g2_font_t0_11b_tf);       //font size is ok - it is loaded in PROGMEM 
  FONT_Ascent = u8g2_GetAscent(&u8g2);          //getAscent returns the number of pixels above the baseline
  FONT_Descent = -u8g2_GetDescent(&u8g2);       //getDescent returns a negative value, a number of pixels below the baseline 
  u8g2_SetDrawColor(&u8g2,1);                    //set the color
  //display.setTextSize(1);
  //display.setTextColor(WHITE);
  
  getHighScore();
}

/* **********************************************************
/*                      Void Loop                           *
/* ******************************************************** */ 
void tiny_invaders_loop(){  
  //NEW
  if(GameInPlay)
  {
    Physics();
    UpdateDisplay();
  }
  else  
    AttractScreen();
}

/* **********************************************************
/*                 Functions and Subroutines                *
/* ******************************************************** */
 
void AttractScreen(void){
  uint8_t RowHeight;
  uint8_t NumWidth = u8g2_GetStrWidth(&u8g2,"8");
  uint8_t ColPosition = 0;

  //Determine number of digits in HiScore and set ColPosition
  if (HiScore<10){
    ColPosition = (int)((SCREEN_WIDTH - u8g2_GetStrWidth(&u8g2,"Hi Score ") - NumWidth)/2.0);
  } else if (HiScore <100) {
    ColPosition = (int)((SCREEN_WIDTH - u8g2_GetStrWidth(&u8g2,"Hi Score ") - NumWidth*2)/2.0);
  } else if (HiScore <1000) {
    ColPosition = (int)((SCREEN_WIDTH - u8g2_GetStrWidth(&u8g2,"Hi Score ") - NumWidth*3)/2.0);
  } else if (HiScore <10000) {
    ColPosition = (int)((SCREEN_WIDTH - u8g2_GetStrWidth(&u8g2,"Hi Score ") - NumWidth*4)/2.0);
 } else {
    ColPosition = (int)((SCREEN_WIDTH - u8g2_GetStrWidth(&u8g2,"Hi Score ") - NumWidth*5)/2.0);
   //65,535 max
  }

  u8g2_ClearBuffer(&u8g2);
  RowHeight = FONT_Ascent+(SCREEN_HEIGHT - 4*(FONT_Ascent+FONT_Descent+1))/2;
  CenterText("Play",RowHeight);
  //--> Next lines are for debugging <--
  //u8g2_print(&u8g2," ");u8g2_print(&u8g2,FONT_Ascent); u8g2_print(&u8g2," ");u8g2_print(&u8g2,FONT_Descent);
  //print(&u8g2," ");u8g2_print(&u8g2,ColPosition);
  RowHeight = RowHeight+FONT_Ascent+FONT_Descent+1;
  CenterText("Space Invaders",RowHeight); 
  RowHeight = RowHeight+FONT_Ascent+FONT_Descent+1;
  CenterText("Press Fire to start",RowHeight); 
  RowHeight = RowHeight+FONT_Ascent+FONT_Descent+1;
  // Special Center Text ---->

  char str[13];
  sprintf(str, "Hi Score %d", HiScore);
  u8g2_DrawStr(&u8g2, ColPosition, RowHeight, str);

  u8g2_SendBuffer(&u8g2);

  if((fireButtonPressed())|(fire2ButtonPressed())){
    GameInPlay=true;
    NewGame();
  }

  // CHECK FOR HIGH SCORE RESET, Player Must Hold FIRE_BUT, FIRE_BUT2, and Pull Down on the Joystick at the Same Time 
  if((fireButtonPressed())&(fire2ButtonPressed())&(downButtonPressed())) {
    setHighScore(0);
  }
}



void Physics(void){
  if(Player.Ord.Status==ACTIVE) {
    AlienControl(); 
    MotherShipPhysics();
    PlayerControl(); 
    MissileControl();
    CheckCollisions();
  }
}

uint8_t GetScoreForAlien(int RowNumber){
   // returns value for killing and alien at the row indicated
   switch (RowNumber)
   {
     case 0:return 30;
     case 1:return 20;
     case 2:return 10;
   }
   return 0;
}

void MotherShipPhysics(void){
  if(MotherShip.Ord.Status==ACTIVE)  {      // spawned, move it
    //toneAC2( pin1, pin2, frequency [, length [, background ]] ) 
    //toneAC2(spkr_pos,spkr_neg,(MotherShip.Ord.X % 8)*500,200,true);
    if (MotherShipType) {
      //Smaller Mothership
      toneAC((MotherShip.Ord.X % 8)*800,6,200,true);
    } else {
      //Larger Mothership
      toneAC((MotherShip.Ord.X % 8)*500,6,200,true);
    }
    MotherShip.Ord.X+=MotherShipSpeed;
    if(MotherShipSpeed>0)                   // going left to right , check if off right hand side
    {
      if(MotherShip.Ord.X>=SCREEN_WIDTH)  
      {
        MotherShip.Ord.Status=DESTROYED;
        noToneAC();
      }
    }
    else                                    // going right to left , check if off left hand side
    {
      if(MotherShip.Ord.X+MOTHERSHIP_WIDTH<0)  
      {
        MotherShip.Ord.Status=DESTROYED;
        noToneAC();
      }
    }
  }
  else  {
    // try to spawn mothership
    if(random(MOTHERSHIP_SPAWN_CHANCE)==1)
    {
      // Spawn a mother ship, starts just off screen at top
      MotherShip.Ord.Status=ACTIVE;
      //randomSeed(analogRead(A1));
      MotherShipType = random(2);           // a value of 0 or 1

      // need to set direction
      if(random(2)==1)                      // values between 0 and 1
      {
        MotherShip.Ord.X=SCREEN_WIDTH;
        MotherShipSpeed=-MOTHERSHIP_SPEED;  // if we go in here swaps to right to left 
      }
      else
      {
        MotherShip.Ord.X=-MOTHERSHIP_WIDTH;
        MotherShipSpeed=MOTHERSHIP_SPEED;   // set to go left ot right      
      }
    }
  }
}
 
void PlayerControl(){
  // user input checks
  if((leftButtonPressed())&(Player.Ord.X+TANKGFX_WIDTH<SCREEN_WIDTH))
    Player.Ord.X+=PLAYER_X_MOVE_AMOUNT;
  if((rightButtonPressed())&(Player.Ord.X>0))
    Player.Ord.X-=PLAYER_X_MOVE_AMOUNT;
  if(((fireButtonPressed()||fire2ButtonPressed())&(Missile.Status!=ACTIVE)))
  {
    Missile.X=Player.Ord.X+(TANKGFX_WIDTH/2);
    Missile.Y=PLAYER_Y_START;
    Missile.Status=ACTIVE;
    //noiseAC(200,10,&ShootCompleted);
    toneAC(600,5,200,true);
    ShootCompleted=true;
  }
}

void MissileControl(void){ 
  if(Missile.Status==ACTIVE)  
  {
    Missile.Y-=MISSILE_SPEED;
    if(Missile.Y+MISSILE_HEIGHT<0)          // If off top of screen destroy so can be used again
      Missile.Status=DESTROYED;
  }
}


void AlienControl(void){
  if((InvadersMoveCounter--)<0)
  {
    bool Dropped=false;
    if((RightMostPos()+AlienXMoveAmount>=SCREEN_WIDTH) |(LeftMostPos()+AlienXMoveAmount<0)) // at edge of screen
    {
      AlienXMoveAmount=-AlienXMoveAmount;   // reverse direction
      Dropped=true;                         // and indicate we are dropping
    }

    // play background music note if other higher priority sounds not playing
    if((ShootCompleted)&(MotherShip.Ord.Status!=ACTIVE))  {
      //toneAC2(spkr_pos,spkr_neg,Music[MusicIndex],100,true);
      toneAC(Music[MusicIndex],2,100,true);
      MusicIndex++;
      if(MusicIndex==sizeof(Music))
        MusicIndex=0;
      MusicCounter=NOTELENGTH;             //I'm not sure what MusicCounter does?
    }
    // update the alien postions
    for(int Across=0;Across<NUM_ALIEN_COLUMNS;Across++) 
    {
      for(int Down=0;Down<3;Down++)
      {
        if(Alien[Across][Down].Ord.Status==ACTIVE)
        {
          if(Dropped==false)
            Alien[Across][Down].Ord.X+=AlienXMoveAmount;
          else
            Alien[Across][Down].Ord.Y+=INVADERS_DROP_BY;
        }
      }
    }  
    InvadersMoveCounter=Player.AlienSpeed;
    AnimationFrame=!AnimationFrame;         //swap to other frame
  }
  // should the alien drop a bomb
  if(random(CHANCEOFBOMBDROPPING)==1)  
    DropBomb();
  MoveBombs();
}
 
void MoveBombs(void){
  for(int i=0;i<MAXBOMBS;i++){
    if(AlienBomb[i].Status==ACTIVE)
      AlienBomb[i].Y+=2;
  }
}

void DropBomb(void){
  // if there is a free bomb slot then drop a bomb else nothing happens
  bool Free=false;
  uint8_t ActiveCols[NUM_ALIEN_COLUMNS];  
  uint8_t BombIdx=0;
  // find a free bomb slot
  while((Free==false)&(BombIdx<MAXBOMBS)){
    if(AlienBomb[BombIdx].Status==DESTROYED)
      Free=true;
    else
      BombIdx++;
  }
  if(Free)  {
    uint8_t Columns=0;
    // now pick and alien at random to drop the bomb
    // we first pick a column, obviously some columns may not exist, so we count number of remaining cols 
    // first, this adds time but then also picking columns randomly that don't exist may take time also
    uint8_t ActiveColCount=0;
    signed char Row;
    uint8_t ChosenColumn;
    
    while(Columns<NUM_ALIEN_COLUMNS){
      Row=2;
      while(Row>=0)  {
        if(Alien[Columns][Row].Ord.Status==ACTIVE)
        {
          ActiveCols[ActiveColCount]=Columns;
          ActiveColCount++;
          break;
        }
        --Row;
      }
      Columns++;
    }
    // we have ActiveCols array filled with the column numbers of the active cols and we have how many
    // in ActiveColCount, now choose a column at random
    ChosenColumn=random(ActiveColCount);  // random number between 0 and the amount of columns
    // we now find the first available alien in this column to drop the bomb from
    Row=2;
    while(Row>=0)  {
      if(Alien[ActiveCols[ChosenColumn]][Row].Ord.Status==ACTIVE)  {
        // Set the bomb from this alien
        AlienBomb[BombIdx].Status=ACTIVE;
        AlienBomb[BombIdx].X=Alien[ActiveCols[ChosenColumn]][Row].Ord.X+(int)(AlienWidth[Row]/2);
        // above sets bomb to drop around invaders center, here we add a little randomness around this pos
        AlienBomb[BombIdx].X=(AlienBomb[BombIdx].X-2)+random(4);
        AlienBomb[BombIdx].Y=Alien[ActiveCols[ChosenColumn]][Row].Ord.Y+4;        
        break;
      }
      --Row;
    }
  }
}

void BombCollisions(){
  //check bombs collisions
  for(int i=0;i<MAXBOMBS;i++)
  {
    if(AlienBomb[i].Status==ACTIVE)
    {
      if(AlienBomb[i].Y>64)                 // gone off bottom of screen
        AlienBomb[i].Status=DESTROYED;
      else  
      {
        // HAS IT HIT PLAYERS missile
          if(Collision(AlienBomb[i],BOMB_WIDTH,BOMB_HEIGHT,Missile,MISSILE_WIDTH,MISSILE_HEIGHT))
          {
              // destroy missile and bomb
              AlienBomb[i].Status=EXPLODING;
              Missile.Status=DESTROYED;
          }
          else
          {      
             // has it hit players ship          
            if(Collision(AlienBomb[i],BOMB_WIDTH,BOMB_HEIGHT,Player.Ord,TANKGFX_WIDTH,TANKGFX_HEIGHT))
            {
               PlayerHit();
               AlienBomb[i].Status=DESTROYED;
            }
            else  
              BombAndBasesCollision(&AlienBomb[i]);
          }
       }
    }
  }  
}

void BombAndBasesCollision(GameObjectStruct *Bomb){
  // check and handle any bomb and base collision
  for(int i=0;i<NUM_BASES;i++)
  {
    if(Collision(*Bomb,BOMB_WIDTH,BOMB_HEIGHT,Base[i].Ord,BASE_WIDTH,BASE_HEIGHT)) 
    {
      /* Now get the position of the bomb within the structure so we can destroy it bit by bit
         we first get the relative position from the left hand side of the base
         then we divide this by 2 (X>>1)  , this gives us a position with 2bit resolution
      */
      uint8_t X=Bomb->X-Base[i].Ord.X;
      X=X>>1;
        
      /* Because the bomb is 2 pixels wide it's X pos could have been less than the bases XPos, if this is the case the above substaction
         on an uint8_t (uint8_t) will cause a large number to occur (255), we check for this and if this large number has
         resulted we just set X to 0 for the start of the bases structure
      */
      if(X>7)  X=0; 

      /* We now look at wether the bomb is hitting a part of the structure below it.
         the bomb may be past the top of the base (even on first collision detection) as it moves in Y amounts greater than 1 pixel to give a higher speed
         so we start from top of the base and destroy any structure of the base that comes before or equal to the
         bombs Y pos, if we go past the bomb Y without finding any bit of base then stop looking and bomb is allowed to progress further down
      */
      signed char Bomb_Y=(Bomb->Y+BOMB_HEIGHT)-Base[i].Ord.Y;
      uint8_t Base_Y=0;

      while((Base_Y<=Bomb_Y)&(Base_Y<BASE_HEIGHT)&(Bomb->Status==ACTIVE))
      {
        uint8_t Idx=(Base_Y*BASE_WIDTH_IN_u8S)+(X>>2);  // this gets the index for the uint8_t in question from the gfx array
        uint8_t Theu8=Base[i].Gfx[Idx];   // we now have the uint8_t to inspect, but need to only look at the 2 bits where the bomb is colliding
        // now isolate the 2 bits in question, we have the correct uint8_t and we only have need the bottom 2 bits of X now to denote which bits
        // we need to destroy in the uint8_t itself
        uint8_t BitIdx=X & 3;       // this preserves the bottom 2 bits and removes the rest
        /* A value of X of 0 (zero) would mean we want to look at the first 2 bits of the uint8_t
           A value of X of 1 (zero) would mean we want to look at the second 2 bits of the uint8_t
           A value of X of 2 (zero) would mean we want to look at the third 2 bits of the uint8_t
           A value of X of 3 (zero) would mean we want to look at the fourth 2 bits of the uint8_t
        */
          
        // So we need an AND mask to isolate these bits depending on the value of X
        // Here it is and we initially set it to the first 2 bits
        //uint8_t Mask=B11000000;
        uint8_t Mask=0xCD;
        /* now we need to move those 2 "11"'s to the right depending on the value of X as noted above
           the next line may look odd but all we're doing is multiplying X by 2 initially, so the values above would become,0,2,4,6 rather than 0,1,2,3
           Then we move the bits in the mask by this new amount, check it above,a value of X of 2 would shift the bits 4 places, whichis correct!
        */
        Mask=Mask>>(BitIdx<<1);
          
        //now we peform a logical and to remove all bits (pixels) from around these two bits
        Theu8=Theu8 & Mask;
          
        // and if there are any set pixels (bits) then they must be destroyed, else the bomb is allowed to continue
        if(Theu8>0) 
        { 
          /* There are some pixels to destroy
             We need to remove the pixels(bits) where there were "11"'s in the mask and leave those intact where there were 0's
             easiest way, flip all 0's in the mask to ones and all 1's to 0's, the tilde "~" means invert (swap), reffered to as logical NOT
          */
          Mask=~Mask; 
          // we can then "AND" the uint8_t with the Mask to remove the bits but leave the rest intact
          Base[i].Gfx[Idx]=Base[i].Gfx[Idx] & Mask;

          // now do some collateral damage to surrounding bricks, try left hand side first
          if(X>0){          // not at far left, if we were there would be nothing to destroy to this left
            if(random(CHANCE_OF_BOMB_DAMAGE_TO_LEFT_OR_RIGHT))   // if true blow some bricks to the left hand side
            {
              if(X!=4)       // we not at start of second uint8_t
              {                  
                Mask=(Mask<<1)|1;
                Base[i].Gfx[Idx]=Base[i].Gfx[Idx] & Mask;
              }
              else          // we are at very start of second uint8_t, so wipe out adjacent pixel first uint8_t
              {                  
                //Base[i].Gfx[Idx-1]=Base[i].Gfx[Idx-1] & B11111110;
                Base[i].Gfx[Idx-1]=Base[i].Gfx[Idx-1] & 0x7f;
              }
            }
          } 
            
          // now do some collateral damage right hand side first
          if(X<7){          // not at far right, if we were there would be nothing to destroy to this right
            if(random(CHANCE_OF_BOMB_DAMAGE_TO_LEFT_OR_RIGHT))   // if true blow some bricks to the left hand side
            {
              if(X!=3)       // not at last pixel of left of first uint8_t
              {                  
                Mask=(Mask>>1)|128;
                Base[i].Gfx[Idx]=Base[i].Gfx[Idx] & Mask;
              }
              else          // we are at last pixel of first uint8_t so destroy pixil in adjacent uint8_t
              {                  
                //Base[i].Gfx[Idx+1]=Base[i].Gfx[Idx+1] & B01111111;
                Base[i].Gfx[Idx+1]=Base[i].Gfx[Idx+1] & 0xfe;
              }
            }
          }
          if(random(CHANCE_OF_BOMB_PENETRATING_DOWN)==false)   // if false BOMB EXPLODES else carries on destroying more
            Bomb->Status=EXPLODING;
        }
        else
          Base_Y++;
      }
    }
  }
}

void MissileAndBasesCollisions(){
  // check and handle any player missile and base collision
  for(int i=0;i<NUM_BASES;i++)
  { 
      if(Collision(Missile,MISSILE_WIDTH,MISSILE_HEIGHT,Base[i].Ord,BASE_WIDTH,BASE_HEIGHT)) 
      {
        /* Now get the position of the bomb within the structure so we can destroy it bit by bit
           we first get the relative position from the left hand side of the base
           then we divide this by 2 (X>>1)  , this gives us a position with 2bit resolution
        */
        uint8_t X=Missile.X-Base[i].Ord.X;
        X=X>>1;
        
        /* Because the bomb is 2 pixels wide it's X pos could have been less than the bases XPos, if this is the case the above substaction
           on an uint8_t (uint8_t) will cause a large number to occur (255), we check for this and if this large number has
           resulted we just set X to 0 for the start of the bases structure
        */
        if(X>7)  X=0; 

        /* We now look at wether the bomb is hitting a part of the structure above it.
           the bomb may be past the top of the base (even on first collision detection) as it moves in Y amounts greater than 1 pixel to give a higher speed
           so we start from top of the base and destroy any structure of the base that comes before or equal to the
           bombs Y pos, if we go past the bomb Y without finding any bit of base then stop looking and bomb is allowed to progress further down
        */
        signed char Missile_Y=Missile.Y-Base[i].Ord.Y;
        signed char Base_Y=BASE_HEIGHT-1;
        while((Base_Y>=Missile_Y)&(Base_Y>=0)&(Missile.Status==ACTIVE))
        {
         // if(Base_Y<0) {Serial.println("oop"); Delay_Ms(999999);}
          uint8_t Idx=(Base_Y*BASE_WIDTH_IN_u8S)+(X>>2);  // this gets the index for the uint8_t in question from the gfx array
          uint8_t Theu8=Base[i].Gfx[Idx];   // we now have the uint8_t to inspect, but need to only look at the 2 bits where the bomb is colliding
          
          // now isolate the 2 bits in question, we have the correct uint8_t and we only have need the bottom 2 bits of X now to denote which bits
          // we need to destroy in the uint8_t itself
          uint8_t BitIdx=X & 3;       // this preserves the bottom 2 bits and removes the rest
          /* A value of X of 0 (zero) would mean we want to look at the first 2 bits of the uint8_t
             A value of X of 1 (zero) would mean we want to look at the second 2 bits of the uint8_t
             A value of X of 2 (zero) would mean we want to look at the third 2 bits of the uint8_t
             A value of X of 3 (zero) would mean we want to look at the fourth 2 bits of the uint8_t
          */
          
          // So we need an AND mask to isolate these bits depending on the value of X
          // Here it is and we initially set it to the first 2 bits
          //uint8_t Mask=B11000000;
          uint8_t Mask=0xCD;
          /* now we need to move those 2 "11"'s to the right depending on the value of X as noted above
             the next line may look odd but all we're doing is multiplying X by 2 initially, so the values above would become,0,2,4,6 rather than 0,1,2,3
             Then we move the bits in the mask by this new amount, check it above,a value of X of 2 would shift the bits 4 places, whichis correct! 
          */
          Mask=Mask>>(BitIdx<<1);
          
          //now we peform a logical AND to remove all bits (pixels) from around these two bits
          Theu8=Theu8 & Mask;
          
          // and if there are any set pixels (bits) then they must be destroyed, else the bomb is allowed to continue
          if(Theu8>0) 
          { 
            /* There are some pixels to destroy
               We need to remove the pixels(bits) where there were "11"'s in the mask and leave those intact where there were 0's
               easiest way, flip all 0's in the mask to ones and all 1's to 0's, the tilde "~" means invert (swap), reffered to as logical NOT
            */
            Mask=~Mask; 
            // we can then "AND" the uint8_t with the Mask to remove the bits but leave the rest intact
            Base[i].Gfx[Idx]=Base[i].Gfx[Idx] & Mask;

            // now do some collateral damage to surrounding bricks, try left hand side first
            if(X>0){                        // not at far left, if we were there would be nothing to destroy to this left
              if(random(CHANCE_OF_BOMB_DAMAGE_TO_LEFT_OR_RIGHT))   // if true blow some bricks to the left hand side
              {
                if(X!=4)                    // we not at start of second uint8_t
                {                  
                  Mask=(Mask<<1)|1;
                  Base[i].Gfx[Idx]=Base[i].Gfx[Idx] & Mask;
                }
                else                        // we are at very start of second uint8_t, so wipe out adjacent pixel first uint8_t
                {                  
                  //Base[i].Gfx[Idx-1]=Base[i].Gfx[Idx-1] & B11111110;
                  Base[i].Gfx[Idx-1]=Base[i].Gfx[Idx-1] & 0x7f;
                }
              }
            } 
            
            // now do some collateral damage right hand side first
            if(X<7){                        // not at far right, if we were there would be nothing to destroy to this right
              if(random(CHANCE_OF_BOMB_DAMAGE_TO_LEFT_OR_RIGHT))   // if true blow some bricks to the left hand side
              {
                if(X!=3)                    // not at last pixel of left of first uint8_t
                {                  
                  Mask=(Mask>>1)|128;
                  Base[i].Gfx[Idx]=Base[i].Gfx[Idx] & Mask;
                }
                else                        // we are at last pixel of first uint8_t so destroy pixil in adjacent uint8_t
                {                  
                  //Base[i].Gfx[Idx+1]=Base[i].Gfx[Idx+1] & B01111111;
                  Base[i].Gfx[Idx+1]=Base[i].Gfx[Idx+1] & 0xfe;
                }
              }
            }
            if(random(CHANCE_OF_BOMB_PENETRATING_DOWN)==false)   // if false BOMB EXPLODES else carries on destroying more
              Missile.Status=EXPLODING;
          }
          else
            Base_Y--;
        }
      }
  }
}

void PlayerHit(void){
  Player.Ord.Status=EXPLODING;
  Player.ExplosionGfxCounter=PLAYER_EXPLOSION_TIME;
  Missile.Status=DESTROYED;
  noiseAC(500,10,&PlayerExplosionNoiseCompleted);
}

void CheckCollisions(void){
  MissileAndAlienCollisions();
  MotherShipCollisions();
  MissileAndBasesCollisions();
  BombCollisions(); 
  AlienAndBaseCollisions();
}

char GetAlienBaseCollisionMask(int AlienX, int AlienWidth,int BaseX){
  signed int DamageWidth;
  uint8_t LeftMask,RightMask,FullMask;
  /* this routine uses a 1 to mean remove bit and 0 to preserve, this is kind of opposite of what we would 
     normally think of, but it's done this way as when we perform bit shifting to show which bits are preserved
     it will shift in 0's in (as that's just was the C shift operater ">>" and "<<" does
     at the end we will flip all the bits 0 becomes 1, 1 becomes 0 etc. so that the mask then works correctly
     with the calling code
  */
  
  //LeftMask=B11111111;     // initially set to remove all
  //RightMask=B11111111;    // unless change in code below                
  LeftMask=0xff;     // initially set to remove all
  RightMask=0xff;    // unless change in code below                
  // if Alien X more than base x then some start bits are unaffected
  if(AlienX>BaseX)  
  {                    
    // we shift the bits above to the right by the amount unaffected, thus putting 0's into the bits
    // not to delete
    DamageWidth=AlienX-BaseX;
    LeftMask>>=DamageWidth;          
  }
  
  // now work out how much of remaining uint8_t is affected
  
  // if right hand side of alien is less than BaseX right hand side then some preserved at the right hand end
  if(AlienX+AlienWidth<BaseX+(BASE_WIDTH/2))
  {
    DamageWidth=(BaseX+(BASE_WIDTH/2))-(AlienX+AlienWidth);
    RightMask<<=DamageWidth;        
  }
  /* We now have two masks, one showing which bits to preserve on left of the uint8_t, the other the right hand side, 
     we need to combine them to one mask, the code in the brackets does this combining
     at the moment a 0 means keep, 1 destroy, but this is actually makes it difficult to implement later on, so we flip
     the bits to be a more logical 1= keep bit and 0 remove bit (pixel) and then return the mask
     the tilde (~) flips the bits that resulted from combining the two masks
  */
  return ~(LeftMask & RightMask);
}

void DestroyBase(GameObjectStruct *Alien,BaseStruct *Base,char Mask,int Baseu8Offset){
  signed char Y;
  // go down "removing" bits to the depth that the alien is down into the base
  Y=(Alien->Y+ALIEN_HEIGHT)-Base->Ord.Y;
  if(Y>BASE_HEIGHT-1) Y=BASE_HEIGHT-1;
  for(;Y>=0;Y--){
    Base->Gfx[(Y*2)+Baseu8Offset]=Base->Gfx[(Y*2)+Baseu8Offset] & Mask;
  }
}

void AlienAndBaseCollisions(void){
  uint8_t Mask;
  // checks if aliens are in collision with the tank
  // start at bottom row as they are most likely to be in contact or not and if not then none further up are either
  for(int row=2;row>=0;row--)
  {
    for(int column=0;column<NUM_ALIEN_COLUMNS;column++)
    {
      if(Alien[column][row].Ord.Status==ACTIVE)
      {
        // now scan for a collision with each base in turn
        for(int BaseIdx=0;BaseIdx<NUM_BASES;BaseIdx++)
        {          
          if(Collision(Alien[column][row].Ord,AlienWidth[row],ALIEN_HEIGHT,Base[BaseIdx].Ord,BASE_WIDTH,BASE_HEIGHT))
          {
            // WE HAVE A COLLSISION, REMOVE BITS OF BUILDING
            // process left half (first uint8_t) of base first
            Mask=GetAlienBaseCollisionMask(Alien[column][row].Ord.X,AlienWidth[row],Base[BaseIdx].Ord.X);
            DestroyBase(&Alien[column][row].Ord,&Base[BaseIdx],Mask,0);
            
            // do right half, second uint8_t of base
            Mask=GetAlienBaseCollisionMask(Alien[column][row].Ord.X,AlienWidth[row],Base[BaseIdx].Ord.X+(BASE_WIDTH/2));
            DestroyBase(&Alien[column][row].Ord,&Base[BaseIdx],Mask,1);            
          }
        }
      }
    }
  }
}

void MotherShipCollisions(void){
  if((Missile.Status==ACTIVE)&(MotherShip.Ord.Status==ACTIVE))
  {
    if(Collision(Missile,MISSILE_WIDTH,MISSILE_HEIGHT,MotherShip.Ord,MOTHERSHIP_WIDTH,MOTHERSHIP_HEIGHT))
    {
      MotherShip.Ord.Status=EXPLODING;
      MotherShip.ExplosionGfxCounter=EXPLOSION_GFX_TIME;
      Missile.Status=DESTROYED;
      // generate the score for the mothership hit, note in the real arcade space invaders the score was not random but 
      // just appeared so, a player could infulence its value with clever play, but we'll keep it a little simpler
      MotherShipBonus=random(4); // a random number between 0 and 3
      switch(MotherShipBonus)
      {
        case 0:MotherShipBonus=50;break;
        case 1:MotherShipBonus=100;break;
        case 2:MotherShipBonus=150;break;
        case 3:MotherShipBonus=300;break;
      }
      Player.Score+=MotherShipBonus + MotherShipType*100;
      MotherShipBonusXPos=MotherShip.Ord.X;    
      if(MotherShipBonusXPos>100)                   // to ensure isn't half off right hand side of screen
        MotherShipBonusXPos=100;       
      if(MotherShipBonusXPos<0)                     // to ensure isn't half off right hand side of screen
        MotherShipBonusXPos=0;       
      MotherShipBonusCounter=DISPLAY_MOTHERSHIP_BONUS_TIME;        
    }
  }
}

void MissileAndAlienCollisions(void){
  for(int across=0;across<NUM_ALIEN_COLUMNS;across++)
  {
    for(int down=0;down<NUM_ALIEN_ROWS;down++)
    {
      if(Alien[across][down].Ord.Status==ACTIVE)
      {
        if(Missile.Status==ACTIVE)
        {
          if(Collision(Missile,MISSILE_WIDTH,MISSILE_HEIGHT,Alien[across][down].Ord,AlienWidth[down],INVADER_HEIGHT))
          {
              // missile hit
              Alien[across][down].Ord.Status=EXPLODING;
              //toneAC2(spkr_pos,spkr_neg,700,100,true);
              toneAC(700,10,100,true);
              Missile.Status=DESTROYED;
              Player.Score+=GetScoreForAlien(down);
              Player.AliensDestroyed++;
              // calc new speed of aliens, note (float) must be before TOTAL_ALIENS to force float calc else
              // you will get an incorrect result
              Player.AlienSpeed=((1-(Player.AliensDestroyed/(float)TOTAL_ALIENS))*INVADERS_SPEED);              
              // for very last alien make to  fast!
              if(Player.AliensDestroyed==TOTAL_ALIENS-2) {
                if(AlienXMoveAmount>0)
                  AlienXMoveAmount=ALIEN_X_MOVE_AMOUNT*2;
                else
                  AlienXMoveAmount=-(ALIEN_X_MOVE_AMOUNT*2);
              }
              // for very last alien make to super fast!
              if(Player.AliensDestroyed==TOTAL_ALIENS-1) {
                if(AlienXMoveAmount>0)
                  AlienXMoveAmount=ALIEN_X_MOVE_AMOUNT*4;
                else
                  AlienXMoveAmount=-(ALIEN_X_MOVE_AMOUNT*4);
              }
              if(Player.AliensDestroyed==TOTAL_ALIENS)
                NextLevel(&Player);            
          }
        }
          if(Alien[across][down].Ord.Status==ACTIVE)      // double check still active afer above code
        {
          // check if this alien is in contact with TankGfx
          if(Collision(Player.Ord,TANKGFX_WIDTH,TANKGFX_HEIGHT,Alien[across][down].Ord,AlienWidth[down],ALIEN_HEIGHT))
             PlayerHit();  
          else 
          {
            // check if alien is below bottom of screen
            if(Alien[across][down].Ord.Y+8>SCREEN_HEIGHT)
              PlayerHit();                   
          }
        }
      }
    }
  }
}

bool Collision(GameObjectStruct Obj1,uint8_t Width1,uint8_t Height1,GameObjectStruct Obj2,uint8_t Width2,uint8_t Height2){
  return ((Obj1.X+Width1>Obj2.X)&(Obj1.X<Obj2.X+Width2)&(Obj1.Y+Height1>Obj2.Y)&(Obj1.Y<Obj2.Y+Height2));
}

int RightMostPos(void){
  //returns x pos of right most alien
  int Across=NUM_ALIEN_COLUMNS-1;
  int Down;
  int Largest=0;
  int RightPos;
  while(Across>=0){
    Down=0;
    while(Down<NUM_ALIEN_ROWS){
      if(Alien[Across][Down].Ord.Status==ACTIVE)
      {
        // different aliens have different widths, add to x pos to get rightpos
        RightPos= Alien[Across][Down].Ord.X+AlienWidth[Down];
        if(RightPos>Largest)
          Largest=RightPos;
      }
      Down++;
    }
    if(Largest>0)  // we have found largest for this column
      return Largest;
    Across--;
  }
  return 0;       // should never get this far
}
 
int LeftMostPos(){
  //returns x pos of left most alien
  int Across=0;
  int Down;
  int Smallest=SCREEN_WIDTH*2;
  while(Across<NUM_ALIEN_COLUMNS){
    Down=0;
    while(Down<3){
      if(Alien[Across][Down].Ord.Status==ACTIVE)
        if(Alien[Across][Down].Ord.X<Smallest)
          Smallest=Alien[Across][Down].Ord.X;
      Down++;
    }
    if(Smallest<SCREEN_WIDTH*2)  // we have found smallest for this column
      return Smallest;
    Across++;
  }
  return 0;  // should nevr get this far
}
 
void UpdateDisplay(){
  int i; 
  uint8_t RowHeight;
  
  //display.clearDisplay(); 
  u8g2_ClearBuffer(&u8g2);
  RowHeight = FONT_Ascent; 
  // Mothership bonus display if required
  if(MotherShipBonusCounter>0)
  {
    // mothership bonus
    char bonus[8]; sprintf(bonus, "%d", MotherShipBonus);

    u8g2_DrawStr(&u8g2,MotherShipBonusXPos,RowHeight, bonus);
    MotherShipBonusCounter--;   
  } else {
    // draw score and lives, anything else can go above them
    char score[8]; sprintf(score, "%d", Player.Score);
    u8g2_DrawStr(&u8g2,0,RowHeight, score);

    char lives[8]; sprintf(lives, "%d", Player.Lives);
    u8g2_DrawStr(&u8g2,SCREEN_WIDTH-7,RowHeight, lives);
  }   

  //BOMBS
  // draw bombs next as aliens have priority of overlapping them
  for(i=0;i<MAXBOMBS;i++)  {
      if(AlienBomb[i].Status==ACTIVE)
        //old code:
        //display.drawBitmap(AlienBomb[i].X, AlienBomb[i].Y,  AlienBombGfx, 2, 4,WHITE);
        //New code:
        u8g2_DrawXBMP(&u8g2,AlienBomb[i].X, AlienBomb[i].Y, 2, 4, AlienBombGfx);
        //drawXBMP(x, y, w, h, *bitmap)
      else  {// must be destroyed
        if(AlienBomb[i].Status==EXPLODING)
          //olde code:
          //display.drawBitmap(AlienBomb[i].X-4, AlienBomb[i].Y,  ExplosionGfx, 4, 8,WHITE);
          //New code:
          u8g2_DrawXBMP(&u8g2,AlienBomb[i].X-4, AlienBomb[i].Y, 4, 8, ExplosionGfx);
          //drawXBMP(x, y, w, h, *bitmap)
          
        // Ensure on next draw that ExplosionGfx dissapears
        AlienBomb[i].Status=DESTROYED;
      }
  }
  
  //Invaders
  for(int across=0;across<NUM_ALIEN_COLUMNS;across++){
    for(int down=0;down<NUM_ALIEN_ROWS;down++){
      if(Alien[across][down].Ord.Status==ACTIVE){
        int j;
        if(AnimationFrame){j=0;}else{j=1;}
        switch(down)  {
          case 0: 
            //if(AnimationFrame)
            //  display.drawBitmap(Alien[across][down].Ord.X, Alien[across][down].Ord.Y,  InvaderTopGfx, AlienWidth[down],INVADER_HEIGHT,WHITE);
            //else
            //  display.drawBitmap(Alien[across][down].Ord.X, Alien[across][down].Ord.Y,  InvaderTopGfx2, AlienWidth[down],INVADER_HEIGHT,WHITE);
            u8g2_DrawXBMP(&u8g2,Alien[across][down].Ord.X, Alien[across][down].Ord.Y, AlienWidth[down], INVADER_HEIGHT, InvaderTopGfx[j]);
            break;
          case 1: 
            //if(AnimationFrame)
            //  display.drawBitmap(Alien[across][down].Ord.X, Alien[across][down].Ord.Y,  InvaderMiddleGfx, AlienWidth[down],INVADER_HEIGHT,WHITE);
            //else
            //  display.drawBitmap(Alien[across][down].Ord.X, Alien[across][down].Ord.Y,  InvaderMiddleGfx2, AlienWidth[down],INVADER_HEIGHT,WHITE);
            u8g2_DrawXBMP(&u8g2,Alien[across][down].Ord.X, Alien[across][down].Ord.Y, AlienWidth[down], INVADER_HEIGHT, InvaderMiddleGfx[j]);
            break;
          default: 
            //if(AnimationFrame)
            //  display.drawBitmap(Alien[across][down].Ord.X, Alien[across][down].Ord.Y,  InvaderBottomGfx, AlienWidth[down],INVADER_HEIGHT,WHITE);
            //else
            //  display.drawBitmap(Alien[across][down].Ord.X, Alien[across][down].Ord.Y,  InvaderBottomGfx2, AlienWidth[down],INVADER_HEIGHT,WHITE);
            u8g2_DrawXBMP(&u8g2,Alien[across][down].Ord.X, Alien[across][down].Ord.Y, AlienWidth[down], INVADER_HEIGHT, InvaderBottomGfx[j]);
        }  //end switch
      } else {
        if(Alien[across][down].Ord.Status==EXPLODING){
          Alien[across][down].ExplosionGfxCounter--;
          if(Alien[across][down].ExplosionGfxCounter>0)  {
            //toneAC2(spkr_pos,spkr_neg,Alien[across][down].ExplosionGfxCounter*100,100,true);
            toneAC(Alien[across][down].ExplosionGfxCounter*100,10,100,true);
            //display.drawBitmap(Alien[across][down].Ord.X, Alien[across][down].Ord.Y,  ExplosionGfx, 13, 8,WHITE);
            u8g2_DrawXBMP(&u8g2,Alien[across][down].Ord.X, Alien[across][down].Ord.Y, 13, 8, ExplosionGfx);
          } else
            Alien[across][down].Ord.Status=DESTROYED;
        }
      }//end if
    }//end for
  }// end for  
  
  // player
  if(Player.Ord.Status==ACTIVE)
    //display.drawBitmap(Player.Ord.X, Player.Ord.Y,  TankGfx, TANKGFX_WIDTH, TANKGFX_HEIGHT,WHITE);
    u8g2_DrawXBMP(&u8g2,Player.Ord.X, Player.Ord.Y, TANKGFX_WIDTH, TANKGFX_HEIGHT, TankGfx);
  else {    
    if(Player.Ord.Status==EXPLODING)  {
      for(i=0;i<TANKGFX_WIDTH;i+=2)  {
        //display.drawBitmap(Player.Ord.X+i, Player.Ord.Y,  ExplosionGfx, random(4)+2, 8,WHITE);
        u8g2_DrawXBMP(&u8g2,Player.Ord.X+i, Player.Ord.Y, random(4)+2, 8, ExplosionGfx);
      }
      Player.ExplosionGfxCounter--;
      if(Player.ExplosionGfxCounter==0)  {
        Player.Ord.Status=DESTROYED;
        Delay_Ms(500);                     // small delay after tank explodes and player status screen
        LoseLife();
      }
    }
  }
  //missile  
  if(Missile.Status==ACTIVE)
    //display.drawBitmap(Missile.X, Missile.Y,  MissileGfx, MISSILE_WIDTH, MISSILE_HEIGHT,WHITE);
    u8g2_DrawXBMP(&u8g2,Missile.X, Missile.Y, MISSILE_WIDTH, MISSILE_HEIGHT, MissileGfx);

  // mothership (not bonus if hit)
  if(MotherShip.Ord.Status==ACTIVE){
    //display.drawBitmap(MotherShip.Ord.X, MotherShip.Ord.Y,  MotherShipGfx, MOTHERSHIP_WIDTH, MOTHERSHIP_HEIGHT,WHITE);
    u8g2_DrawXBMP(&u8g2,MotherShip.Ord.X, MotherShip.Ord.Y, MOTHERSHIP_WIDTH, MOTHERSHIP_HEIGHT, MotherShipGfx[MotherShipType]);
  } else  {
    if(MotherShip.Ord.Status==EXPLODING)  
    {
      for(i=0;i<MOTHERSHIP_WIDTH;i+=2)  {
        //display.drawBitmap(MotherShip.Ord.X+i, MotherShip.Ord.Y,  ExplosionGfx, random(4)+2, MOTHERSHIP_HEIGHT,WHITE);
        u8g2_DrawXBMP(&u8g2,MotherShip.Ord.X+i, MotherShip.Ord.Y, random(4)+2, MOTHERSHIP_HEIGHT, ExplosionGfx);
      }
      //toneAC2( pin1, pin2, frequency [, length [, background ]] ) 
      //toneAC2(spkr_pos,spkr_neg,MotherShip.ExplosionGfxCounter*50,100,true);
      toneAC(MotherShip.ExplosionGfxCounter*50,10,100,true);
      MotherShip.ExplosionGfxCounter--;
      if(MotherShip.ExplosionGfxCounter==0)  {
        MotherShip.Ord.Status=DESTROYED;
      }
    }
  }

  // plot bases
  
  for(i=0;i<NUM_BASES;i++)  {    
    if(Base[i].Ord.Status==ACTIVE)
      //display.drawBitmap(Base[i].Ord.X, Base[i].Ord.Y,  Base[i].Gfx, BASE_WIDTH, BASE_HEIGHT,WHITE,true);
      u8g2_DrawXBM(&u8g2,Base[i].Ord.X, Base[i].Ord.Y, BASE_WIDTH, BASE_HEIGHT, Base[i].Gfx);
  }
  //display.display();
  u8g2_SendBuffer(&u8g2);
}

void LoseLife(void){
  Player.Lives--;
  if(Player.Lives>0)  {
    DisplayPlayerAndLives(&Player);
    // clear alien missiles
    for(int i=0;i<MAXBOMBS;i++)  {
      AlienBomb[i].Status=DESTROYED;
      AlienBomb[i].Y=0;
    }
    // ENABLE PLAYER
    Player.Ord.Status=ACTIVE;
    Player.Ord.X=0;
  } else  {
    GameOver();
  }
}

void GameOver(){  
  uint8_t RowHeight;
  uint8_t ColPosition;
  GameInPlay=false;
  //display.clearDisplay();
  u8g2_ClearBuffer(&u8g2);   
  if(Player.Score>HiScore){
    RowHeight = FONT_Ascent;
  }else{
    RowHeight = FONT_Ascent+(SCREEN_HEIGHT - 4*(FONT_Ascent+FONT_Descent+1))/2;
  }
  CenterText("Player 1",RowHeight); 
  RowHeight = RowHeight+FONT_Ascent+FONT_Descent+1;
  CenterText("Game Over",RowHeight);   
  RowHeight = RowHeight+FONT_Ascent+FONT_Descent+1;
  // Special Center Text ---->
  ColPosition = u8g2_GetStrWidth(&u8g2,"8");
  int score_width = getDigits(HiScore);
  ColPosition = (int)((SCREEN_WIDTH - u8g2_GetStrWidth(&u8g2,"Score ") - ColPosition*score_width)/2.0);
  u8g2_DrawStr(&u8g2, ColPosition, RowHeight, "Score ");
  printNum(ColPosition + 20, RowHeight, Player.Score);

  if(Player.Score>HiScore){
    RowHeight = RowHeight+FONT_Ascent+FONT_Descent+1;
    CenterText("NEW HIGH SCORE!!!",RowHeight);
    RowHeight = RowHeight+FONT_Ascent+FONT_Descent+1;
    CenterText("**CONGRATULATIONS**",RowHeight);    
  }
  //display.display();
  u8g2_SendBuffer(&u8g2);
  if(Player.Score>HiScore){    
    setHighScore(Player.Score);
    PlayRewardMusic();
  }
  Delay_Ms(3000);  
}

void PlayRewardMusic(void){
  uint8_t Notes[] = { 26, 20, 18, 22, 20, 0, 26, 0, 26 };
  uint8_t NoteDurations[] = { 40, 20, 20, 40, 30, 50, 30, 10,30 };
  for(int i=0;i<9;i++) {
    //toneAC2( pin1, pin2, frequency [, length [, background ]] ) 
    //toneAC2(spkr_pos, spkr_neg, Notes[i]*10, 0, true);
    toneAC(Notes[i]*10,10,0,true);
    Delay_Ms(NoteDurations[i]*10);       // time not plays for
    noToneAC();
    Delay_Ms(20);                        // small delay between notes
  }
  noToneAC();
}

void DisplayPlayerAndLives(PlayerStruct *Player){
  uint8_t RowHeight;
  //display.clearDisplay(); 
  u8g2_ClearBuffer(&u8g2);
  RowHeight = FONT_Ascent+(SCREEN_HEIGHT - 4*(FONT_Ascent+FONT_Descent+1))/2;
  CenterText("Player 1",RowHeight);
  RowHeight = RowHeight+FONT_Ascent+FONT_Descent+1;
  CenterText("Score ",RowHeight);   
  printNum(Player->Score, SCREEN_WIDTH*3/2, RowHeight);
  RowHeight = RowHeight+FONT_Ascent+FONT_Descent+1;
  CenterText("Lives ",RowHeight);   
  printNum(Player->Lives, SCREEN_WIDTH*3/2, RowHeight);
  RowHeight = RowHeight+FONT_Ascent+FONT_Descent+1;
  CenterText("Level ",RowHeight);   
  printNum(Player->Level, SCREEN_WIDTH*3/2, RowHeight);
  //display.display();
  u8g2_SendBuffer(&u8g2);
  Delay_Ms(2000);
  Player->Ord.X=PLAYER_X_START;
}

void CenterText(const char *Text, uint8_t RowValue){
  // center text on screen
  u8g2_DrawStr(&u8g2,(int)((SCREEN_WIDTH - u8g2_GetStrWidth(&u8g2, Text))/2.0),RowValue, Text);
}

void InitPlayer(){
  Player.Ord.Y=PLAYER_Y_START;
  Player.Ord.X=PLAYER_X_START;
  Player.Ord.Status=ACTIVE;
  Player.Lives=LIVES;
  Player.Level=0;
  Missile.Status=DESTROYED;
  Player.Score=0;
}

void NextLevel(PlayerStruct *Player){
  // reset any dropping bombs
  int YStart;
  for(int i=0;i<MAXBOMBS;i++)
    AlienBomb[i].Status=DESTROYED;
  AnimationFrame=false;
  Player->Level++;
  YStart=((Player->Level-1) % LEVEL_TO_RESET_TO_START_HEIGHT)*AMOUNT_TO_DROP_BY_PER_LEVEL;
  InitAliens(YStart);  
  AlienXMoveAmount=ALIEN_X_MOVE_AMOUNT;
  Player->AlienSpeed=INVADERS_SPEED;
  Player->AliensDestroyed=0;
  MotherShip.Ord.X=-MOTHERSHIP_WIDTH;
  MotherShip.Ord.Status=DESTROYED; 
  Missile.Status=DESTROYED;
  InitBases();
  DisplayPlayerAndLives(Player);  
  MusicIndex=0;
  MusicCounter=NOTELENGTH;
}

void InitBases(void){
  // Bases need to be re-built!
  uint8_t Theu8;
  int Spacing = (SCREEN_WIDTH-(NUM_BASES*BASE_WIDTH))/NUM_BASES;
  for(int i=0; i<NUM_BASES; i++)  
  {    
    for(int DataIdx=0; DataIdx<BASE_HEIGHT*BASE_WIDTH_IN_u8S; DataIdx++)  
    {
        Theu8 = 5; // pgm_read_u8(BaseGfx + DataIdx);
        Base[i].Gfx[DataIdx] = Theu8;
    }
    Base[i].Ord.X = (i*Spacing)+(i*BASE_WIDTH)+(Spacing/2);
    Base[i].Ord.Y = BASE_Y;
    Base[i].Ord.Status = ACTIVE;
  }
}

void NewGame(){
  InitPlayer();
  NextLevel(&Player);
}

void InitAliens(int YStart){ 
  for(int across=0;across<NUM_ALIEN_COLUMNS;across++)  {
    for(int down=0;down<3;down++)  {
      // we add down to centralize the aliens, just happens to be the right value we need per row!
      // we need to adjust a little as row zero should be 2, row 1 should be 1 and bottom row 0
      Alien[across][down].Ord.X=X_START_OFFSET+(across*(LARGEST_ALIEN_WIDTH+SPACE_BETWEEN_ALIEN_COLUMNS))-(AlienWidth[down]/2);  
      Alien[across][down].Ord.Y=YStart+(down*SPACE_BETWEEN_ROWS);
      Alien[across][down].Ord.Status=ACTIVE;
      Alien[across][down].ExplosionGfxCounter=EXPLOSION_GFX_TIME;
    }
  }
  MotherShip.Ord.Y=0;
  MotherShip.Ord.X=-MOTHERSHIP_WIDTH;
  MotherShip.Ord.Status=DESTROYED; 
}

