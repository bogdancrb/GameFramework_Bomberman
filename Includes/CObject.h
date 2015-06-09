//-----------------------------------------------------------------------------
// File: CObject.h
//
// Desc: This file stores the objects of the game. This class performs tasks
//		such as object movement, minor physics and rendering.
//
// Original desig by Adam Hoult & Gary Simmsons. Modified by Bogdan Corbeanu.
//-----------------------------------------------------------------------------

#ifndef _COBJECT_H_
#define _COBJECT_H_

//-----------------------------------------------------------------------------
// CObject Specific Includes
//-----------------------------------------------------------------------------
#include "Main.h"
#include "Sprite.h"
#include "CPlayer.h"

//-----------------------------------------------------------------------------
// Main Class Definitions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CObject (Class)
// Desc : Object class handles all objects manipulation in the game, update and management.
//-----------------------------------------------------------------------------
class CObject
{
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
			 CObject(const BackBuffer *pBackBuffer);
	virtual ~CObject();

	//-------------------------------------------------------------------------
	// Public Functions for This Class.
	//-------------------------------------------------------------------------
	void					UpdateObj(float dt);
	void					DrawObj();
	void					StartMoving();
	Vec2&					ObjPosition();
	Vec2&					ObjVelocity();
	bool					Visible();
	int						Width();
	int						Height();

	bool					ObjColision(CPlayer* Player);

	void					ObjExplode();
private:
	//-------------------------------------------------------------------------
	// Private Variables for This Class.
	//-------------------------------------------------------------------------
	Sprite*					m_pObjSprite;
};

#endif // _COBJECT_H_