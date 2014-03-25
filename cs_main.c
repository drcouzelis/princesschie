/**
 * Copyright 2009 David Couzelis
 * 
 * This file is part of "Princess Chie".
 * 
 * "Princess Chie" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * "Princess Chie" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with "Princess Chie".  If not, see <http://www.gnu.org/licenses/>.
 */
#include "cs_data.h"



// Make OO...
OO_DATA * makeGndWalker(OO_DATA *oo, FILE *file);
OO_DATA * makeGndImobileTosser(OO_DATA *oo, FILE *file);

// Update OO...
void updateGndWalker(OO_SHELL *oo, int hPos, int vPos, CC *cc, LEVEL *level);
void updateGndImobileTosser(OO_SHELL *oo, int hPos, int vPos, CC *cc, LEVEL *level);

// Initialize...
void initializeTileList(TILE **tiles, char *fileName);
void initializeOoData(OO_DATA **data, char *fileName);
void initializeItemList(ITEM **items, char *fileName);
void initializeOoList(OO_SHELL **list);
void initializeHighComputer(void);
void initializeLowComputer(void);
void initializeComputer(void);
void initializeTimer(void);
ATTACK_DATA * initializeAttack(char *fileName);
LEVEL * initializeLevel(char *fileName);
HIT_PIC * initializeHitPic(FILE *file);
ANIM * initializeAnim(FILE *animFile);
CC * initializeCc(char *fileName);

// Destroy...
void destroyAttackData(ATTACK_DATA *atkData);
void destroyHitPic(HIT_PIC *hitPic);
void destroyLevel(LEVEL *level);
void destroyOoData(OO_DATA *oo);
void destroyAnim(ANIM *anim);
void destroyTile(TILE *tile);
void destroyItem(ITEM *item);
void destroyCc(CC *cc);

// Update...
void updateAttackList(ATTACK_SHELL **atkList, int hRefPos, OO_SHELL **ooList);
void updateOo(LEVEL *level, int hPos, int vPos, CC *cc);
inline void updateScrollground(LEVEL *level, int hPos);
inline void updateWalkground(LEVEL *level, int hPos);
inline void updateCamera(int hPos);
void updateStatusBar(CC *cc);
void updateCc(CC *cc);

// State specific CC updating...
void doCcCrouching(CC *cc);
void doCcAttacking(CC *cc);
void doCcStanding(CC *cc);
void doCcHurting(CC *cc);
void doCcWalking(CC *cc);
void doCcJumping(CC *cc);

// Game...
inline int checkTime(int laterTime, int earlierTime);
inline void finishScreen(void);
inline void waitForGame(void);
void playNewGame(void);
void cleanUp(void);

// Collision and movement...
inline FLAG isInContact(int hPos1,int vPos1,int uRad1,int dRad1,int lRad1, int rRad1, int hPos2,int vPos2,int uRad2,int dRad2,int lRad2, int rRad2);
void movePPS(int *hPos,int *vPos, float *fudge, int uRad,int dRad,int lRad, int rRad, int *pps, AXIS axis);
void movePPSSansCollision(int *hPos,int *vPos, float *fudge, int *pps, AXIS axis);
inline void addGravity(int hPos,int vPos, int dRad,int lRad, int rRad, int *pps);
inline FLAG onSolid(int hPos, int vPos, int dRad, int lRad, int rRad);
inline FLAG checkPassable(int hPos, int vPos, DIRECTION dir);
inline void addCcFriction(CC *cc);

// Start...
void startOo(OO_SHELL *ooShell, OO_DATA *ooData, int sentHPos, int sentVPos, DIRECTION fromDir);
inline void startCcAttack(CC *cc, int sentHPos, int sentVPos);

// Reset...
void resetAttackShell(ATTACK_SHELL *atkShell, ATTACK_DATA *atkData);
void resetOoShell(OO_SHELL *oo);
void resetOoData(OO_DATA *oo);

// Non game specific...
void stateToString(STATE state, char *stateString);
inline void lowGameAdjust(int *num);
OO_TYPE toOoType(char *str);
int swipeln(FILE *file);

// Game specific...
inline int getFront(int hPos, int lRad, int rRad, DIRECTION dir);
inline void animate(ANIM *anim, int *timeMark, int *pos);
inline void animateTiles(TILE **tiles);

// CC state changing...
void ccToHurting(CC *cc, int hurtPoints);
void ccToCrouching(CC *cc);
void ccToAttacking(CC *cc);
void ccToStanding(CC *cc);
void ccToWalking(CC *cc);
void ccToJumping(CC *cc);

// OO specific...
inline void killOo(OO_SHELL *oo, DIRECTION dir);
inline void checkOoForCc(OO_SHELL *oo, CC *cc);
inline void drawOo(OO_SHELL *oo, int hPos);
inline void checkOoForLedge(OO_SHELL *oo);
inline void checkOoFaceCc(OO_SHELL *oo, CC *cc);
inline void checkOoForWall(OO_SHELL *oo);
inline void moveKilledOo(OO_SHELL *oo);




//Global variables...
volatile int timer;
static LEVEL *level;
PALLETE gamePallete;
FLAG quitting;
BITMAP *sBuffer;
int timeMark;
int ticks;
FLAG useScrollground;
FLAG useSound;
FLAG lowGame;
int camera;
int screenChange;
//FILE *debugFile;
//MIDI *bgm;
//MP3 *bgm;

int SW;
int SH;
int fullscreen;

int TILE_SIZE;
int SW_TILES;
int SH_TILES;
int GRAVITY;
int healthBarHPos;
int healthBarVPos;
int healthBarW;
int healthBarH;

// The game sound effects...
SAMPLE *jumpEffect;


/******************
***  do_timer   ***
******************/
// This controls the game time.
void do_timer(void)
{ timer+=1;
  if (timer >= MAX_TICKER)
    timer = 1;
} END_OF_FUNCTION (do_timer);



/****************
***   main    ***
****************/
// This is the main function.
int main(void)
{ BITMAP *title;
  //timeMark=0;
  ticks=0;
  camera=0;
  fullscreen=0;

  initializeComputer();
  initializeHighComputer();
  useSound = F;

  title = load_bitmap(TITLE_IMAGE,gamePallete);
  set_palette(gamePallete);

  // Next comes nice, procedural game code...
  // Riiiight...
  quitting = F;
  timeMark = timer;
  while(!quitting)
  { blit(title,sBuffer, 0,0, 0,0, SW,SH);
    if(keypressed())
    { if(key[KEY_1])
      { useScrollground=T;
        lowGame=F;
        playNewGame();
      }
      else if(key[KEY_2])
      { useScrollground=F;
        lowGame=F;
        playNewGame();
      }
      else if(key[KEY_3])
      { useScrollground=T;
        lowGame=T;
        cleanUp();
        initializeLowComputer();
        playNewGame();
      }
      else if(key[KEY_4])
      { useScrollground=F;
        lowGame=T;
        cleanUp();
        initializeLowComputer();
        playNewGame();
      }
      else if (key[KEY_ESC])
        quitting = T;
      else if (key[KEY_F]) {
        if (fullscreen == 0) {
          set_gfx_mode(GFX_AUTODETECT, SW,SH, 0,0);
          set_palette(gamePallete);
          fullscreen = 1;
        } else {
          set_gfx_mode(GFX_AUTODETECT_WINDOWED, SW,SH, 0,0);
          set_palette(gamePallete);
          fullscreen = 0;
        }
      }
      clear_keybuf();
    }
    finishScreen();
  }

  cleanUp();

  // Clean up sound effects...
  destroy_sample(jumpEffect);

  allegro_exit();
  return(1);
}
END_OF_MAIN()


/**********************
***   playNewGame   ***
**********************/
// This starts the player into a new game.
void playNewGame(void)
{ CC *mainCc;
  char levelDataFileName[30];
  char levelTilesFileName[30];
  FLAG gameOver=F;
  FLAG levelDone=F;

  clear_keybuf();
  clear(sBuffer);

  //load and play music
  //bgm =
  //bgm = load_midi(BGM_FILE_PATH);
  //play_midi(bgm,1);

  set_palette(gamePallete);
  //show openning scene

  strcpy(levelDataFileName,LEVEL_1_DATA);
  strcpy(levelTilesFileName,LEVEL_1_TILES);

  while(!gameOver)
  {
    // Initialize the cc from files...
    mainCc = initializeCc(CHIEKO_DATA);
    // Initialize the current level from file
    // and initialize necessary tiles from level...
    level = initializeLevel(levelDataFileName);
    // Initialize all of the picture sets...
    // Initialize the music from level...
    while(!levelDone)
    { //clear_to_color(sBuffer,BACKGROUND_COLOR);
      updateCamera(mainCc->hPos);
      if (key[KEY_ESC] || mainCc->health<=0)
      { levelDone=T;
      }
      if (key[KEY_F]) {
        if (fullscreen == 0) {
          set_gfx_mode(GFX_AUTODETECT, SW,SH, 0,0);
          set_palette(gamePallete);
          fullscreen = 1;
        } else {
          set_gfx_mode(GFX_AUTODETECT_WINDOWED, SW,SH, 0,0);
          set_palette(gamePallete);
          fullscreen = 0;
        }
      }
      //updateCcKeys(cc);
      // Update everything!...
      // Draw the farground...
      blit(level->farground,sBuffer, 0,0, 0,0, SW,SH);
      // Update and draw the scrollground...
      if(useScrollground)
        updateScrollground(level,mainCc->hPos-camera);
      // Update and draw the walkground...
      animateTiles(level->tiles);
      updateWalkground(level,mainCc->hPos-camera);
      // Update and draw all oos...
      // NOTE: When you go through and update
      // all of the oos and oo atks, give them
      // the current cc, so they can see if they
      // can hurt it...
      updateOo(level,mainCc->hPos-camera, mainCc->vPos, mainCc);
      // Update and draw the cc...
      updateCc(mainCc);
      // Update and draw all attacks...
      // NOTE: As you update each cc atk,
      // whip through the list of oos to see
      // if they are hit...
      updateAttackList(mainCc->attackList, mainCc->hPos-camera, level->ooList);
      //textprintf(sBuffer,font, 0,0,TEXT_COLOR, "pretending to start game...");
      //textprintf(sBuffer,font, 0,SH-30,TEXT_COLOR, "hPos %d",mainCc->hPos);
      //textprintf(sBuffer,font, 0,SH-30,TEXT_COLOR, "health %d",mainCc->health);
      updateStatusBar(mainCc);

      finishScreen();
    }
    // Show end level scene from level
    destroyLevel(level);
    destroyCc(mainCc);

    //levelNum ++
    gameOver=T;
  }

  //stop_midi();
  //destroy_midi(bgm);


  cleanUp();
  //if(lowGame)
  initializeHighComputer();
  set_palette(gamePallete);
}



/*
void updateCcKeys(CC *cc)
{
  if(key[KEY_LEFT])

}
*/



void updateStatusBar(CC *cc)
{ int healthPercent;
  int barOffset;

  if(lowGame)  barOffset=2;
  else  barOffset=4;

  healthPercent = (cc->health * healthBarH) / cc->maxHealth;

  rectfill(sBuffer, healthBarHPos-barOffset*2,healthBarVPos-barOffset*2, healthBarHPos+healthBarW+barOffset*2,healthBarVPos+healthBarH+barOffset*2, 215);  // White color..
  rectfill(sBuffer, healthBarHPos-barOffset,healthBarVPos-barOffset, healthBarHPos+healthBarW+barOffset,healthBarVPos+healthBarH+barOffset, 185);  // Black color..
  rectfill(sBuffer, healthBarHPos,healthBarVPos+(healthBarH-healthPercent), healthBarHPos+healthBarW,healthBarVPos+healthBarH, 204); // Cyan color...

}



