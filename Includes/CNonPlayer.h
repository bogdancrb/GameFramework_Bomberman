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
#include "CMap.h"
#include "CPlayer.h"
#include "CBomb.h"

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
			 CNonPlayer(const BackBuffer *pBackBuffer,int ID = 0);
	virtual ~CNonPlayer();

	//-------------------------------------------------------------------------
	// Public Functions for This Class.
	//-------------------------------------------------------------------------
	void					UpdateNPC(float dt);
	void					DrawNPC();

	void					NPCMove(CPlayer* Player, CMap* Map, CBomb* Bomb);

	Vec2&					NPCPosition();
	Vec2&					NPCOldPosition();
	Vec2&					NPCDecalPosition();
	Vec2&					NPCVelocity();

	bool&					Visible();

	void					SelectVelocity(CPlayer* Player, CMap* Map, CBomb* Bomb);

	void					NPCColision(CMap* Map, CBomb* Bomb);	// Detectare coliziune intre jucator si block de pe harta
	bool					NPCDetectColision(CMap* Map, CBomb* Bomb, double PosX, double PosY);

	void					NPCExplode(int ID = 0);
	bool					NPCAdvanceExplosion();

private:
	//-------------------------------------------------------------------------
	// Private Variables for This Class.
	//-------------------------------------------------------------------------
	Sprite*					m_pNPCSprite;

	bool					m_bNPCExplosion;
	AnimatedSprite*			m_pNPCExplosionSprite;
	int						m_iNPCExplosionFrame;

	int						MoveTimer; // Pentru a putea stabili o viteza de schimbare a velocitatii

	Vec2					DistanceToPlayer; // Pentru a memora distanta ramasa intre NPC si jucator

public:
	//-------------------------------------------------------------------------
	// Keep these public because they need to be modified externally.
	//-------------------------------------------------------------------------
};

#endif // _CNONPLAYER_H_