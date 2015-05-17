//-----------------------------------------------------------------------------
// File: CBomb.cpp
//
// Desc : Bomb class handles all bomb manipulation
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CBomb Specific Includes
//-----------------------------------------------------------------------------
#include "CBomb.h"

//-----------------------------------------------------------------------------
// Name : CBomb () (Constructor)
// Desc : CBomb Class Constructor
//-----------------------------------------------------------------------------
CBomb::CBomb(const BackBuffer *pBackBuffer, int ID)
{
	if (ID == 0)
	{
		m_pBombSprite = new Sprite("data/objects/bomb.bmp", RGB(0xff,0x00,0xff));
	}
	if (ID == 1)
	{
		m_pBombSprite = new Sprite("data/objects/specialbomb.bmp", RGB(0xff,0x00,0xff));
	}

	m_pBombSprite->setBackBuffer(pBackBuffer);

	// Animation frame crop rectangle
	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = 64;
	r.bottom = 64;

	// Incarcam cate un sprite pentru fiecare bucata de explozie
	for (int index = 0; index < EXPLOSION_RANGE; index++)
	{
		m_pBombExplosionSprite[index] = new AnimatedSprite("data/explosions/explosion.bmp", "data/explosions/explosionmask.bmp", r, 25);
		//m_pBombExplosionSprite[index]	= new AnimatedSprite("data/explosionandmask.bmp", RGB(0xff,0x00,0xff), r, 16);
		m_pBombExplosionSprite[index]->setBackBuffer(pBackBuffer);
	}
	m_bBombExplosion = false;
	m_iBombExplosionFrame	= 0;

	// Marcam bomba ca fiind inactiva
	m_BombIsActive = false;
}

//-----------------------------------------------------------------------------
// Name : ~CBomb () (Destructor)
// Desc : CBomb Class Destructor
//-----------------------------------------------------------------------------
CBomb::~CBomb()
{
	delete m_pBombSprite;
	for (int index = 0; index < EXPLOSION_RANGE; index++)
		delete m_pBombExplosionSprite[index];
}

void CBomb::UpdateBomb(float dt, int ID)
{
	// Update sprite
	m_pBombSprite->update(dt,ID);
}

void CBomb::DrawBomb()
{
	if(!m_bBombExplosion)
		m_pBombSprite->draw();
	else
		for (int index = 0; index < EXPLOSION_RANGE; index++)
			m_pBombExplosionSprite[index]->draw();
}

Vec2& CBomb::BombPosition()
{
	return m_pBombSprite->mPosition;
}

Vec2& CBomb::BombExplosionPosition(int index)
{
	return m_pBombExplosionSprite[index]->mPosition;
}

Vec2& CBomb::BombVelocity()
{
	return m_pBombSprite->mVelocity;
}

void CBomb::BombExplode(int ID)
{
	//-------------------------------------------------------------------
	// Forma explozie cu vectori
	// Daca EXPLOSION_RANGE = 13 avem:
	//								  12
	//								  10
	//								   8
	//							5 3 1 (0) 2 4 6
	//							       7
	//								   9
	//								  11
	// Unde numerele reprezinta indicii din vector m_pBombExplosionSprite 
	//-------------------------------------------------------------------

	int k = 0; // Folosim o variabila pentru a memora pozitiile bombei fata de jucator
	
	m_pBombExplosionSprite[0]->mPosition = m_pBombSprite->mPosition; // Prima explozie are loc pe centru (pe pozitia bombei)

	for (int index = 1; index <= (EXPLOSION_RANGE-1)/2; index+=2)
	{
		k += BLOCKSIZE; // Setam viitoarele coordonate in functie de BLOCKSIZE (60, 120, 180 etc.)

		// Setam jumate din explozii sa aiba loc pe axa X
		m_pBombExplosionSprite[index]->mPosition = m_pBombSprite->mPosition + Vec2(-k,0);
		m_pBombExplosionSprite[index+1]->mPosition = m_pBombSprite->mPosition + Vec2(k,0);

		// Setam cealalta jumate din explozii sa aiba loc pe axa Y
		m_pBombExplosionSprite[index+(EXPLOSION_RANGE)/2]->mPosition = m_pBombSprite->mPosition + Vec2(0,-k);
		m_pBombExplosionSprite[index+(EXPLOSION_RANGE+1)/2]->mPosition = m_pBombSprite->mPosition + Vec2(0,k);
	}

	m_bBombExplosion = true;
}

bool CBomb::BombAdvanceExplosion()
{
	if(m_bBombExplosion)
	{
		for (int index = 0; index < EXPLOSION_RANGE; index++)
		{
			m_pBombExplosionSprite[index]->SetFrame(m_iBombExplosionFrame++);
			if(m_iBombExplosionFrame==m_pBombExplosionSprite[index]->GetFrameCount())
			{
				m_bBombExplosion = false;
				m_iBombExplosionFrame = 0;
				m_pBombSprite->mVelocity = Vec2(0,0);
				return false;
			}
		}
	}

	return true;
}