/**********************
***   resetOoData   ***
**********************/
// Sent a pointer to an OO...
// This "cleans" (completely zeroes) the whole OO...
// NOTE::  This does NOT deallocate ANYTHING!...
void resetOoData(OO_DATA *oo)
{
  oo->walkAnim = NULL;
  oo->standAnim = NULL;
  oo->jumpAnim = NULL;
  oo->flyAnim = NULL;
  oo->fallAnim = NULL;
  oo->dieAnim = NULL;

  oo->hInitVel = 0;
  oo->vInitVel = 0;
  oo->initHealth = 0;

  oo->hHurtVel = 100;
  oo->vHurtVel = -475;
  lowGameAdjust(&(oo->hHurtVel));
  lowGameAdjust(&(oo->vHurtVel));

  // Hey, if these are not going to be used
  // by guys that don't need them, then
  // there's not really any reason to
  // initialize them!...
  //oo->canNotFallOffEdge = F;
}




/*******************
***   updateOo   ***
*******************/
// Sent the current level (with a list of OOs in it), the pos to update around, and the main CC...
// This goes through the level's list of OOs and updates them one by one.
void updateOo(LEVEL *level, int hPos, int vPos, CC *cc)
{
  OO_SHELL *oo;
  int color;
  int i;
  int j;
  int x;

  // NOTE: You should only have to check for
  // new oo creation at the tile size multiples...
  if(hPos/TILE_SIZE != level->hPosLastOoCheck)
  { level->hPosLastOoCheck = hPos/TILE_SIZE;

    // First, check the current ooground, to see
    // if any new oos need to be loaded into memory...
    x = hPos/TILE_SIZE+(SW_TILES/2)+1;
    for(i=0; i<level->hTiles; i+=1)
      if((color = getpixel(level->ooground,x,i))>0)
        for(j=0; j<MAX_OO_ON_SCREEN; j+=1)
          if(level->ooList[j]->state==INACTIVE)
          {
            // NOTE::  All of this code looks great!  ...But
            // as for right now, don't make any bad guys...
            // Just see if you can get this thing to compile!...

            startOo(level->ooList[j], level->ooData[color], x*TILE_SIZE,(i)*TILE_SIZE, L);
            level->ooList[j]->storeColor = color;
            level->ooList[j]->storeHLevelPos = x;
            level->ooList[j]->storeVLevelPos = i;
            putpixel(level->ooground,level->ooList[j]->storeHLevelPos,level->ooList[j]->storeVLevelPos,0);
            j=MAX_OO_ON_SCREEN;

          }
    x = hPos/TILE_SIZE-(SW_TILES/2);
    for(i=0; i<level->hTiles; i+=1)
      if((color = getpixel(level->ooground,x,i))>0)
        for(j=0; j<MAX_OO_ON_SCREEN; j+=1)
          if(level->ooList[j]->state==INACTIVE)
          {
            startOo(level->ooList[j], level->ooData[color], x*TILE_SIZE,(i)*TILE_SIZE, R);
            level->ooList[j]->storeColor = color;
            level->ooList[j]->storeHLevelPos = x;
            level->ooList[j]->storeVLevelPos = i;
            putpixel(level->ooground,level->ooList[j]->storeHLevelPos,level->ooList[j]->storeVLevelPos,0);
            j=MAX_OO_ON_SCREEN;

          }
  } // End last oo pos checked...

  // Go throught each active oo...
  // First, see if they should be deactivated...
  // After that's checked, send it through the AI function...
  // Then, by using state checks, determing which animation
  // to use, and check to see if it has to be reset...
  // Then, blit to the screen!  Easy, right?
  for(i=0; i<MAX_OO_ON_SCREEN; i+=1)
  {
    // First, determine if the oo should go bye bye...
    if(level->ooList[i]->state!=INACTIVE)
    { oo = level->ooList[i];
      if(oo->hPos>=hPos+(SW) || oo->hPos<=hPos-(SW) ||
          oo->vPos>=SH+(SH/2) || oo->vPos<=-(SH/2))
      { // If this oo isn't dead, and he's going
        // off the screen somewheres, don't forget to
        // put him back!...
        if(oo->state!=DYING)
          putpixel(level->ooground,oo->storeHLevelPos,oo->storeVLevelPos,oo->storeColor);
        resetOoShell(oo);  //oo->state=INACTIVE;
      }
    }

    // NOTE::  Hey, real soon, this is the place where
    // you will check what type of OO you're dealing
    // with.  Then, call the appropriate update function...

    if(level->ooList[i]->state!=INACTIVE)
    { oo = level->ooList[i];

      // ...Ready?...
      // Determine the OO type!...
      switch(oo->data->type)
      { case GND_WALKER:  updateGndWalker(oo, hPos,vPos, cc,level);  break;
        case GND_IMOBILE_TOSSER:  updateGndImobileTosser(oo, hPos,vPos, cc,level);  break;

      }
    }

  }
}



/***********************
***   resetOoShell   ***
***********************/
// Sent a pointer to an OO shell...
// This completely "zeroes" out the OO shell.
void resetOoShell(OO_SHELL *oo)
{
  oo->state = INACTIVE;
  oo->data = NULL;

  oo->anim = NULL;
  oo->animPos = 0;
  oo->animTimeMark = 0;

  oo->hVel = 0;
  oo->vVel = 0;

  oo->hPos = 0;
  oo->vPos = 0;
  oo->hFudge = 0;
  oo->vFudge = 0;

  oo->health = 0;
  oo->facing = 0;

  oo->storeColor = 0;
  oo->storeHLevelPos = 0;
  oo->storeVLevelPos = 0;
}



/**********************
***   isInContact   ***
**********************/
// Sent the coordinates to two rectangular areas...
// This checks if the two rectangles are in contact with other.
// NOTE:  I am VERY proud of this "if" statement...  ^-^
inline FLAG isInContact(int hPos1,int vPos1,int uRad1,int dRad1,int lRad1, int rRad1, int hPos2,int vPos2,int uRad2,int dRad2,int lRad2, int rRad2)
{
  // If any left edge or any right edge is inside one of the other...
  if(  ((hPos1-lRad1>hPos2-lRad2 && hPos1-lRad1<hPos2+lRad2) || (hPos1+rRad1<hPos2+rRad2 && hPos1+rRad1>hPos2-rRad2)
     || (hPos2-lRad2>hPos1-lRad1 && hPos2-lRad2<hPos1+rRad1) || (hPos2+rRad2<hPos1+rRad1 && hPos2+rRad2>hPos1-lRad1))
        // ...And any up edge or any down edge is inside one of the other...
        && ((vPos1+dRad1<vPos2+dRad2 && vPos1+dRad1>vPos2-uRad2) || (vPos2+dRad2<vPos1+dRad1 && vPos2+dRad2>vPos1-uRad1)
         || (vPos1-uRad1>vPos2-uRad2 && vPos1-uRad1<vPos2+dRad2) || (vPos2-uRad2>vPos1-uRad1 && vPos2-uRad2<vPos1+dRad1)) )
  { return(T);
  }
  return(F);
}



inline void checkOoFaceCc(OO_SHELL *oo, CC *cc)
{ // Always face the CC...
  if(cc->hPos <= oo->hPos)  oo->facing = L;
  else  oo->facing = R;
}



inline void checkOoForLedge(OO_SHELL *oo)
{ //Not falling off a ledge stuff...
  if(oo->state!=FALLING)
    if(oo->data->canNotFallOffEdge)
    { if(checkPassable(getFront(oo->hPos,oo->anim->frames[oo->animPos]->lRad,oo->anim->frames[oo->animPos]->rRad,oo->facing),oo->vPos+oo->anim->frames[oo->animPos]->dRad+1,D))
      { if(oo->facing==R)  oo->facing=L;
        else  oo->facing=R;
        oo->hVel *= -1;
      }
    }
}




inline void checkOoForWall(OO_SHELL *oo)
{
  // Hitting a wall stuff...
  // This may need to be fixed in the future,
  // to take into account the SOME part of
  // the oo's edge may not be passable...
  if(!checkPassable(getFront(oo->hPos,oo->anim->frames[oo->animPos]->lRad,oo->anim->frames[oo->animPos]->rRad,oo->facing),oo->vPos,oo->facing))
  {
    switch(oo->facing)
    { case R:
        oo->facing=L;
        oo->hVel = -(oo->data->hInitVel);
        break;
      case L:
        oo->facing=R;
        oo->hVel = (oo->data->hInitVel);
        break;
      case U:
      case D:
        // Do nothing.
        break;
    }
  }
}




inline void drawOo(OO_SHELL *oo, int hPos)
{
  // NOTE: This method is REALLY ugly...  So, just to clarify
  // (at least a little bit) every time you see the phrase
  // (oo->anim->frames[oo->animPos]->pic)
  // it simply means the current "sprite" picture of the oo...

  if(oo->state!=DYING)
  { if (oo->facing==L)  draw_sprite(sBuffer,(oo->anim->frames[oo->animPos]->pic), (oo->hPos-hPos+(SW/2))-((oo->anim->frames[oo->animPos]->pic)->w/2),oo->vPos-((oo->anim->frames[oo->animPos]->pic)->h/2));
    else  draw_sprite_h_flip(sBuffer,(oo->anim->frames[oo->animPos]->pic), (oo->hPos-hPos+(SW/2))-((oo->anim->frames[oo->animPos]->pic)->w/2),oo->vPos-((oo->anim->frames[oo->animPos]->pic)->h/2));
  }
  // If is dying, draw 'im upside down...
  else
  { if (oo->facing==L)  draw_sprite_vh_flip(sBuffer,(oo->anim->frames[oo->animPos]->pic), (oo->hPos-hPos+(SW/2))-((oo->anim->frames[oo->animPos]->pic)->w/2),oo->vPos-((oo->anim->frames[oo->animPos]->pic)->h/2));
    else  draw_sprite_v_flip(sBuffer,(oo->anim->frames[oo->animPos]->pic), (oo->hPos-hPos+(SW/2))-((oo->anim->frames[oo->animPos]->pic)->w/2),oo->vPos-((oo->anim->frames[oo->animPos]->pic)->h/2));
  }
}





inline void checkOoForCc(OO_SHELL *oo, CC *cc)
{
  // One of the last things to do, is
  // check to see if this bad dude is
  // doing hit damage to the cc...
  if(cc->state!=HURTING && !cc->isHurtInvincible)
  { if(isInContact(oo->hPos,oo->vPos,oo->anim->frames[oo->animPos]->uRad,oo->anim->frames[oo->animPos]->dRad,oo->anim->frames[oo->animPos]->lRad,oo->anim->frames[oo->animPos]->lRad,
       cc->hPos,cc->vPos,cc->anim->frames[cc->animPos]->uRad,cc->anim->frames[cc->animPos]->dRad,cc->anim->frames[cc->animPos]->lRad,cc->anim->frames[cc->animPos]->rRad))
    {
      //textprintf(sBuffer,font, 0,SH-60,TEXT_COLOR, "Chieko's getting hurt!!  =(");
      //Send the cc hurting...  =(
      ccToHurting(cc,HURT_POINTS);
     }
  }
}





