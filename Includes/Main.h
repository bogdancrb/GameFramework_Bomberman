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

#define MAX_CRATE 10
#define MAX_NPCS 10
#define BULLET_SPEED 50
#define EXPLOSION_SPEED 540 // Viteza de afisare a exploziei
#define EXPLOSION_RANGE 3+3+1+3+3 // Pe cate bock-uri este extinsa explozia (3 sus + 3 jos + 1 centru + 3 stanga + 3 dreapta = 13)
#define BLOCKSIZE 60 // Marimea unui block in pixeli (60x60)
#define BOMB_TIMER 4*50 // Dupa cat timp sa explodeze o bomba (10*50 => 10 sec, 20*50 => 20 sec, etc.)

#endif // _MAIN_H_