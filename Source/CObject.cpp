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
		int randSelect = rand() % 2 + 1;

		if (randSelect == 1)
		{
			int crateNR = rand() % 4 + 1;

			itoa(crateNR,nr,10);

			int randSelect = rand() % 2 + 1;

			strcpy(img,"data/objects/crate"); strcat(img,nr); strcat(img,"AndMask.bmp");
		}
		else
		{
			strcpy(img,"data/objects/bomb.bmp");
		}
	}
	else
	{
		static int crateCount;

		itoa(crateCount,nr,10);

		strcpy(img,"data/objects/crateAndMask"); strcat(img,nr); strcat(img,".bmp");

		crateCount++;
	}

	m_pObjSprite = new Sprite(img, RGB(0xff,0x00,0xff));

	m_pObjSprite->setBackBuffer(pBackBuffer);
	m_pObjSprite->isSpriteVisible = true;
}

//-----------------------------------------------------------------------------
// Name : ~CPlayer () (Destructor)
// Desc : CPlayer Class Destructor
//-----------------------------------------------------------------------------
CObject::~CObject()
{
	delete m_pObjSprite;
}

void CObject::UpdateObj(float dt)
{
	// Update sprite
	m_pObjSprite->update(dt);
}

void CObject::DrawObj()
{
	m_pObjSprite->draw();
}

void CObject::StartMoving()
{
	int x, y;

	x = rand() % (GetSystemMetrics(SM_CXSCREEN)-BLOCKSIZE*2) + BLOCKSIZE*2;
	y = -(rand() % (GetSystemMetrics(SM_CYSCREEN)+BLOCKSIZE*4) + 1);

	m_pObjSprite->mPosition = Vec2(x,y);

	if (strcmpi(m_pObjSprite->fileName,"data/objects/bomb.bmp") == 0)
		m_pObjSprite->mVelocity.y = rand() % 200 + 150;
	else
		m_pObjSprite->mVelocity.y = rand() % 60 + 30;
}

bool CObject::ObjColision(CPlayer* Player)
{
	// Daca obiectul a trecut de marginea de jos a ecranului, il resetam
	if (m_pObjSprite->mPosition.y >= GetSystemMetrics(SM_CYSCREEN))
	{
		m_pObjSprite->isSpriteVisible = true;
		StartMoving();
	}
	
	// Daca jucatorul a intrat intr-un obiect
	if (abs(m_pObjSprite->mPosition.x - Player->Position().x) <= 20 && abs(m_pObjSprite->mPosition.y - Player->Position().y) <= 20)
	{
		if (
			(strcmpi(m_pObjSprite->fileName,"data/objects/crate1AndMask.bmp") == 0
			|| strcmpi(m_pObjSprite->fileName,"data/objects/crate2AndMask.bmp") == 0
			|| strcmpi(m_pObjSprite->fileName,"data/objects/crate3AndMask.bmp") == 0
			|| strcmpi(m_pObjSprite->fileName,"data/objects/crate4AndMask.bmp") == 0)
			&& m_pObjSprite->isSpriteVisible
			)
		{
			Player->m_pPoints += rand() % 50 + 20;
			m_pObjSprite->isSpriteVisible = false;
		}
		else if (strcmpi(m_pObjSprite->fileName,"data/objects/bomb.bmp") == 0 && m_pObjSprite->isSpriteVisible)
		{
			m_pObjSprite->isSpriteVisible = false;
			return true;
		}
	}

	return false;
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