void updateGndImobileTosser(OO_SHELL *oo, int hPos, int vPos, CC *cc, LEVEL *level)
{
  if(oo->state!=DYING)
  { checkOoFaceCc(oo,cc);

    oo->state = STANDING; // This was ==, but was probably supposed to be =.
    oo->anim = oo->data->standAnim;

    checkOoForCc(oo,cc);
  }
  else
  { moveKilledOo(oo);
  }

  // Animate!!  Yipee!...
  // And finally, draw the oo to the screen...
  animate(oo->anim,&(oo->animTimeMark),&(oo->animPos));
  drawOo(oo,hPos);
}





void updateGndWalker(OO_SHELL *oo, int hPos, int vPos, CC *cc, LEVEL *level)
{
  if(oo->state!=DYING)
  {
    // This is where you edit the oo state.
    // An oo's state will then determing it's
    // current anim...
    if(oo->anim!=NULL && !onSolid(oo->hPos,oo->vPos, oo->anim->frames[oo->animPos]->dRad,oo->anim->frames[oo->animPos]->lRad,oo->anim->frames[oo->animPos]->rRad))
    { if(oo->state!=FALLING)
      { oo->animPos=0;
        oo->animTimeMark=timer;
      }
      oo->state=FALLING;
    }
    else  oo->state=WALKING;

    // Set the anim, according to the oo's
    // current state...
    switch(oo->state)
    { case WALKING:
        oo->anim = oo->data->walkAnim;
        break;
      case FALLING:
        oo->anim = oo->data->fallAnim;
        break;
      case JUMPING:
      case STANDING:
      case CROUCHING:
      case ATTACKING:
      case MOVING:
      case HITTING:
      case INACTIVE:
      case FLYING:
      case BOUNCING:
      case HURTING:
      case DYING:
      case ACTIVE:
        // Do nothing.
        break;
    }

    checkOoForWall(oo);
    checkOoForLedge(oo);

    //H Velocity...
    //V Velocity...
    //Gravity...
    movePPS(&(oo->hPos),&(oo->vPos), &(oo->hFudge), oo->anim->frames[oo->animPos]->uRad,oo->anim->frames[oo->animPos]->dRad,oo->anim->frames[oo->animPos]->lRad,oo->anim->frames[oo->animPos]->rRad, &(oo->hVel), HORI);
    movePPS(&(oo->hPos),&(oo->vPos), &(oo->vFudge), oo->anim->frames[oo->animPos]->uRad,oo->anim->frames[oo->animPos]->dRad,oo->anim->frames[oo->animPos]->lRad,oo->anim->frames[oo->animPos]->rRad, &(oo->vVel), VERT);
    addGravity(oo->hPos,oo->vPos, oo->anim->frames[oo->animPos]->dRad,oo->anim->frames[oo->animPos]->lRad,oo->anim->frames[oo->animPos]->rRad, &(oo->vVel));

    checkOoForCc(oo,cc);
  }
  else
  { moveKilledOo(oo);
  }

  animate(oo->anim,&(oo->animTimeMark),&(oo->animPos));
  drawOo(oo,hPos);
}




inline void moveKilledOo(OO_SHELL *oo)
{  //Yipee!  This bad boy's goin' DOOOWN!!...
  oo->vVel += GRAVITY;
  movePPSSansCollision(&(oo->hPos),&(oo->vPos), &(oo->hFudge), &(oo->hVel), HORI);
  movePPSSansCollision(&(oo->hPos),&(oo->vPos), &(oo->vFudge), &(oo->vVel), VERT);
}




inline int getFront(int hPos, int lRad, int rRad, DIRECTION dir)
{
  switch(dir)
  { case L:
      return(hPos-lRad-1);
      break;
    case R:
      return(hPos+rRad+1);
      break;
    case U:
    case D:
      // Do nothing.
      break;
  }
  // If all else fails, just return the "R" answer.
  return(hPos+rRad+1);
}







/******************
***   onSolid   ***
******************/
// Send a few integers, describing the object's position and size...
// This checks to see if the object is directly on a solid tile.
// Returns a true flag if on a solid tile...
inline FLAG onSolid(int hPos, int vPos, int dRad, int lRad, int rRad)
{ int i;
  FLAG checkOk;

  // Check the bottom left of the object,
  // and all along the bottom...
  for(checkOk=T,i=hPos-lRad; i<hPos+rRad; i+=TILE_SIZE)  //check the perpendicular axis too!
  { if(!checkPassable(i,vPos+(dRad)+1,D))                //check to see if passable at that pos...
      return(T);
  }
  // Then quickly check the far bottom
  // right corner...
  if(!checkPassable(hPos+rRad,vPos+(dRad)+1,D))
    return(T);
  return(F);
}





/*********************
***   addGravity   ***
*********************/
// Send alll the sizes and velocities needed for a gravity change...
// This pulls the object down, unless it is on solid ground.
inline void addGravity(int hPos,int vPos, int dRad,int lRad, int rRad, int *pps)
{ int i;
  FLAG checkOk;

  // Check the bottom left corner,
  // and the bottome edge...
  for(checkOk=T,i=hPos-lRad; i<hPos+rRad; i+=TILE_SIZE)      //check the perpendicular axis too!
  { if(!checkPassable(i,vPos+(dRad)+1,D))            //check to see if passable at that pos...
    { checkOk=F;
      i=hPos+rRad;
      *pps=0;
    }
  }
  // Then, quickly check the bottom right
  // corner...
  if(!checkPassable(hPos+rRad,vPos+(dRad)+1,D))
    checkOk=F;

  // If you CAN go down, then go down!
  if(checkOk)
  { *pps+=GRAVITY;
  }
}




/******************
***   movePPS   ***
******************/
// Sent the pos pointers, the hit radius, the speed in PPS, and the axis direction...
// This moves adjusts the sent positions, using the sent velocity and the sent direction (axis)...
void movePPS(int *hPos,int *vPos, float *fudge, int uRad,int dRad,int lRad, int rRad, int *pps, AXIS axis)
{ int change;
  int bin;
  int hCheck;
  int vCheck;
  int i;
  int j;
  DIRECTION dir;
  FLAG checkOk;
  change = *pps/GAME_TICKER;


  *fudge += (float)((float)(*pps)/(float)GAME_TICKER - *pps/GAME_TICKER);
  while(*fudge >= 1)
  { change += 1;
    *fudge -= 1;
  }
  while(*fudge <= -1)
  { change -= 1;
    *fudge += 1;
  }

  //textprintf(sBuffer,font, 0,30,TEXT_COLOR, "change=%d",change);
  if(change)                                        //if you HAVE to move...
  { bin = abs(change) / change;                     //this will either be a 1 or -1...
    if(axis == HORI)
    { if(change>0)  dir=R;
      else  dir=L;

      if(bin>0)  hCheck=rRad;
      else  hCheck=lRad;

      for(i=0; i<abs(change); i+=1)                          //start moving those changes!
      {
        // Start at the top corner, and
        // check down along that edge...
        for(checkOk=T,j=*vPos-uRad; j<*vPos+dRad; j+=TILE_SIZE)  //check the perpendicular axis too!
        { if(!checkPassable(*hPos+(hCheck*bin)+bin,j,dir))         //check to see if passable at that pos...
          { checkOk=F;
            i=abs(change);
            j=*vPos+dRad;
            *pps=0;
          }
        }
        // Then, quickly check the bottom corner...
        if(!checkPassable(*hPos+(hCheck*bin)+bin,*vPos+dRad,dir))
        { checkOk=F;
          i=abs(change);
          *pps=0;
        }
        // If all is ok, then move it!...
        if(checkOk)
          *hPos+=bin;
      }
    } //End if HORI
    else
    { if(change>0)  dir=D;
      else  dir=U;

      if(bin>0)  vCheck=dRad;
      else  vCheck=uRad;

      for(i=0; i<abs(change); i+=1)                 //start moving those changes!
      {
        // First, check the left corner, and
        // all along that edge...
        for(checkOk=T,j=*hPos-lRad; j<*hPos+rRad; j+=TILE_SIZE)     //check the perpendicular axis too!
        { if(!checkPassable(j,*vPos+(vCheck*bin)+bin,dir))          //check to see if passable at that pos...
          { checkOk=F;
            i=abs(change);
            j=*hPos+rRad;
            *pps=0;
          }
        }
        // Then, quickly check the right corner...
        if(!checkPassable(*hPos+rRad,*vPos+(vCheck*bin)+bin,dir))
        { checkOk=F;
          i=abs(change);
          *pps=0;
        }
        // If it's all good, then move it!...
        if(checkOk)
        { *vPos+=bin;
        }
      }
    } //End if VERT
  } //End if(change)...

}





/*******************************
***   movePPSSansCollision   ***
*******************************/
// Sent the pos pointers and speed in PPS, and the axis direction...
// This moves adjusts the sent positions.
void movePPSSansCollision(int *hPos,int *vPos, float *fudge, int *pps, AXIS axis)
{ int change = *pps/GAME_TICKER;
  *fudge += (float)((float)(*pps)/(float)GAME_TICKER - *pps/GAME_TICKER);
  while(*fudge >= 1)
  { change += 1;
    *fudge -= 1;
  }
  while(*fudge <= -1)
  { change -= 1;
    *fudge += 1;
  }

  if(change)
  { if(axis == HORI)
      *hPos+=change;
    else
      *vPos+=change;
  } //End if(change)...

}





/************************
***   checkPassable   ***
************************/
// Send the position of the object in the level...
// This will determine if the specified position is passable...
// Returns true if passable...
inline FLAG checkPassable(int hPos, int vPos, DIRECTION dir)
{ int color;
  if(vPos<0)
    return(T);
  color = getpixel(level->walkground, hPos/TILE_SIZE,vPos/TILE_SIZE);
  //textprintf(sBuffer,font, 0,40,TEXT_COLOR, "color checked: %d",color);
  if(hPos>=0)
  { if(color==0)
      return(T);
    if(color>0)
    { if(level->tiles[color]->passability==SOLID)
        return(F);
      if(level->tiles[color]->passability==PASS_FROM_BOTTOM && dir==D && vPos%TILE_SIZE==0)
        return(F);
      return(T);
    }
  }
  return(F);
}



/***********************
***   animateTiles   ***
***********************/
// Sent a pointer to a list of tile pointers...
// This animates all of the current tiles in the sent list.
inline void animateTiles(TILE **tiles)
{ int i;

  // NOTE: Tiles start at space 0...
  for(i=1; i<MAX_TILES; i+=1)
  { if(tiles[i]!=NULL && tiles[i]->size>1)
    {
      if(checkTime(timer,tiles[i]->animTimeMark) >= tiles[i]->speed)
      { if(tiles[i]->animPos >= tiles[i]->size-1)
        { tiles[i]->animPos=0;
          tiles[i]->animTimeMark = timer;
        }
        else
        { tiles[i]->animPos+=1;
          tiles[i]->animTimeMark = timer;
        }
      }

    }
  }
}




/***************************
***   updateWalkground   ***
***************************/
// Sent a pointer to the current level, and the CC's h pos...
// This draws the tiles of the walkground to the screen.
inline void updateWalkground(LEVEL *level, int hPos)
{
  int color;
  int x;
  int y;

  for(y=0; y<SH_TILES; y+=1)
  { for(x=0; x<SW_TILES+1; x+=1)
    { if((color = getpixel(level->walkground,((hPos/*-camera*/)/TILE_SIZE)-(SW_TILES/2)+x,y)) > 0)
      { masked_blit(level->tiles[color]->pics[level->tiles[color]->animPos],sBuffer, 0,0, x*TILE_SIZE-(hPos%TILE_SIZE)/*+(camera%TILE_SIZE)*/,y*TILE_SIZE, TILE_SIZE,TILE_SIZE);
      }
    }
  }
}



