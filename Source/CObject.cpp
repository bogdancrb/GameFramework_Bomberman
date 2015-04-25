//-----------------------------------------------------------------------------
// File: CObject.cpp
//
// Desc: This file stores the objects of the game. This class performs tasks
//		such as object movement, minor physics and rendering.
//
// Original desig by Adam Hoult & Gary Simmsons. Modified by Bogdan Corbeanu.
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CObject Specific Includes
//-----------------------------------------------------------------------------
#include "CObject.h"

//-----------------------------------------------------------------------------
// Name : CObject () (Constructor)
// Desc : CObject Class Constructor
//-----------------------------------------------------------------------------
CObject::CObject(const BackBuffer *pBackBuffer)
{
	char img[255], nr[MAX_CRATE+1];

	if (MAX_CRATE > 10)
	{
		int crateNR = rand() % 4 + 1;

		itoa(crateNR,nr,10);

		strcpy(img,"data/crates/crate"); strcat(img,nr); strcat(img,"AndMask.bmp");
	}
	else
	{
		static int crateCount;

		itoa(crateCount,nr,10);

		strcpy(img,"data/crates/crateAndMask"); strcat(img,nr); strcat(img,".bmp");

		crateCount++;
	}

	m_pObjSprite = new Sprite(img, RGB(0xff,0x00,0xff));

	m_pObjSprite->setBackBuffer(pBackBuffer);
	m_pObjSprite->isSpriteVisible = false;
}

//-----------------------------------------------------------------------------
// Name : ~CPlayer () (Destructor)
// Desc : CPlayer Class Destructor
//-----------------------------------------------------------------------------
CObject::~CObject()
{
	delete m_pObjSprite;
}

void CObject::UpdateObj(float dt, int ID)
{
	// Update sprite
	m_pObjSprite->update(dt,ID);
}

void CObject::DrawObj()
{
	m_pObjSprite->draw();
}

void CObject::StartMoving()
{
	int x;

	x = rand() % GetSystemMetrics(SM_CXSCREEN) + 1;

	m_pObjSprite->mPosition = Vec2(x,0);

	m_pObjSprite->mVelocity.y = rand() % 50 + 30;
}

Vec2& CObject::ObjPosition()
{
	return m_pObjSprite->mPosition;
}

Vec2& CObject::ObjVelocity()
{
	return m_pObjSprite->mVelocity;
}

bool CObject::Visible()
{
	return m_pObjSprite->isSpriteVisible;
}

int CObject::Height()
{
	return m_pObjSprite->height();
}

int CObject::Width()
{
	return m_pObjSprite->width();
}

void CObject::ObjExplode()
{
	ObjPosition() = Vec2(0,(int)GetSystemMetrics(SM_CYSCREEN));
}