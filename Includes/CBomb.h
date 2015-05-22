//-----------------------------------------------------------------------------
// File: CBomb.h
//
// Desc: This file stores the bomb objects and animations, including mechanics
//
// Original design by Adam Hoult & Gary Simmons.
//-----------------------------------------------------------------------------

#ifndef _CBOMB_H_
#define _CBOMB_H_

//-----------------------------------------------------------------------------
// CBomb Specific Includes
//-----------------------------------------------------------------------------
#include "Main.h"
#include "Sprite.h"
#include "CMap.h"

//-----------------------------------------------------------------------------
// Main Class Definitions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CBomb (Class)
// Desc : Bomb class handles all bomb manipulation
//-----------------------------------------------------------------------------
class CBomb
{
public:
	//-------------------------------------------------------------------------
	// Enumerators
	//-------------------------------------------------------------------------


	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
			 CBomb(const BackBuffer *pBackBuffer, int ID = 0);
	virtual ~CBomb();

	//-------------------------------------------------------------------------
	// Public Functions for This Class.
	//-------------------------------------------------------------------------
	void					UpdateBomb(float dt, int ID = 0);
	void					DrawBomb();
	Vec2&					BombPosition();
	Vec2&					BombVelocity();
	Vec2&					BombExplosionPosition(int index);

	void					BombExplode(CMap* Map, int ID = 0);
	bool					BombAdvanceExplosion();

	bool					BombColision(Vec2 Position, CMap* Map);

	int						Width() { return m_pBombSprite->width(); }
	int						Height() { return m_pBombSprite->height(); }

private:
	//-------------------------------------------------------------------------
	// Private Variables for This Class.
	//-------------------------------------------------------------------------
	Sprite*					m_pBombSprite;

	bool					m_bBombExplosion;
	AnimatedSprite*			m_pBombExplosionSprite[EXPLOSION_RANGE];
	int						m_iBombExplosionFrame;

public:
	//-------------------------------------------------------------------------
	// Keep these public because they need to be modified externally.
	//-------------------------------------------------------------------------
	bool					m_BombIsActive;
};

#endif // _CBOMB_H_