/***********************
***   destroyLevel   ***
***********************/
// Send a pointer to a level...
// This removes a level from memory.
void destroyLevel(LEVEL *level)
{
  int i;

  for(i=0; i<MAX_OO; i+=1)
    if(level->ooData[i]!=NULL)
      destroyOoData(level->ooData[i]);

  for(i=0; i<MAX_OO_ON_SCREEN; i+=1)
    if(level->ooList[i]!=NULL)
      free(level->ooList[i]);

  for(i=0; i<MAX_TILES; i+=1)
    if(level->tiles[i]!=NULL)
      destroyTile(level->tiles[i]);

  for(i=0; i<MAX_ITEMS; i+=1)
    if(level->items[i]!=NULL)
      destroyItem(level->items[i]);

  destroy_bitmap(level->farground);
  destroy_bitmap(level->scrollground);
  destroy_bitmap(level->walkground);
  destroy_bitmap(level->ooground);
  destroy_bitmap(level->itemground);

  free(level);
}




void destroyItem(ITEM *item)
{
  int i;
  for(i=0; i<item->size; i+=1)
    destroy_bitmap(item->pics[i]);
  free(item->pics);
  free(item);
}




void destroyOoData(OO_DATA *oo)
{
  //printf("destroying an OO of type %d \n",oo->type);  readkey();

  if(oo->walkAnim!=NULL)   destroyAnim(oo->walkAnim);
  if(oo->standAnim!=NULL)  destroyAnim(oo->standAnim);
  if(oo->jumpAnim!=NULL)   destroyAnim(oo->jumpAnim);
  if(oo->flyAnim!=NULL)    destroyAnim(oo->flyAnim);
  if(oo->fallAnim!=NULL)   destroyAnim(oo->fallAnim);
  if(oo->dieAnim!=NULL)    destroyAnim(oo->dieAnim);

  free(oo);
}





/**********************
***   destroyTile   ***
**********************/
// Sent a pointer to a tile...
// This removes a tile from memory.
void destroyTile(TILE *tile)
{  
  int i;
  for(i=0; i<tile->size; i+=1)
    destroy_bitmap(tile->pics[i]);
  free(tile->pics);
  free(tile);
}




/*****************************
***   updateScrollground   ***
*****************************/
// Sent a pointer to the current level, and the hPos of the main CC...
// This updates (scrolls) the scrollground with reference to the CC.
inline void updateScrollground(LEVEL *level, int hPos)
{
  // If the main cc has moved to the right...
  // ...And the cc moved far enough to scroll the screen...
  // ...Then scroll the screen however many pixels...
  // ...And resave the current position of the main cc!...
  if(hPos > level->hScrollMark)
  { if(hPos - level->hScrollMark >= level->hScrollChange)
    { level->hScrollPos += (hPos - level->hScrollMark) / level->hScrollChange;
      level->hScrollMark = hPos;
    }
  }

  // Now do the opposite, in case the
  // main cc actually moves left...  =)
  if(hPos < level->hScrollMark)
  { if(level->hScrollMark - hPos >= level->hScrollChange)
    { level->hScrollPos -= (level->hScrollMark - hPos) / level->hScrollChange;
      level->hScrollMark = hPos;
    }
  }

  // This part checks to see if the scrollground
  // has scrolled itself off the screen!  If its
  // current position is bigger than itself, set
  // it back near 0.  If its position is less than
  // 0, set back to close to its size.
  if(level->hScrollPos >= level->scrollground->w)
    level->hScrollPos -= level->scrollground->w;
  else if(level->hScrollPos < 0)
    level->hScrollPos += level->scrollground->w;

  // Blit to the screen buffer.  Keep in mind that if
  // the scrollground doesn't fill the entire screen,
  // it has to start repeating itself.
  masked_blit(level->scrollground,sBuffer, level->hScrollPos,0, 0,0, SW,SH);
  if(level->hScrollPos > level->scrollground->w-SW)
    masked_blit(level->scrollground,sBuffer, 0,0, level->scrollground->w-level->hScrollPos,0, SW,SH);
}


/*************************
***  initializeLevel   ***
*************************/
// Sent a file name string for the level data file...
// This loads a new level into memory.
// Returns a pointer to the new level...
LEVEL * initializeLevel(char *fileName)
{ FILE *levelFile;
  LEVEL *newLevel;
  char tempString[40];
  BITMAP *tempBmp;
  newLevel = (LEVEL *)malloc(sizeof(LEVEL));
  levelFile = fopen(fileName, "r");

  fscanf(levelFile,"%d",&(newLevel->hInitPos));
  fscanf(levelFile,"%d",&(newLevel->vInitPos));

  fscanf(levelFile,"%s",tempString);
  initializeOoData(newLevel->ooData,tempString);
  initializeOoList(newLevel->ooList);
  newLevel->hPosLastOoCheck = 0;

  fscanf(levelFile,"%s",tempString);
  initializeTileList(newLevel->tiles,tempString);
  fscanf(levelFile,"%s",tempString);
  initializeItemList(newLevel->items,tempString);

  fscanf(levelFile,"%s",tempString);
  newLevel->farground = load_bitmap(tempString,gamePallete);
  if(lowGame)
  { tempBmp = newLevel->farground;
    newLevel->farground = create_bitmap(tempBmp->w/2,tempBmp->h/2);
    stretch_blit(tempBmp,newLevel->farground, 0,0, tempBmp->w,tempBmp->h, 0,0, newLevel->farground->w,newLevel->farground->h);
    destroy_bitmap(tempBmp);
  }

  fscanf(levelFile,"%s",tempString);
  newLevel->scrollground = load_bitmap(tempString,gamePallete);
  if(lowGame)
  { tempBmp = newLevel->scrollground;
    newLevel->scrollground = create_bitmap(tempBmp->w/2,tempBmp->h/2);
    stretch_blit(tempBmp,newLevel->scrollground, 0,0, tempBmp->w,tempBmp->h, 0,0, newLevel->scrollground->w,newLevel->scrollground->h);
    destroy_bitmap(tempBmp);
  }
  if(!useScrollground)
  { masked_blit(newLevel->scrollground,newLevel->farground, 0,0, 0,0, SW,SH);
    destroy_bitmap(newLevel->scrollground);
    newLevel->scrollground = create_bitmap(20,20);
  }
  newLevel->hScrollChange=3;
  newLevel->hScrollPos=0;
  newLevel->hScrollMark = newLevel->hInitPos;

  fscanf(levelFile,"%s",tempString);
  newLevel->walkground = load_bitmap(tempString,gamePallete);
  newLevel->wTiles = newLevel->walkground->w;
  newLevel->hTiles = newLevel->walkground->h;
  fscanf(levelFile,"%s",tempString);
  newLevel->ooground = load_bitmap(tempString,gamePallete);
  fscanf(levelFile,"%s",tempString);
  newLevel->itemground = load_bitmap(tempString,gamePallete);


  fclose(levelFile);
  return(newLevel);
}





void initializeOoList(OO_SHELL **list)
{ int i;

  for(i=0; i<MAX_OO_ON_SCREEN; i+=1)
  { list[i] = (OO_SHELL *)malloc(sizeof(OO_SHELL));
    resetOoShell(list[i]);
  }
}



void initializeItemList(ITEM **items, char *fileName)
{
  FILE *itemFile;
  int i;

  itemFile = fopen(fileName,"r");

  // First, clean up the list...
  for(i=0; i<=MAX_ITEMS; i+=1)
    items[i] = NULL;

  for(i=1; i<=MAX_ITEMS; i+=1)
  { // Soon to be implemented!...
    //item[i] = initializeItem(itemFile);
  }

  fclose(itemFile);
}




/******************
***   startOo   ***
******************/
// Sent the OO pointer to initialize, the appropriate OO pointer data, where, to put the OO, and going which direction...
// This initializes the OO with the sent OO data, and the sent place.
void startOo(OO_SHELL *ooShell, OO_DATA *ooData, int sentHPos, int sentVPos, DIRECTION fromDir)
{
  ooShell->state = ACTIVE;
  ooShell->data = ooData;

  if(ooData->walkAnim)  ooShell->anim = ooData->walkAnim;
  else if(ooData->standAnim)  ooShell->anim = ooData->standAnim;
  else if(ooData->jumpAnim)  ooShell->anim = ooData->jumpAnim;
  else if(ooData->flyAnim)  ooShell->anim = ooData->flyAnim;
  else if(ooData->fallAnim)  ooShell->anim = ooData->fallAnim;
  else if(ooData->dieAnim)  ooShell->anim = ooData->dieAnim;
  else  ooShell->anim = NULL;

  ooShell->animPos = 0;
  ooShell->animTimeMark = timer;

  if(fromDir==L)
  { ooShell->hVel = -(ooShell->data->hInitVel);
    ooShell->facing = L;
  }
  else
  { ooShell->hVel = ooShell->data->hInitVel;
    ooShell->facing = R;
  }
  ooShell->vVel = ooShell->data->vInitVel;
  ooShell->health = ooShell->data->initHealth;

  // Hey.
  // Set this position to take OO radiuses
  // into account...
  ooShell->hPos = sentHPos;
  ooShell->vPos = sentVPos;
  ooShell->hFudge = 0;
  ooShell->vFudge = 0;

  // There!
  // Now this OO is all ready to go!
  // Go kick some butt, buddy!
  // (don't tell him that the bad guys always lose...)  ^-^
}




/***************************
***   initializeOoData   ***
***************************/
// Sent the name of the file to load OO data file names from...
// This loads a list of OO data for a level to use...
// Returns the new OO data list...
void initializeOoData(OO_DATA **data, char *fileName)
{ FILE *file;
  FILE *ooDataFile;
  char tempString[120];
  OO_TYPE type;
  int i;
  //OO_DATA **newOoDataList;
  //OO_PARENT parent;
  //OO_CHILD child;

  file = fopen(fileName, "r");
  //data = (OO_DATA **)calloc(MAX_NUM_OF_OO, sizeof(OO_DATA *));

  // First, clean the oo data list...
  for(i=0;i<MAX_OO;i+=1)
    data[i]=NULL;

  for(i=1; i<MAX_OO && (fscanf(file,"%s",tempString)!=EOF); i+=1)
  { ooDataFile = fopen(tempString, "r");
    data[i] = (OO_DATA *)malloc(sizeof(OO_DATA));

    // NOTE::  Here!  Don't just "initialize" an OO!
    // Instead, determine what type, and call the
    // corresponding initialization function!
    // One, two, easy as pie!...

    // Clean up!...
    resetOoData(data[i]);

    // The first 2 lines of every OO file
    // should be it's parent and child
    // types...
    //fscanf(ooDataFile,"%s",tempString);  parent = toOoParent(tempString);
    //fscanf(ooDataFile,"%s",tempString);  child = toOoChild(tempString);
    fscanf(ooDataFile,"%s",tempString);  type = toOoType(tempString);

    // This, is the "ever growing" list of
    // OO type loading...
    switch(type)
    { case GND_WALKER:  makeGndWalker(data[i],ooDataFile);  break;
      case GND_IMOBILE_TOSSER:  makeGndImobileTosser(data[i],ooDataFile);  break;

    }

    fclose(ooDataFile);
  }

  fclose(file);
}




