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
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>
#include "allegro.h"

#define TITLE_IMAGE     "cs_images/title.bmp"
#define CHIEKO_DATA     "cs_data/cs.cc"
#define CHIEKO_MAIN_ATK "cs_data/cs_riceball.atk"
#define LEVEL_1_DATA    "cs_data/01_data.lvl"
#define LEVEL_1_TILES   "cs_data/01_tiles.lvl"
#define BGM_FILE_PATH   "cs_music/bgm1a.mid"

#define JUMP_EFFECT_FILE_NAME  "cs_music/jump01.wav"

#define COLOR_DEPTH   8


#define SECONDS       1000
#define TIMER         2
#define GAME_TICKER   60
#define MAX_TICKER    1000000000

#define TEXT_COLOR          215
#define BACKGROUND_COLOR    128

#define MAX_TILES    256
#define MAX_ITEMS    256

#define MAX_CC_ATTACKS            30
#define MAX_CC_ATTACKS_ON_SCREEN  25
#define MAX_OO                    256
#define MAX_OO_ON_SCREEN          10

#define HURT_POINTS  20

//#define PIC_SET_LIST_SIZE   50
//#define TILE_SIZE     40
//#define SW_TILES      16
//#define SH_TILES      12
//#define GRAVITY  25
//#define SW  320  //640
//#define SH  240  //480



/***************
***   FLAG   ***
***************/
// Is a boolean flag...
typedef enum
{ F=0, T=1
} FLAG;


/********************
***   DIRECTION   ***
********************/
// Is used mostly for the cc...
typedef enum
{ L=1, R=2, U=3, D=4
} DIRECTION;


/***************
***   AXIS   ***
***************/
// Is for more general directions...
typedef enum
{ HORI, VERT
} AXIS;



/***************
***   PASS   ***
***************/
// Is used mostly for a tile...
typedef enum
{ SOLID=0, PASSABLE=1, PASS_FROM_BOTTOM=2
} PASS;




/****************
***   STATE   ***
****************/
// Is used mostly for the cc...
typedef enum
{ WALKING, JUMPING, STANDING, CROUCHING, ATTACKING,
  MOVING, HITTING, INACTIVE, FALLING, FLYING,
  BOUNCING, HURTING, DYING, ACTIVE
} STATE;





typedef enum
{
  GND_WALKER,
  GND_IMOBILE_TOSSER

} OO_TYPE;




/******************
***   HIT_PIC   ***
******************/
// This is a frame of an animation, including the pic and its hit radius...
typedef struct
{ BITMAP *pic;

  // A "radius" is from the center of the pic outwards...
  //
  //          ^ uRad
  //          |
  //    lRad  |     rRad
  //    <-----|----->
  //          |
  //          |
  //          v dRad
  //
  // The radii, in the example shown, would all be positive.
  // A negative radius value would send the radius in the
  // opposite direction...
  int uRad;
  int dRad;
  int lRad;
  int rRad;
} HIT_PIC;




/***************
***   ANIM   ***
***************/
// Is an animation!...
typedef struct
{ HIT_PIC **frames;
  FLAG loop;        //If the animation should loop or not...
  int speed;        //The speed at which to animate...    (60 fps)
  int size;         //The size of the array...

} ANIM;






/**********************
***   ATTACK_DATA   ***
**********************/
// Is a throwable attack, but just a storage...
typedef struct
{ ANIM *moveAnim;
  ANIM *hitAnim;

  int hInitVel;
  int vInitVel;
  int hInitUVel;
  int vInitUVel;
  int hInitDVel;
  int vInitDVel;

  int damagePower;
} ATTACK_DATA;




/***********************
***   ATTACK_SHELL   ***
***********************/
// Is what is created when you attack, and uses an ATTACK...
typedef struct
{ ATTACK_DATA *data;
  STATE state;

  ANIM *anim;
  int animTimeMark;
  int animPos;

  DIRECTION facing;
  int hVel;
  int vVel;
  int hPos;
  int vPos;
  float hFudge;
  float vFudge;
} ATTACK_SHELL;




