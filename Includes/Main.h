//-----------------------------------------------------------------------------
// File: Main.h
//
// Desc: Main application entry / handling header file.
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

#ifndef _MAIN_H_
#define _MAIN_H_

//-----------------------------------------------------------------------------
// Main Application Includes
//-----------------------------------------------------------------------------
#define CRTDBG_MAP_ALLOC
#include "..\\Res\\resource.h"
#include <windows.h>
#include <crtdbg.h>
#include <assert.h> 
#include "Commdlg.h"
#include <tchar.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

//-----------------------------------------------------------------------------
// Common defines
//-----------------------------------------------------------------------------
#define CAPS1			94
#define C1_TRANSPARENT	1


#define EPS 1e-3 // epsilon (the smallest float value used)
#define PI 3.14159265358979323846
#define DEG2RAD(deg) (PI * (deg) / 180.0)
#define RAD2DEG(rad) ((rad) * 180.0 / PI)


//------------------------[Valori modificabile]--------------------------------
#define BLOCKSIZE 60 // Marimea unui block in pixeli (60x60)

#define MAX_CRATE 25 // Numarul maxim de cutii/bombe care sa cada din cer
#define MAX_NPCS 3 // Numarul maxim de NPC-uri
#define MAX_LEVELS 1 // Numarul maxim de normal game levels

// Nivelele bonus si boss (cele bonus sunt negative pentru a putea lucra fara probleme)
#define BONUS_LEVEL1 -1
#define BONUS_LEVEL2 -2
#define	BOSS_LEVEL 999
#define TEST_LEVEL 0

#define EXPLOSION_SPEED 3*50 // Viteza de afisare a exploziei (1*50 => 1 frame/sec, 2*50 => 2 frame/sec, etc.)
#define EXPLOSION_RANGE 2+2+1+2+2 // Pe cate bock-uri este extinsa explozia (2 sus + 2 jos + 1 centru + 2 stanga + 2 dreapta = 9)
#define BOMB_TIMER 3*50 // Dupa cat timp sa explodeze o bomba (10*50 => 10 sec, 20*50 => 20 sec, etc.)

#define NPC_SPEED 0.7*50 // Viteza de schimbare a directiei NPC (1*50 => 1 block/sec, 2*50 => 2 block/sec, etc.)
#define NPC_SPAWN_DISTANCE 3*BLOCKSIZE // Distanta minima in block-uri dintre jucator si NPC (ex: 1*BLOCKSIZE = 60, 2*BLOCKSIZE = 120, etc.)
#define	NPC_FOLLOW_DISTANCE 4*BLOCKSIZE // Distanta in block-uri de la care NPC incepe sa urmareasca jucatorul (ex: 1*BLOCKSIZE = 60, 2*BLOCKSIZE = 120, etc.)
//-----------------------------------------------------------------------------

#endif // _MAIN_H_