/******************
***   swipeln   ***
******************/
// Sent a pointer to a file...
// This sweet little function just scans a "useless" line out of a file.
// NOTE::  This is for taking comments out of data files...
int swipeln(FILE *file)
{ char tempString[80];
  return(fscanf(file,"%s",tempString));
}



inline void lowGameAdjust(int *num)
{ if(lowGame)
    *num = *num/2;
}




OO_DATA * makeGndWalker(OO_DATA *oo, FILE *file)
{
  oo->type = GND_WALKER;
  //oo->parent = GND;
  //oo->child = WALKER;

  swipeln(file);  fscanf(file,"%d",&(oo->initHealth));

  swipeln(file);  fscanf(file,"%d",&(oo->hInitVel));
  swipeln(file);  fscanf(file,"%d",&(oo->vInitVel));
  lowGameAdjust(&(oo->hInitVel));
  lowGameAdjust(&(oo->vInitVel));

  swipeln(file);  fscanf(file,"%d",(int *)&(oo->canNotFallOffEdge));

  swipeln(file);  oo->walkAnim = initializeAnim(file);
  swipeln(file);  oo->fallAnim = initializeAnim(file);
  swipeln(file);  oo->dieAnim = initializeAnim(file);
  
  return oo;
}



OO_DATA * makeGndImobileTosser(OO_DATA *oo, FILE *file)
{
  // NOTE::  An imobile tosser will throw the next
  // registered oo for the level.  (maybe this will
  // be a standard)...
  oo->type = GND_IMOBILE_TOSSER;
  //oo->parent = GND;
  //oo->child = IMOBILE_TOSSER;

  swipeln(file);  fscanf(file,"%d",&(oo->initHealth));

  swipeln(file);  fscanf(file,"%d",(int *)&(oo->aims));

  swipeln(file);  oo->standAnim = initializeAnim(file);
  swipeln(file);  oo->dieAnim = initializeAnim(file);
  
  return oo;
}





OO_TYPE toOoType(char *str)
{ if(!strcmp("GND_WALKER",str))                  return(GND_WALKER);
  else if(!strcmp("GND_IMOBILE_TOSSER",str))     return(GND_IMOBILE_TOSSER);
  else  return(0);

}





/**************************
***   initializeTiles   ***
**************************/
// Sent the file name of all the tiles to load for a level...
// This loads a whole list of tiles into memory.
// Returns a list of tiles...
void initializeTileList(TILE **tiles, char *fileName)
{ FILE *file;
  char tempString[80];
  int tileNum;
  int i;
  BITMAP *tempBmp;

  // First, clean the tile list...
  for(i=0;i<MAX_TILES;i+=1)
    tiles[i] = NULL;

  file = fopen(fileName, "r");
  //fscanf(tileFile,"%d",&numOfTiles);
  //printf("num of tiles %d \n",numOfTiles);

  //for(i=0; i<numOfTiles; i+=1)
  while(swipeln(file)!=EOF)
  { //swipeln(tileFile);     //Catch the === tile header...

    // Load this tile's reference number for this level...
    fscanf(file,"%d",&tileNum);
    tiles[tileNum] = (TILE *)malloc(sizeof(TILE));

    //load frames of animation...
    fscanf(file,"%d",&(tiles[tileNum]->size));
    tiles[tileNum]->pics = (BITMAP **)calloc(tiles[tileNum]->size, sizeof(BITMAP *));
    for(i=0; i<tiles[tileNum]->size; i+=1)
    { fscanf(file,"%s",tempString);
      tiles[tileNum]->pics[i] = load_bitmap(tempString,gamePallete);
      if(lowGame)
      { tempBmp = tiles[tileNum]->pics[i];
        tiles[tileNum]->pics[i] = create_bitmap(tempBmp->w/2,tempBmp->h/2);
        stretch_blit(tempBmp,tiles[tileNum]->pics[i], 0,0, tempBmp->w,tempBmp->h, 0,0, tiles[tileNum]->pics[i]->w,tiles[tileNum]->pics[i]->h);
        destroy_bitmap(tempBmp);
      }
    }

    fscanf(file,"%d",&(tiles[tileNum]->speed));
    fscanf(file,"%d",(int *)&(tiles[tileNum]->passability));

    tiles[tileNum]->animPos=0;
    tiles[tileNum]->animTimeMark=timer;

    //printf("tile init num %d  passability %d \n",tileNum,newTiles[tileNum]->passability);
    //blit(newTiles[tileNum]->pics[0],screen, 0,0, 0,0, TILE_SIZE,TILE_SIZE);
    //readkey();
  }

  fclose(file);
}



/********************
***   destroyCc   ***
********************/
// Sent a pointer to a CC...
// This deallocates memory from a CC...
void destroyCc(CC *cc)
{
  int i;

  for(i=0; i<MAX_CC_ATTACKS; i+=1)
    if(cc->attackData[i]!=NULL)
      destroyAttackData(cc->attackData[i]);
  for(i=0; i<MAX_CC_ATTACKS_ON_SCREEN; i+=1)
    free(cc->attackList[i]);

  destroyAnim(cc->walkAnim);
  destroyAnim(cc->standAnim);
  destroyAnim(cc->attackAnim);
  destroyAnim(cc->jumpAnim);
  destroyAnim(cc->crouchAnim);
  destroyAnim(cc->hurtAnim);
  destroyAnim(cc->jumpAndAtkAnim);
  destroyAnim(cc->crouchAndAtkAnim);

  free(cc);
}




/****************************
***   destroyAttackData   ***
****************************/
// Send a pointer to an attack data...
// This removes attack data from memory.
void destroyAttackData(ATTACK_DATA *atkData)
{ destroyAnim(atkData->moveAnim);
  destroyAnim(atkData->hitAnim);
  free(atkData);
}



/**********************
***   destroyAnim   ***
**********************/
// Sent an animation pointer...
// This deallocates memory from an animation.
void destroyAnim(ANIM *anim)
{ int i;
  for(i=0; i<anim->size; i+=1)
    destroyHitPic(anim->frames[i]);
  free(anim->frames);
  free(anim);
}




void destroyHitPic(HIT_PIC *hitPic)
{
  destroy_bitmap(hitPic->pic);
  free(hitPic);
}





void ccToHurting(CC *cc, int hurtPoints)
{
  cc->state=HURTING;
  cc->health-=HURT_POINTS;
  cc->isHurtInvincible=T;

  if(cc->facing==R)  cc->hVel = -(cc->hHurtVel);
  else               cc->hVel = cc->hHurtVel;
  cc->vVel = cc->vHurtVel;

  // Reset the animation...
  cc->anim = cc->hurtAnim;
  cc->animTimeMark=timer;
  cc->animPos=0;

  // Start that blinking stuff...
  cc->hurtTimeMark = timer;
  cc->hurtBlinkTimeMark = timer;

  //Just nudge the cc up into the air...
  cc->vPos-=1;

}



void ccToCrouching(CC *cc)
{
  cc->state = CROUCHING;

  cc->anim = cc->crouchAnim;
  cc->animTimeMark = timer;
  cc->animPos = 0;

}



void ccToAttacking(CC *cc)
{
  switch(cc->state)
  {
    case WALKING:
    case STANDING:
      cc->anim = cc->attackAnim;
      cc->state = ATTACKING;
      cc->animTimeMark = timer;
      cc->animPos = 0;
      startCcAttack(cc,cc->hPos,cc->vPos+cc->vAttackOffset);
      break;

    case CROUCHING:
      cc->anim = cc->crouchAndAtkAnim;
      cc->state = ATTACKING;
      cc->animTimeMark = timer;
      cc->animPos = 0;
      startCcAttack(cc,cc->hPos,cc->vPos+cc->vCrouchAttackOffset);
      break;

    case JUMPING:
      cc->anim = cc->jumpAndAtkAnim;
      cc->state = ATTACKING;
      cc->animTimeMark = timer;
      cc->animPos = 0;
      startCcAttack(cc,cc->hPos,cc->vPos+cc->vJumpAttackOffset);
      break;

    case ATTACKING:
    case MOVING:
    case HITTING:
    case INACTIVE:
    case FALLING:
    case FLYING:
    case BOUNCING:
    case HURTING:
    case DYING:
    case ACTIVE:
      // Do nothing.
      break;
  }
}


void ccToStanding(CC *cc)
{
  cc->state = STANDING;

  cc->anim = cc->standAnim;
  cc->animTimeMark = timer;
  cc->animPos = 0;

}


void ccToWalking(CC *cc)
{
  cc->state = WALKING;

  cc->anim = cc->walkAnim;
  cc->animTimeMark = timer;
  if(cc->facing==R)  cc->animPos = 3;
  else  cc->animPos = 1;

}


void ccToJumping(CC *cc)
{
  cc->state = JUMPING;

  cc->anim = cc->jumpAnim;
  cc->animTimeMark = timer;
  cc->animPos = 0;

  play_sample(jumpEffect, 200,140,1000, 0);

  if(onSolid(cc->hPos,cc->vPos, cc->anim->frames[cc->animPos]->dRad,cc->anim->frames[cc->animPos]->lRad,cc->anim->frames[cc->animPos]->rRad))
    cc->vVel = cc->jumpVel;

}



inline void startCcAttack(CC *cc, int sentHPos, int sentVPos)
{ int i;

  //start a new attack...
  for(i=0; i<MAX_CC_ATTACKS_ON_SCREEN; i+=1)
    if(cc->attackList[i]->state==INACTIVE)
    { //startAttack(cc->attackList[i], cc->hPos,cc->vPos+(-25), cc->facing);
      //startAttack(ATTACK_SHELL *atkShell, int sentHPos, int sentVPos, DIRECTION direction)

      cc->attackList[i]->state = MOVING;
      cc->attackList[i]->animTimeMark=timer;

      cc->attackList[i]->hPos = sentHPos;
      cc->attackList[i]->vPos = sentVPos;

      if(key[KEY_UP])
      { cc->attackList[i]->hVel = cc->attackList[i]->data->hInitUVel;
        cc->attackList[i]->vVel = cc->attackList[i]->data->vInitUVel;
      }
      else if(key[KEY_DOWN])
      { cc->attackList[i]->hVel = cc->attackList[i]->data->hInitDVel;
        cc->attackList[i]->vVel = cc->attackList[i]->data->vInitDVel;
      }
      else
      { cc->attackList[i]->hVel = cc->attackList[i]->data->hInitVel;
        cc->attackList[i]->vVel = cc->attackList[i]->data->vInitVel;
      }

      if(cc->facing==L)  cc->attackList[i]->hVel *= -1;

      cc->attackList[i]->vFudge = 0;
      cc->attackList[i]->hFudge = 0;
      cc->attackList[i]->facing = cc->facing;

      i=MAX_CC_ATTACKS_ON_SCREEN;
    }
}




void doCcCrouching(CC *cc)
{
  if(key[KEY_RIGHT])  cc->facing = R;
  else if(key[KEY_LEFT])  cc->facing = L;

  addCcFriction(cc);

  if(key[KEY_Z] || key[KEY_COLON])  ccToJumping(cc);
  else if(key[KEY_C] || key[KEY_J])  ccToAttacking(cc);
  else if(!key[KEY_DOWN])  ccToStanding(cc);

}