/*************
***   CC   ***
*************/
// Is a controllable character...
typedef struct
{
  STATE state;    //The current state of the cc...

  ATTACK_DATA *attackData[MAX_CC_ATTACKS];
  ATTACK_SHELL *attackList[MAX_CC_ATTACKS_ON_SCREEN];

  int hVel;
  int vVel;
  int hPos;      //Positions, at the cc's center...
  int vPos;
  float hFudge;
  float vFudge;

  ANIM *standAnim;
  ANIM *walkAnim;
  ANIM *jumpAnim;
  ANIM *crouchAnim;
  ANIM *hurtAnim;

  ANIM *attackAnim;
  ANIM *jumpAndAtkAnim;
  ANIM *crouchAndAtkAnim;

  ANIM *anim;
  int animTimeMark;
  int animPos;

  DIRECTION facing;
  int maxHealth;
  int health;
  int lives;

  int hChangeVel;
  int maxWalkVel;
  int hHurtVel;
  int vHurtVel;
  int jumpVel;
  int vAttackOffset;
  int vJumpAttackOffset;
  int vCrouchAttackOffset;

  FLAG isHurtInvincible;
  FLAG isHurtAndIsTimeToDraw;
  int hurtBlinkTimeMark;
  int hurtTimeMark;
  int hurtBlinkTime;      //How fast the character "blinks" after being hurt...
  int hurtTime;           //How long to stay invincible after being hurt...

} CC;





/******************
***   OO_DATA   ***
******************/
// This is the OO data, saved in RAM,
// to be used to load an OO shell...
typedef struct
{
  // This tells the computer what
  // the classification of oo this,
  // um, oo is.
  OO_TYPE type;

  ANIM *walkAnim;
  ANIM *standAnim;
  ANIM *jumpAnim;
  ANIM *flyAnim;
  ANIM *fallAnim;
  ANIM *dieAnim;

  int initHealth;
  int hInitVel;
  int vInitVel;

  int hHurtVel;
  int vHurtVel;

  //These flags determine how the oo behaves...
  FLAG canNotFallOffEdge;
  FLAG aims;
  //FLAG doesNotHaveGravity;
  //FLAG passesThroughSolid;
  //FLAG canWalk;
  //FLAG canFly;
  //FLAG canBounce;
  //FLAG canFall;
  //FLAG canDie;

} OO_DATA;




/*******************
***   OO_SHELL   ***
*******************/
// This is just the currently loaded data
// and info on a live OO...
typedef struct
{
  STATE state;

  // This is a pointer to OO "data"...
  // See, there can be many OO's of
  // the same type on the screen at the
  // same time, and so, they all have
  // their own OO "shell" to take care of
  // personal data, but use this "data" to
  // reference more general stuff, like
  // grafics for animations and stuff...
  OO_DATA *data;

  // Animation stuff...
  ANIM *anim;
  int animTimeMark;
  int animPos;

  int hVel;
  int vVel;
  int hPos;
  int vPos;
  float hFudge;
  float vFudge;

  int health;
  DIRECTION facing;

  // This is just backup stuff...  See, if the
  // OO doesn't actually "die" but just walks off
  // the screen, then it puts itself back into
  // the level OO map for future loading...
  int storeHLevelPos;
  int storeVLevelPos;
  int storeColor;

} OO_SHELL;





/***************
***   TILE   ***
***************/
// Is used to make up the walkground...
typedef struct
{ BITMAP **pics;       //The list of pictures for the tile...
  int speed;
  int size;

  int animTimeMark;
  int animPos;

  PASS passability;    //How the cc reacts to these tiles...
} TILE;





/***************
***   ITEM   ***
***************/
// This is a collectable item floating around a level...
typedef struct
{
  BITMAP **pics;       //The list of pictures for the item...
  int speed;
  int size;

  int animTimeMark;
  int animPos;

  int pointsUp;
  int livesUp;
  int healthUp;
  int weaponChange;

} ITEM;




/****************
***   LEVEL   ***
****************/
// Is a world for the cc to explore...
typedef struct
{
  OO_DATA *ooData[MAX_OO];
  OO_SHELL *ooList[MAX_OO_ON_SCREEN];
  int hPosLastOoCheck;

  int wTiles;
  int hTiles;

  TILE *tiles[MAX_TILES];
  ITEM *items[MAX_ITEMS];

  BITMAP *farground;      //The far background picture...
  BITMAP *scrollground;   //Parallax scrolling...
  BITMAP *walkground;     //The parts you walk on...
  BITMAP *ooground;       //The oo placement...
  BITMAP *itemground;     //Where the items are placed...
  int hScrollChange;      //How fast the scrollground changes...
  int hScrollPos;         //The current position of the scrollground...
  int hScrollMark;        //Where the cc was last...

  int hInitPos;
  int vInitPos;

} LEVEL;







