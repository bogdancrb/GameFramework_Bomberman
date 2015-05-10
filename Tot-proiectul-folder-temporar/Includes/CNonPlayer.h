//-----------------------------------------------------------------------------
// File: CNonPlayer.cpp
//
// Desc: This file stores the NPC object class. This class performs tasks
//	   such as NPC movement, some minor physics as well as rendering.
//
// Original design by Adam Hoult & Gary Simmons. Modified by Bogdan Corbeanu.
//-----------------------------------------------------------------------------

#ifndef _CNONPLAYER_H_
#define _CNONPLAYER_H_

//-----------------------------------------------------------------------------
// CNonPlayer Specific Includes
//-----------------------------------------------------------------------------
#include "Main.h"
#include "Sprite.h"

//-----------------------------------------------------------------------------
// Main Class Definitions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CNonPlayer (Class)
// Desc : NPC class handles all NPC manipulation, update and management.
//-----------------------------------------------------------------------------
class CNonPlayer
{
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
			 CNonPlayer(const BackBuffer *pBackBuffer,int ID);
	virtual ~CNonPlayer();

	//-------------------------------------------------------------------------
	// Public Functions for This Class.
	//-------------------------------------------------------------------------
	void					UpdateNPC( float dt , int ID);
	void					DrawNPC();
	void					StartMoving();
	Vec2&					NPCPosition();
	Vec2&					NPCVelocity();
	bool					Visible();

	void					NPCExplode(int ID = 0);
	bool					NPCAdvanceExplosion();
	void					NPCFire();

private:
	//-------------------------------------------------------------------------
	// Private Variables for This Class.
	//-------------------------------------------------------------------------
	Sprite*					m_pNPCSprite;

	bool					m_bNPCExplosion;
	AnimatedSprite*			m_pNPCExplosionSprite;
	int						m_iNPCExplosionFrame;

public:
	//-------------------------------------------------------------------------
	// Keep these public because they need to be modified externally.
	//-------------------------------------------------------------------------
	int						m_pNPCHealth;
};

#endif // _CNONPLAYER_H_