void doCcAttacking(CC *cc)
{
  if(!onSolid(cc->hPos,cc->vPos, cc->anim->frames[cc->animPos]->dRad,cc->anim->frames[cc->animPos]->lRad,cc->anim->frames[cc->animPos]->rRad))
  { if(key[KEY_RIGHT])
    { cc->facing = R;
      cc->hVel+=(cc->hChangeVel*2);
      if(cc->hVel > (cc->maxWalkVel+cc->hChangeVel))
        cc->hVel = (cc->maxWalkVel+cc->hChangeVel);
      if(cc->hVel==0)  cc->hVel+=1;
    }
    else if(key[KEY_LEFT])
    { cc->facing = L;
      cc->hVel-=(cc->maxWalkVel*2);
      if(cc->hVel < -(cc->maxWalkVel+cc->hChangeVel))
        cc->hVel = -(cc->maxWalkVel+cc->hChangeVel);
      if(cc->hVel==0)  cc->hVel+=1;
    }
  }
  addCcFriction(cc);

  if(cc->animPos == cc->anim->size-1 && checkTime(timer,cc->animTimeMark)>cc->anim->speed)
  {
    //if(key[KEY_C])  ccToAttacking(cc);

    if(onSolid(cc->hPos,cc->vPos, cc->anim->frames[cc->animPos]->dRad,cc->anim->frames[cc->animPos]->lRad,cc->anim->frames[cc->animPos]->rRad))
    {
      if(key[KEY_Z] || key[KEY_COLON])  ccToJumping(cc);
      else if(key[KEY_DOWN])  ccToCrouching(cc);
      else  ccToStanding(cc);
    }
    else
      ccToJumping(cc);
  }

}


void doCcStanding(CC *cc)
{
  if(key[KEY_RIGHT])
  { cc->facing = R;
    ccToWalking(cc);
  }
  else if(key[KEY_LEFT])
  { cc->facing = L;
    ccToWalking(cc);
  }
  else
    addCcFriction(cc);

  if(key[KEY_DOWN])  ccToCrouching(cc);
  else if(key[KEY_Z] || key[KEY_COLON])  ccToJumping(cc);
  else if(key[KEY_C] || key[KEY_J])  ccToAttacking(cc);

}

void doCcHurting(CC *cc)
{
  addCcFriction(cc);

  if(onSolid(cc->hPos,cc->vPos, cc->anim->frames[cc->animPos]->dRad,cc->anim->frames[cc->animPos]->lRad,cc->anim->frames[cc->animPos]->rRad))
  {
    if(key[KEY_RIGHT])  cc->facing = R;
    else if(key[KEY_LEFT])  cc->facing = L;

    if(key[KEY_C] || key[KEY_J])  ccToAttacking(cc);
    else if(key[KEY_DOWN])  ccToCrouching(cc);
    else if(key[KEY_Z] || key[KEY_COLON])  ccToJumping(cc);
    else ccToStanding(cc);
  }

}


void doCcWalking(CC *cc)
{
  if(key[KEY_RIGHT])
  { cc->facing = R;
    cc->hVel+=cc->hChangeVel;
    if(cc->hVel > cc->maxWalkVel)
      cc->hVel = cc->maxWalkVel;
    if(cc->hVel==0)  cc->hVel+=1;
  }
  else if(key[KEY_LEFT])
  { cc->facing = L;
    cc->hVel-=cc->hChangeVel;
    if(cc->hVel < -cc->maxWalkVel)
      cc->hVel = -cc->maxWalkVel;
    if(cc->hVel==0)  cc->hVel+=1;
  }
  else
    addCcFriction(cc);

  if(!onSolid(cc->hPos,cc->vPos, cc->anim->frames[cc->animPos]->dRad,cc->anim->frames[cc->animPos]->lRad,cc->anim->frames[cc->animPos]->rRad))
    ccToJumping(cc);

  if(cc->hVel==0)  ccToStanding(cc);

  if(key[KEY_C] || key[KEY_J])  ccToAttacking(cc);
  else if(key[KEY_DOWN])  ccToCrouching(cc);
  else if(key[KEY_Z] || key[KEY_COLON])  ccToJumping(cc);

}


void doCcJumping(CC *cc)
{
  if(key[KEY_RIGHT])
  { cc->facing = R;
    cc->hVel+=(cc->hChangeVel*2);
    if(cc->hVel > (cc->maxWalkVel+cc->hChangeVel))
      cc->hVel = (cc->maxWalkVel+cc->hChangeVel);
    if(cc->hVel==0)  cc->hVel+=1;
  }
  else if(key[KEY_LEFT])
  { cc->facing = L;
    cc->hVel-=(cc->maxWalkVel*2);
    if(cc->hVel < -(cc->maxWalkVel+cc->hChangeVel))
      cc->hVel = -(cc->maxWalkVel+cc->hChangeVel);
    if(cc->hVel==0)  cc->hVel+=1;
  }
  else
    addCcFriction(cc);

  if(key[KEY_C] || key[KEY_J])  ccToAttacking(cc);

  if(onSolid(cc->hPos,cc->vPos, cc->anim->frames[cc->animPos]->dRad,cc->anim->frames[cc->animPos]->lRad,cc->anim->frames[cc->animPos]->rRad))
  {
    if(key[KEY_DOWN])  ccToCrouching(cc);
    else if(key[KEY_C] || key[KEY_J])  ccToAttacking(cc);
    else  ccToStanding(cc);
  }
}




inline void addCcFriction(CC *cc)
{
  if(cc->hVel > cc->hChangeVel)
    cc->hVel-=cc->hChangeVel;
  else if(cc->hVel < -cc->hChangeVel)
    cc->hVel+=cc->hChangeVel;
  else cc->hVel=0;
}






/*******************
***   updateCc   ***
*******************/
// Sent a pointer to the CC to be updated...
// This checks for keyboard input, moves CC accordingly, and draws it to the screen.
void updateCc(CC *cc)
{ //ANIM *anim;
  //char stateString[15];
  BITMAP *sprite;

  //stateToString(cc->state,stateString);
  //textprintf(sBuffer,font, 0,SH-20,TEXT_COLOR, "state %s",stateString);
  //textprintf(sBuffer,font, 0,SH-30,TEXT_COLOR, "hVel %d  vVel %d",cc->hVel,cc->vVel);


  switch(cc->state)
  {
    case HURTING:
      doCcHurting(cc);  break;
    case WALKING:
      doCcWalking(cc);  break;
    case CROUCHING:
      doCcCrouching(cc);  break;
    case JUMPING:
      doCcJumping(cc);  break;
    case STANDING:
      doCcStanding(cc);  break;
    case ATTACKING:
      doCcAttacking(cc);  break;
    default:
      printf("CC accidentally in state %d \n",cc->state);  readkey();  break;

  }


  /*
  //Hurting has been set...  =(
  if(cc->state==HURTING)
  { cc->anim=cc->hurtAnim;
    if(onSolid(cc->hPos,cc->vPos, cc->anim->frames[cc->animPos]->dRad,cc->anim->frames[cc->animPos]->lRad,cc->anim->frames[cc->animPos]->rRad))
      cc->state=STANDING;
  }
  //Key right...
  if(key[KEY_RIGHT] && cc->state!=HURTING)
  { if(!key[KEY_DOWN]) //&& (cc->state!=ATTACKING && onSolid(cc->hPos,cc->vPos, cc->anim->frames[cc->animPos]->dRad,cc->anim->frames[cc->animPos]->lRad,cc->anim->frames[cc->animPos]->rRad)))
    { if(cc->state==JUMPING)
      { cc->hVel+=(cc->hChangeVel*2);
        if(cc->hVel > (cc->maxWalkVel+cc->hChangeVel))
          cc->hVel = (cc->maxWalkVel+cc->hChangeVel);
      }
      else
      { cc->hVel+=cc->hChangeVel;
        if(cc->hVel > cc->maxWalkVel)
          cc->hVel = cc->maxWalkVel;
      }
      if(cc->hVel==0)
        cc->hVel+=1;
    }
    cc->facing=R;
  }
  //Key left...
  else if(key[KEY_LEFT] && cc->state!=HURTING)
  { if(!key[KEY_DOWN]) //&& (cc->state!=ATTACKING && onSolid(cc->hPos,cc->vPos, cc->anim->frames[cc->animPos]->dRad,cc->anim->frames[cc->animPos]->lRad,cc->anim->frames[cc->animPos]->rRad)))
    { if(cc->state==JUMPING)
      { cc->hVel-=(cc->maxWalkVel*2);
        if(cc->hVel < -(cc->maxWalkVel+cc->hChangeVel))
          cc->hVel = -(cc->maxWalkVel+cc->hChangeVel);
      }
      else
      { cc->hVel-=cc->hChangeVel;
        if(cc->hVel < -cc->maxWalkVel)
          cc->hVel = -cc->maxWalkVel;
      }
      if(cc->hVel==0)
        cc->hVel-=1;
    }
    cc->facing=L;
  }
  //Slowing down...
  else
  { if(cc->hVel > cc->hChangeVel)
      cc->hVel-=cc->hChangeVel;
    else if(cc->hVel < -cc->hChangeVel)
      cc->hVel+=cc->hChangeVel;
    else cc->hVel=0;
  }
  //key jump...
  if(key[KEY_Z])
  { if(onSolid(cc->hPos,cc->vPos, cc->anim->frames[cc->animPos]->dRad,cc->anim->frames[cc->animPos]->lRad,cc->anim->frames[cc->animPos]->rRad)) //cc->state!=JUMPING && cc->state!=HURTING)
    { // Play the jump sound...
      play_sample(jumpEffect, 200,140,1000, 0);
      cc->vVel = cc->jumpVel;
    }
  }

  //now change states...
  //if state is new, reset animation...

  if(cc->state!=HURTING)
  {
    //Attacking...
    if((key[KEY_C] || cc->state==ATTACKING)) //&& cc->state!=JUMPING)
    { if(cc->state!=ATTACKING)
      { if(!onSolid(cc->hPos,cc->vPos, cc->anim->frames[cc->animPos]->dRad,cc->anim->frames[cc->animPos]->lRad,cc->anim->frames[cc->animPos]->rRad))
          cc->anim = cc->jumpAndAtkAnim;
        else if(key[KEY_DOWN])
          cc->anim = cc->crouchAndAtkAnim;
        else
          cc->anim = cc->attackAnim;
        cc->animTimeMark=timer;
        cc->animPos=0;

        //cc->attackAnim->pos=0;
        //cc->attackAnim->timeMark=timer;
        cc->state=ATTACKING;
        //cc->anim=cc->attackAnim;

        //start a new attack...
        for(i=0; i<MAX_CC_ATTACKS_ON_SCREEN; i+=1)
          if(cc->attackList[i]->state==INACTIVE)
          { startAttack(cc->attackList[i], cc->hPos,cc->vPos+(-25), cc->facing);
          //{ startAttack(cc->attackList[i], cc->hPos,cc->vPos+(-25)+(cc->anim->atkOffset), cc->facing);
            i=MAX_CC_ATTACKS_ON_SCREEN;
          }

      }
      else
      { if(cc->animPos<cc->anim->size-1
              || checkTime(timer,cc->animTimeMark)<cc->anim->speed)
        { //cc->anim=cc->attackAnim;
        }
        else
        { cc->state=STANDING;
          cc->animTimeMark=timer;
          cc->animPos=0;
        }
      }
    }
    //Walking...
    if(cc->hVel!=0 && cc->vVel==0 && cc->state!=ATTACKING)
    { if(cc->state!=WALKING)
      { if(cc->facing==R)
          cc->animPos=3;
        else
          cc->animPos=1;
        cc->animTimeMark=timer;
        cc->state=WALKING;
      }
      cc->anim=cc->walkAnim;
    }
    //Crouching...
    else if(key[KEY_DOWN] && cc->vVel==0 && cc->state!=ATTACKING)
    { if(cc->state!=CROUCHING)
      { cc->animPos=0;
        cc->animTimeMark=timer;
        cc->state=CROUCHING;
      }
      cc->anim=cc->crouchAnim;
    }
    //Standing...
    else if(cc->hVel==0 && cc->vVel==0 && cc->state!=ATTACKING)
    { if(cc->state!=STANDING)
      { cc->animPos=0;
        cc->animTimeMark=timer;
        cc->state=STANDING;
      }
      cc->anim=cc->standAnim;
    }
    //Jumping...
    if(cc->state!=ATTACKING && (cc->vVel!=0 || !onSolid(cc->hPos,cc->vPos, cc->anim->frames[cc->animPos]->dRad,cc->anim->frames[cc->animPos]->lRad,cc->anim->frames[cc->animPos]->rRad)))
    { //printf("  Is jumping... \n");  readkey();
      if(cc->state!=JUMPING)
      { cc->animPos=0;
        cc->animTimeMark=timer;
        cc->state=JUMPING;
      }
      cc->anim=cc->jumpAnim;
    }

  } //End if not hurting...
  */

  animate(cc->anim,&(cc->animTimeMark),&(cc->animPos));
  sprite = cc->anim->frames[cc->animPos]->pic;

  //H Velocity...
  //if(!(cc->state==ATTACKING && onSolid(cc->hPos,cc->vPos, cc->anim->frames[cc->animPos]->dRad,cc->anim->frames[cc->animPos]->lRad,cc->anim->frames[cc->animPos]->rRad)))
  movePPS(&(cc->hPos),&(cc->vPos), &(cc->hFudge), cc->anim->frames[cc->animPos]->uRad,cc->anim->frames[cc->animPos]->dRad,cc->anim->frames[cc->animPos]->lRad,cc->anim->frames[cc->animPos]->rRad, &(cc->hVel), HORI);
  //V Velocity...
  movePPS(&(cc->hPos),&(cc->vPos), &(cc->vFudge), cc->anim->frames[cc->animPos]->uRad,cc->anim->frames[cc->animPos]->dRad,cc->anim->frames[cc->animPos]->lRad,cc->anim->frames[cc->animPos]->rRad, &(cc->vVel), VERT);

  //Gravity...
  //if you CAN go down, go down...
  addGravity(cc->hPos,cc->vPos, cc->anim->frames[cc->animPos]->dRad,cc->anim->frames[cc->animPos]->lRad,cc->anim->frames[cc->animPos]->rRad, &(cc->vVel));

  // Check if you still need to do the hurt blinking thingy...
  if(cc->isHurtInvincible)
  { //textprintf(sBuffer,font, 0,SH-50,TEXT_COLOR, "wait for %d ticks",cc->hurtBlinkTime);
    if(checkTime(timer,cc->hurtBlinkTimeMark)>=cc->hurtBlinkTime)
    { if(cc->isHurtAndIsTimeToDraw)  cc->isHurtAndIsTimeToDraw = F;
      else  cc->isHurtAndIsTimeToDraw = T;
      cc->hurtBlinkTimeMark = timer;
    }
    if(checkTime(timer,cc->hurtTimeMark)>=cc->hurtTime)
      cc->isHurtInvincible = F;
  }

  if(cc->isHurtInvincible && cc->isHurtAndIsTimeToDraw)
  { if (cc->facing==R)  draw_sprite(sBuffer,sprite, (SW/2)-(sprite->w/2)+camera,cc->vPos-(sprite->h/2));
    else  draw_sprite_h_flip(sBuffer,sprite, (SW/2)-(sprite->w/2)+camera,cc->vPos-(sprite->h/2));
  }
  else if(!cc->isHurtInvincible)
  { if (cc->facing==R)  draw_sprite(sBuffer,sprite, (SW/2)-(sprite->w/2)+camera,cc->vPos-(sprite->h/2));
    else  draw_sprite_h_flip(sBuffer,sprite, (SW/2)-(sprite->w/2)+camera,cc->vPos-(sprite->h/2));
  }

}




