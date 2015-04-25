//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//
// Desc: This file stores the player object class. This class performs tasks
//       such as player movement, some minor physics as well as rendering.
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CPlayer Specific Includes
//-----------------------------------------------------------------------------
#include "CPlayer.h"

//-----------------------------------------------------------------------------
// Name : CPlayer () (Constructor)
// Desc : CPlayer Class Constructor
//-----------------------------------------------------------------------------
CPlayer::CPlayer(const BackBuffer *pBackBuffer, int ID)
{
	//m_pSprite = new Sprite("data/planeimg.bmp", "data/planemask.bmp");
	if (ID == 1)
	{
		m_pSprite = new Sprite("data/characters/player.bmp", RGB(0xff,0x00,0xff));
	}
	if (ID == 2)
	{
		m_pSprite = new Sprite("data/guns/bulletandmask.bmp", RGB(0xff,0x00,0xff));
	}

	m_pSprite->setBackBuffer(pBackBuffer);
	m_eSpeedState = SPEED_STOP;
	m_fTimer = 0;

	// Animation frame crop rectangle
	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = 64;
	r.bottom = 64;

	m_pExplosionSprite	= new AnimatedSprite("data/explosions/explosion.bmp", "data/explosions/explosionmask.bmp", r, 25);
	//m_pExplosionSprite	= new AnimatedSprite("data/explosionandmask.bmp", RGB(0xff,0x00,0xff), r, 16);
	m_pExplosionSprite->setBackBuffer(pBackBuffer);
	m_bExplosion		= false;
	m_iExplosionFrame	= 0;

	m_pPoints = 0;
	m_pHealth = 100;
}

//-----------------------------------------------------------------------------
// Name : ~CPlayer () (Destructor)
// Desc : CPlayer Class Destructor
//-----------------------------------------------------------------------------
CPlayer::~CPlayer()
{
	delete m_pSprite;
	delete m_pExplosionSprite;
}

void CPlayer::Update(float dt, int ID)
{
	// Update sprite
	m_pSprite->update(dt,ID);


	// Get velocity
	double v = m_pSprite->mVelocity.Magnitude();

	if (ID == 1)
	{
		// NOTE: for each async sound played Windows creates a thread for you
		// but only one, so you cannot play multiple sounds at once.
		// This creation/destruction of threads also leads to bad performance
		// so this method is not recommanded to be used in complex projects.

		// update internal time counter used in sound handling (not to overlap sounds)
		m_fTimer += dt;

		// A FSM is used for sound manager 
		switch(m_eSpeedState)
		{
		case SPEED_STOP:
			if(v > 35.0f)
			{
				m_eSpeedState = SPEED_START;
				PlaySound("data/sounds/jet-start.wav", NULL, SND_FILENAME | SND_ASYNC);
				m_fTimer = 0;
			}
			break;
		case SPEED_START:
			if(v < 25.0f)
			{
				if (m_bExplosion != true)
				{
					m_eSpeedState = SPEED_STOP;
					PlaySound("data/sounds/jet-stop.wav", NULL, SND_FILENAME | SND_ASYNC);
					m_fTimer = 0;
				}
			}
			else
				if(m_fTimer > 1.f)
				{
					PlaySound("data/sounds/jet-cabin.wav", NULL, SND_FILENAME | SND_ASYNC);
					m_fTimer = 0;
				}
			break;
		}	
	}

	// NOTE: For sound you also can use MIDI but it's Win32 API it is a bit hard
	// see msdn reference: http://msdn.microsoft.com/en-us/library/ms711640.aspx
	// In this case you can use a C++ wrapper for it. See the following article:
	// http://www.codeproject.com/KB/audio-video/midiwrapper.aspx (with code also)
}

void CPlayer::Draw()
{
	if(!m_bExplosion)
		m_pSprite->draw();
	else
		m_pExplosionSprite->draw();
}

void CPlayer::Move(ULONG ulDirection, int ID)
{
	if (m_bExplosion != true)
	{
		if( ulDirection & CPlayer::DIR_LEFT )
		{
			if (ID == 1 && strcmp(m_pSprite->fileName,"data/characters/player.bmp") != 0) // pentru a evita pierderi de fps
			{
				m_pSprite->LoadSprite("data/characters/player.bmp", RGB(0xff,0x00,0xff));
			}
			m_pSprite->mVelocity.x -= .8;
		}
		else if (strcmp(m_pSprite->fileName,"data/characters/player.bmp") == 0 || strcmp(m_pSprite->fileName,"data/characters/player.bmp") == 0) // pentru a evita ships de fps si pentru a revnii la pozitia normala
		{
			if (ID == 1)
			{
				m_pSprite->LoadSprite("data/characters/player.bmp", RGB(0xff,0x00,0xff));
			}
		}

		if( ulDirection & CPlayer::DIR_RIGHT )
		{
			if (ID == 1 && strcmp(m_pSprite->fileName,"data/characters/player.bmp") != 0) // pentru a evita pierderi de fps
			{
				m_pSprite->LoadSprite("data/characters/player.bmp", RGB(0xff,0x00,0xff));
			}
			m_pSprite->mVelocity.x += .8;
		}

		if( ulDirection & CPlayer::DIR_FORWARD )
			m_pSprite->mVelocity.y -= .8;

		if( ulDirection & CPlayer::DIR_BACKWARD )
			m_pSprite->mVelocity.y += .8;
	}
}

Vec2& CPlayer::Position()
{
	return m_pSprite->mPosition;
}

Vec2& CPlayer::Velocity()
{
	return m_pSprite->mVelocity;
}

//int CObject::Height()
//{
//	return m_pObjSprite->height();
//}
//
//int CObject::Width()
//{
//	return m_pObjSprite->width();
//}

void CPlayer::Explode(int ID)
{
	if (ID == 1)
	{
		m_pExplosionSprite->mPosition = m_pSprite->mPosition;
		m_pExplosionSprite->SetFrame(0);
		m_bExplosion = true;
		PlaySound("data/sounds/explosion.wav", NULL, SND_FILENAME | SND_ASYNC);
	}

	m_pSprite->mVelocity.x = m_pSprite->mVelocity.y = 0;
	Position() = Vec2((int)GetSystemMetrics(SM_CXSCREEN)-700, (int)GetSystemMetrics(SM_CYSCREEN)-200);
}

bool CPlayer::AdvanceExplosion()
{
	if(m_bExplosion)
	{
		m_pExplosionSprite->SetFrame(m_iExplosionFrame++);
		if(m_iExplosionFrame==m_pExplosionSprite->GetFrameCount())
		{
			m_bExplosion = false;
			m_iExplosionFrame = 0;
			m_pSprite->mVelocity = Vec2(0,0);
			m_eSpeedState = SPEED_STOP;
			return false;
		}
	}

	return true;
}

void CPlayer::Fire()
{
	for (int i = 1; i < BULLET_SPEED; i++)
	{
		m_pSprite->mVelocity.y--;
	}
}	