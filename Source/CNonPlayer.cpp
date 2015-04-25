//-----------------------------------------------------------------------------
// File: CNoPlayer.cpp
//
// Desc: This file stores the NPC object class. This class performs tasks
//       such as NPC movement, some minor physics as well as rendering.
//
// Original design by Adam Hoult & Gary Simmons. Modified by Bogdan Corbeanu.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CNonPlayer Specific Includes
//-----------------------------------------------------------------------------
#include "CNonPlayer.h"

//-----------------------------------------------------------------------------
// Name : CNonPlayer () (Constructor)
// Desc : CNonPlayer Class Constructor
//-----------------------------------------------------------------------------
CNonPlayer::CNonPlayer(const BackBuffer *pBackBuffer, int ID)
{
	//m_pSprite = new Sprite("data/planeimg.bmp", "data/planemask.bmp");
	if (ID == 1)
	{
		m_pNPCSprite = new Sprite("data/characters/enemy.bmp", RGB(0xff,0x00,0xff));
	}
	if (ID == 2)
	{
		m_pNPCSprite = new Sprite("data/guns/bulletandmask.bmp", RGB(0xff,0x00,0xff));
	}

	m_pNPCSprite->setBackBuffer(pBackBuffer);
	/*m_eSpeedState = SPEED_STOP;
	m_fTimer = 0;*/

	// Animation frame crop rectangle
	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = 64;
	r.bottom = 64;

	m_pNPCExplosionSprite	= new AnimatedSprite("data/explosions/explosion.bmp", "data/explosions/explosionmask.bmp", r, 25);
	//m_pExplosionSprite	= new AnimatedSprite("data/explosionandmask.bmp", RGB(0xff,0x00,0xff), r, 16);
	m_pNPCExplosionSprite->setBackBuffer(pBackBuffer);
	m_bNPCExplosion		= false;
	m_iNPCExplosionFrame	= 0;

	m_pNPCSprite->isSpriteVisible = false;
	m_pNPCHealth = 100;
}

//-----------------------------------------------------------------------------
// Name : ~CNonPlayer () (Destructor)
// Desc : CNonPlayer Class Destructor
//-----------------------------------------------------------------------------
CNonPlayer::~CNonPlayer()
{
	delete m_pNPCSprite;
	delete m_pNPCExplosionSprite;
}

void CNonPlayer::UpdateNPC(float dt, int ID)
{
	// Update sprite
	m_pNPCSprite->update(dt,ID);
}

void CNonPlayer::DrawNPC()
{
	if(!m_bNPCExplosion)
		m_pNPCSprite->draw();
	else
		m_pNPCExplosionSprite->draw();
}

void CNonPlayer::StartMoving()
{
	int x;

	x = rand() % GetSystemMetrics(SM_CXSCREEN) + 1;

	m_pNPCSprite->mPosition = Vec2(x,0);

	m_pNPCSprite->mVelocity.y = rand() % 50 + 30;
}

Vec2& CNonPlayer::NPCPosition()
{
	return m_pNPCSprite->mPosition;
}

Vec2& CNonPlayer::NPCVelocity()
{
	return m_pNPCSprite->mVelocity;
}

bool CNonPlayer::Visible()
{
	return m_pNPCSprite->isSpriteVisible;
}

void CNonPlayer::NPCExplode(int ID)
{
	if (ID == 1)
	{
		m_pNPCExplosionSprite->mPosition = m_pNPCSprite->mPosition;
		m_pNPCExplosionSprite->SetFrame(0);
		m_bNPCExplosion = true;
		//PlaySound("data/sounds/explosion.wav", NULL, SND_FILENAME | SND_ASYNC);
	}

	m_pNPCSprite->mVelocity.x = m_pNPCSprite->mVelocity.y = 0;
	NPCPosition() = Vec2(0,(int)GetSystemMetrics(SM_CYSCREEN));
}

bool CNonPlayer::NPCAdvanceExplosion()
{
	if(m_bNPCExplosion)
	{
		m_pNPCExplosionSprite->SetFrame(m_iNPCExplosionFrame++);
		if(m_iNPCExplosionFrame==m_pNPCExplosionSprite->GetFrameCount())
		{
			m_bNPCExplosion = false;
			m_iNPCExplosionFrame = 0;
			m_pNPCSprite->mVelocity = Vec2(0,0);
			return false;
		}
	}

	return true;
}

void CNonPlayer::NPCFire()
{
	for (int i = 1; i < BULLET_SPEED; i++)
	{
		m_pNPCSprite->mVelocity.y++;
	}
}	