/***************************
***   updateAttackList   ***
***************************/
// Sent a pointer to a list of current attacks...
// This updates and draws all live attacks...
void updateAttackList(ATTACK_SHELL **atkList, int hRefPos, OO_SHELL **ooList)
{ ATTACK_SHELL *atkShell;
  ATTACK_DATA *atkData;
  BITMAP *sprite;
  OO_SHELL *oo;
  int ooCount;
  int i;

  for(i=0; i<MAX_CC_ATTACKS_ON_SCREEN; i+=1)
  {
    if(atkList[i]->state!=INACTIVE)
    { atkShell = atkList[i];
      atkData = atkList[i]->data;
      if(atkList[i]->state==MOVING)  atkShell->anim = atkList[i]->data->moveAnim;
      else if(atkList[i]->state==HITTING)  atkShell->anim = atkList[i]->data->hitAnim;


      // If this attack is not hitting any CC,
      // then make it move and fall!...
      if(atkShell->state!=HITTING)
      { atkShell->vVel+=GRAVITY;
        movePPSSansCollision(&(atkShell->hPos),&(atkShell->vPos), &(atkShell->hFudge), &(atkShell->hVel), HORI);
        movePPSSansCollision(&(atkShell->hPos),&(atkShell->vPos), &(atkShell->vFudge), &(atkShell->vVel), VERT);
      }

      // Check for contact!!  (with OO, silly)...
      for(ooCount=0; ooCount<MAX_OO_ON_SCREEN && atkShell->state!=HITTING; ooCount+=1)
      { if(ooList[ooCount]->state!=INACTIVE)
        { oo = ooList[ooCount];

          if(isInContact(atkShell->hPos,atkShell->vPos,atkShell->anim->frames[atkShell->animPos]->uRad,atkShell->anim->frames[atkShell->animPos]->dRad,atkShell->anim->frames[atkShell->animPos]->lRad,atkShell->anim->frames[atkShell->animPos]->rRad,
             oo->hPos,oo->vPos,oo->anim->frames[oo->animPos]->uRad,oo->anim->frames[oo->animPos]->dRad,oo->anim->frames[oo->animPos]->lRad,oo->anim->frames[oo->animPos]->lRad))
          { if((ooList[ooCount]->health -= atkShell->data->damagePower) <= 0)
            { // OO is hit!!  You've sunk my OO!...
              killOo(oo,atkShell->facing);
            }
            else
            { ooList[ooCount]->state=HURTING;
            }
            // Change the atk to a
            // state of HITTING...
            atkShell->anim = atkData->hitAnim;
            atkShell->animTimeMark = timer;
            atkShell->state = HITTING;
            atkShell->animPos = 0;
          }

        }
      }

      //Animate!  Yipee!!
      if(checkTime(timer,atkShell->animTimeMark) >= atkShell->anim->speed)
      { if(atkShell->animPos >= atkShell->anim->size-1)
        { if(atkShell->state==HITTING)
          { resetAttackShell(atkList[i], atkList[i]->data);
          }
          else if(atkShell->anim->loop)
          { atkShell->animPos=0;
            atkShell->animTimeMark = timer;
          }
        }
        else
        { atkShell->animPos+=1;
          atkShell->animTimeMark = timer;
        }
      } // Done animating, now set the sprite!...
      sprite = atkShell->anim->frames[atkShell->animPos]->pic;

      // Check if the attack is off the screen...
      if(atkList[i]->state!=INACTIVE)
      { if(atkShell->hPos > hRefPos+(SW/2)+50 || atkShell->hPos < hRefPos-(SW/2)-50
           || atkShell->vPos > SH+50 || atkShell->vPos < -(SH))
        { resetAttackShell(atkList[i], atkList[i]->data);
        }
        else
        { if(atkShell->facing==R)  draw_sprite(sBuffer,sprite, (atkShell->hPos-hRefPos)+(SW/2)-(sprite->w/2)/*+camera*/,atkShell->vPos-(sprite->h/2));
          else  draw_sprite_h_flip(sBuffer,sprite, (atkShell->hPos-hRefPos)+(SW/2)-(sprite->w/2)/*+camera*/,atkShell->vPos-(sprite->h/2));
        }
      }

    } // End "if not INACTIVE"...
  } // Now check the next ACTIVE attack!...

}




/*****************
***   killOo   ***
*****************/
// Sent the oo to kill, and the direction of the attack that knocked it out.
// This "kills" off an OO, but doesn't wipe it out altogether!...
// NOTE::  The OO is officially "gone" when it falls off the screen...
inline void killOo(OO_SHELL *oo, DIRECTION dir)
{ if(dir==R)  oo->hVel = oo->data->hHurtVel;
  else  oo->hVel = -(oo->data->hHurtVel);

  oo->state=DYING;
  oo->vVel = oo->data->vHurtVel;
  oo->anim = oo->data->dieAnim;
  oo->animTimeMark = timer;
  oo->animPos = 0;
}




/***************************
***   resetAttackShell   ***
***************************/
// Sent an pointer to an already initialized atk shell, and the data to init it...
// This restarts (not initializes) an attack shell.
void resetAttackShell(ATTACK_SHELL *atkShell, ATTACK_DATA *atkData)
{
  atkShell->data = atkData;
  atkShell->state = INACTIVE;
  atkShell->animPos=0;
  atkShell->animTimeMark=timer;
  //atkShell->anim = NULL;

  //atkShell->hVel = atkData->hInitVel;
  //atkShell->vVel = atkData->vInitVel;
}




// DEPRECATED...
/*
void startAttack(ATTACK_SHELL *atkShell, int sentHPos, int sentVPos, DIRECTION direction)
{
  atkShell->state = MOVING;
  atkShell->animTimeMark=timer;

  atkShell->hPos = sentHPos;
  atkShell->vPos = sentVPos;

  if(key[KEY_UP])
  { atkShell->hVel = atkShell->data->hInitUVel;
    atkShell->vVel = atkShell->data->vInitUVel;
  }
  else if(key[KEY_DOWN])
  { atkShell->hVel = atkShell->data->hInitDVel;
    atkShell->vVel = atkShell->data->vInitDVel;
  }
  else
  { atkShell->hVel = atkShell->data->hInitVel;
    atkShell->vVel = atkShell->data->vInitVel;
  }

  if(direction==L)  atkShell->hVel *= -1;

  atkShell->vFudge = 0;
  atkShell->hFudge = 0;
  atkShell->facing = direction;
}
*/



/************************
***   stateToString   ***
************************/
// Sent a STATE to be converted to a string and where to save the string in...
// This converts a STATE into its text string name.
void stateToString(STATE state, char *stateString)
{ if(state==STANDING)  strcpy(stateString,"STANDING");
  else if(state==WALKING)  strcpy(stateString,"WALKING");
  else if(state==JUMPING)  strcpy(stateString,"JUMPING");
  else if(state==ATTACKING)  strcpy(stateString,"ATTACKING");
  else  strcpy(stateString,"INVALID STATE");
}




/******************
***   animate   ***
******************/
// Sent a pointer to an animation...
// This animates the animation to its next frame, when the time is right.
inline void animate(ANIM *anim, int *timeMark, int *pos)
{ if (checkTime(timer,*timeMark) >= anim->speed)
  { if (*pos >= anim->size-1)
    { if (anim->loop)
      { *pos=0;
        *timeMark = timer;
      }
    }
    else
    { *pos+=1;
      *timeMark = timer;
    }
  }
}




