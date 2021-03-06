//-----------------------------------------------------------------------------
// File: CPlayer.h
//
// Desc: This file stores the player object class. This class performs tasks
//	   such as player movement, some minor physics as well as rendering.
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

#ifndef _CPLAYER_H_
#define _CPLAYER_H_

//-----------------------------------------------------------------------------
// CPlayer Specific Includes
//-----------------------------------------------------------------------------
#include "Main.h"
#include "Sprite.h"
#include "CBomb.h"
#include "CMap.h"
#include "CLives.h"

//-----------------------------------------------------------------------------
// Main Class Definitions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CPlayer (Class)
// Desc : Player class handles all player manipulation, update and management.
//-----------------------------------------------------------------------------
class CPlayer
{
public:
	//-------------------------------------------------------------------------
	// Enumerators
	//-------------------------------------------------------------------------
	enum DIRECTION 
	{ 
		DIR_NONE	= 0,
		DIR_FORWARD	 = 1, 
		DIR_BACKWARD	= 2, 
		DIR_LEFT		= 4, 
		DIR_RIGHT	   = 8, 
	};

	enum ESpeedStates
	{
		SPEED_START,
		SPEED_STOP
	};

	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
			 CPlayer(const BackBuffer *pBackBuffer);
	virtual ~CPlayer();

	//-------------------------------------------------------------------------
	// Public Functions for This Class.
	//-------------------------------------------------------------------------
	void					Update(float dt, int ID = 0);
	void					Draw(int ID = 0);
	void					Move(ULONG ulDirection, int ID = 0);

	bool&					CanMove();

	Vec2&					Position(int ID = 0);
	Vec2&					PlayerDecalPos(int ID = 0);	// Pentru a accaesa pozitia decalata jucatorului
	Vec2&					PlayerOldPos(int ID = 0);		// Pentru a accaesa pozitia veche a jucatorului
	Vec2&					Velocity(int ID = 0);

	void					PlayerColision(CMap* Map, int ID = 0);	// Detectare coliziune intre jucator si block de pe harta

	void					PlaceBomb(vector<CBomb*>* Bomb, BackBuffer* BBuffer, int ID = 0); // Punem o bomba la pozitia jucatorului

	void					Explode(int ID = 0);
	bool					AdvanceExplosion(int ID = 0);
	bool					AdvanceMovement(int ID = 0);

	void					Killed();

	//functie accesor --numarul initial de vieti ale jucatorului
	int&					get_no_lives(){ return no_lives; }
	vector<Lives*>			get_m_pLives(){ return m_pLives; }
	bool&					get_if_is_dead(){ return is_dead; }

	int						Width(int ID = 0) { return m_pSprite[ID]->width(); }
	int						Height(int ID = 0) { return m_pSprite[ID]->height(); }

private:
	//-------------------------------------------------------------------------
	// Private Variables for This Class.
	//-------------------------------------------------------------------------
	AnimatedSprite*			m_pSprite[4];
	int						m_iSpriteFrame;
	
	bool					m_bExplosion;
	AnimatedSprite*			m_pExplosionSprite;
	int						m_iExplosionFrame;

	bool					m_pCanMove; // Pentru ca jucatrul sa nu se deplaseze in mai multe directii in acelasi timp
	
	//player lives
	//-----------------------------------
	std::vector<Lives*>		m_pLives;
	int						no_lives;
	//-----------------------------------
	bool				    is_dead;

	int						m_pMaxBombs;

public:
	//-------------------------------------------------------------------------
	// Keep these public because they need to be modified externally.
	//-------------------------------------------------------------------------
	long int					m_pPoints;
	bool						ResetPosition; // Pentru a reseta pozitia jucatorului la cea initiala
};

#endif // _CPLAYER_H_