/***********************
***   initializeCc   ***
***********************/
// Sent the string file name for the CC to load...
// This creates a CC in memory from a text data file.
// Returns a pointer to the new CC...
CC * initializeCc(char *fileName)
{ CC *newCc;
  FILE *ccFile;
  int i;

  newCc = (CC *)malloc(sizeof(CC));
  ccFile = fopen(fileName, "r");

  newCc->state=JUMPING;

  newCc->hFudge=0;
  newCc->vFudge=0;
  newCc->hVel=0;
  newCc->vVel=0;

  newCc->hPos=400;
  lowGameAdjust(&(newCc->hPos));
  newCc->vPos=SH/2;

  newCc->standAnim = initializeAnim(ccFile);
  newCc->walkAnim = initializeAnim(ccFile);
  newCc->jumpAnim = initializeAnim(ccFile);
  newCc->attackAnim = initializeAnim(ccFile);
  newCc->crouchAnim = initializeAnim(ccFile);
  newCc->hurtAnim = initializeAnim(ccFile);
  newCc->jumpAndAtkAnim = initializeAnim(ccFile);
  newCc->crouchAndAtkAnim = initializeAnim(ccFile);

  newCc->anim = newCc->jumpAnim;
  newCc->animTimeMark = timer;
  newCc->animPos = 0;

  for(i=0;i<MAX_CC_ATTACKS;i+=1)
    newCc->attackData[i]=NULL;
  newCc->attackData[1] = initializeAttack(CHIEKO_MAIN_ATK);

  for (i=0; i<MAX_CC_ATTACKS_ON_SCREEN; i+=1)
  { newCc->attackList[i] = (ATTACK_SHELL *)malloc(sizeof(ATTACK_SHELL));
    resetAttackShell(newCc->attackList[i], newCc->attackData[1]);
  }

  newCc->hChangeVel = 20;
  newCc->maxWalkVel = 120;
  newCc->jumpVel = -480;
  newCc->hHurtVel = 300;
  newCc->vHurtVel = -300;
  newCc->vAttackOffset = -42;
  newCc->vJumpAttackOffset = -42;
  newCc->vCrouchAttackOffset = 42;

  lowGameAdjust(&(newCc->hChangeVel));
  lowGameAdjust(&(newCc->maxWalkVel));
  lowGameAdjust(&(newCc->jumpVel));
  lowGameAdjust(&(newCc->hHurtVel));
  lowGameAdjust(&(newCc->vHurtVel));
  lowGameAdjust(&(newCc->vAttackOffset));
  lowGameAdjust(&(newCc->vJumpAttackOffset));
  lowGameAdjust(&(newCc->vCrouchAttackOffset));

  newCc->maxHealth=100;
  newCc->health=newCc->maxHealth;
  newCc->lives=3;
  newCc->facing=R;

  newCc->isHurtInvincible=F;
  newCc->hurtTime = GAME_TICKER*3;       // 3 seconds...
  newCc->hurtBlinkTime = GAME_TICKER/9;  // 9 times in 1 second...
  newCc->hurtTimeMark = timer;
  newCc->hurtBlinkTimeMark = timer;
  newCc->isHurtAndIsTimeToDraw = F;

  fclose(ccFile);
  return(newCc);
}


/***************************
***   initializeAttack   ***
***************************/
// Sent the file name for the attack data...
// This initializes a usable attack into memory.
// returns a pointer to the attack...
ATTACK_DATA * initializeAttack(char *fileName)
{ FILE *atkFile;
  ATTACK_DATA *newAtk;
  atkFile = fopen(fileName, "r");
  newAtk = (ATTACK_DATA *)malloc(sizeof(ATTACK_DATA));

  fscanf(atkFile,"%d",&(newAtk->hInitVel));
  fscanf(atkFile,"%d",&(newAtk->vInitVel));
  fscanf(atkFile,"%d",&(newAtk->hInitUVel));
  fscanf(atkFile,"%d",&(newAtk->vInitUVel));
  fscanf(atkFile,"%d",&(newAtk->hInitDVel));
  fscanf(atkFile,"%d",&(newAtk->vInitDVel));


  lowGameAdjust(&(newAtk->hInitVel));
  lowGameAdjust(&(newAtk->vInitVel));
  lowGameAdjust(&(newAtk->hInitUVel));
  lowGameAdjust(&(newAtk->vInitUVel));
  lowGameAdjust(&(newAtk->hInitDVel));
  lowGameAdjust(&(newAtk->vInitDVel));

  fscanf(atkFile,"%d",&(newAtk->damagePower));

  newAtk->moveAnim = initializeAnim(atkFile);
  newAtk->hitAnim = initializeAnim(atkFile);

  fclose(atkFile);
  return(newAtk);
}





/*************************
***   initializeAnim   ***
*************************/
// Sent the file pointer to the open CC data file...
// This loads a new animation into memory.
// Returns a pointer to the new animation...
ANIM * initializeAnim(FILE *file)
{
  ANIM *newAnim;
  int i;

  newAnim = (ANIM *)malloc(sizeof(ANIM));
  //This just catches the "===" separator...
  swipeln(file);

  fscanf(file,"%d",&(newAnim->size));
  newAnim->frames = (HIT_PIC **)calloc(newAnim->size,sizeof(HIT_PIC *));

  for (i=0; i<newAnim->size; i+=1)
  { newAnim->frames[i] = initializeHitPic(file);
  }

  fscanf(file,"%d",&(newAnim->speed));
  fscanf(file,"%d",(int *)&(newAnim->loop));

  return(newAnim);
}




/***************************
***   initializeHitPic   ***
***************************/
// Sent an open file with the hit pic data...
// This creates a new hit pic in memory with the data from the sent file.
// Returns the new hit pic...
HIT_PIC * initializeHitPic(FILE *file)
{
  HIT_PIC *newHitPic;
  char bmpFileName[50];
  BITMAP *tempBmp;


  newHitPic = (HIT_PIC *)malloc(sizeof(HIT_PIC));

  fscanf(file,"%s",bmpFileName);
  newHitPic->pic = load_bitmap(bmpFileName,gamePallete);
  if(lowGame)
  { tempBmp = newHitPic->pic;
    newHitPic->pic = create_bitmap(tempBmp->w/2,tempBmp->h/2);
    stretch_blit(tempBmp,newHitPic->pic, 0,0, tempBmp->w,tempBmp->h, 0,0, newHitPic->pic->w,newHitPic->pic->h);
    destroy_bitmap(tempBmp);
  }

  fscanf(file,"%d",&(newHitPic->uRad));
  fscanf(file,"%d",&(newHitPic->dRad));
  fscanf(file,"%d",&(newHitPic->lRad));
  fscanf(file,"%d",&(newHitPic->rRad));

  lowGameAdjust(&(newHitPic->uRad));
  lowGameAdjust(&(newHitPic->dRad));
  lowGameAdjust(&(newHitPic->lRad));
  lowGameAdjust(&(newHitPic->rRad));

  return(newHitPic);
}





/*********************
***   checkTime   ***
*********************/
// Sent a later time and an earlier time...
// This provides an accurate distance between the two times.
// Returns the difference between the two times...
inline int checkTime(int laterTime, int earlierTime)
{ if (laterTime - earlierTime >= 0)
    return (laterTime - earlierTime);
  return (laterTime - (earlierTime - MAX_TICKER));
}



/**********************
***   waitForGame   ***
**********************/
// This waits until the game is ready for the next loop.
inline void waitForGame(void)
{ while(checkTime(timer,timeMark) < 1);
  timeMark = timer;
}



/******************************
***   initializeComputer   ***
******************************/
// This starts ALLEGRO, the keyboard, the screen, you name it!
void initializeComputer(void)
{ char *tempString;

  allegro_init();
  install_keyboard();
  install_sound(DIGI_AUTODETECT,MIDI_NONE,tempString);
  //set_volume(255,255);
  set_color_depth(COLOR_DEPTH);
  //set_gfx_mode(GFX_AUTODETECT, SW,SH, 0,0);

  initializeTimer();

  // Initialize sound effects...
  jumpEffect = load_sample(JUMP_EFFECT_FILE_NAME);

  //sBuffer = create_bitmap(SW,SH);    //The screen buffer...
  //clear(sBuffer);
}




void initializeHighComputer(void)
{

  SW=640;
  SH=480;
  TILE_SIZE=40;
  SW_TILES=16;
  SH_TILES=12;
  GRAVITY=24;
  screenChange=4;
  healthBarHPos=20;
  healthBarVPos=20;
  healthBarW=20;
  healthBarH=100;

  if (fullscreen == 0)
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, SW,SH, 0,0);
  else
    set_gfx_mode(GFX_AUTODETECT, SW,SH, 0,0);

  sBuffer = create_bitmap(SW,SH);    //The screen buffer...
  clear(sBuffer);
}





/********************************
***   initializeLowComputer   ***
********************************/
// This starts ALLEGRO, the keyboard, the screen, you name it, in slow computer mode...
void initializeLowComputer(void)
{

  SW=320;
  SH=240;
  TILE_SIZE=20;
  SW_TILES=16;
  SH_TILES=12;
  GRAVITY=12;
  screenChange=2;
  healthBarHPos=10;
  healthBarVPos=10;
  healthBarW=10;
  healthBarH=50;

  if (fullscreen == 0)
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, SW,SH, 0,0);
  else
    set_gfx_mode(GFX_AUTODETECT, SW,SH, 0,0);

  sBuffer = create_bitmap(SW,SH);    //The screen buffer...
  clear(sBuffer);
}





void initializeTimer(void)
{ install_timer();
  install_int_ex(do_timer,BPS_TO_TIMER(GAME_TICKER));
  LOCK_VARIABLE(timer);
  LOCK_FUNCTION(do_timer);
  timer=0;
}





/******************
***   cleanUp   ***
******************/
// This deallocates game memory, and removes everything ALLEGRO.
void cleanUp(void)
{
  destroy_bitmap(sBuffer);
  //destroy_sample(jumpEffect);

  //remove_timer();
  //clear_keybuf();
  //remove_sound();
  //remove_keyboard();
  //allegro_exit();
}


/***********************
***   finishScreen   ***
***********************/
// This does the final blit to the screen, from the screen buffer...
inline void finishScreen(void)
{ textprintf_ex(sBuffer,font, 0,SH-10,TEXT_COLOR, -1, "%d",ticks);
  textprintf_ex(sBuffer,font, 0,SH-20,TEXT_COLOR, -1, "camera %d",camera);

  ticks+=1;
  if (ticks>MAX_TICKER)  ticks=0;

  vsync();
  blit(sBuffer,screen, 0,0, 0,0, sBuffer->w,sBuffer->h);
  waitForGame();
}




/***********************
***   updateCamera   ***
***********************/
// Sent the hPos to update the camera around (usually the cc hPos)...
// This adds key functionality to the camera, and puts it where it's supposed to be.
inline void updateCamera(int hPos)
{ if(key[KEY_D] || key[KEY_E])
  { if(camera > -(SW/3))
      camera -= screenChange;
  }
  else if(key[KEY_A])
  { if(camera < (SW/3))
      camera += screenChange;
  }
  else if(camera >= 4 || camera <= -4)
  { camera -= (abs(camera)/camera)*screenChange;
    //camera = 0;
  }
  else
  { camera = 0;
  }

  if(hPos-camera < SW/2)
    camera = -(SW/2-hPos);
  else if(hPos-camera > (level->walkground->w*TILE_SIZE)-SW/2)
    camera = -((level->walkground->w*TILE_SIZE)-SW/2-hPos);

}


