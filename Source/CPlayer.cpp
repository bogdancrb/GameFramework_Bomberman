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
	if (ID == 0)
	{
		m_pSprite = new Sprite("data/characters/player.bmp", RGB(0xff,0x00,0xff));
	}
	if (ID == 1)
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

	// Jucatorul se poate misca
	m_pCanMove = true;

	//m_pPoints = 0;
	//m_pHealth = 100;
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
	// Verificam daca pozitia curenta a jucatorului are o diferenta de BLOCKSIZE fata de cea anterioara
	// Astfel se creeaza pozitia decalata cu BLOCKSIZE a jucatorului (PlayerDecalPos)
	if ( (int)m_pSprite->mPosition.x > (int)m_pSprite->PlayerDecalPos.x + BLOCKSIZE
		|| (int)(m_pSprite->mPosition.y) > (int)(m_pSprite->PlayerDecalPos.y) + BLOCKSIZE
		|| (int)(m_pSprite->mPosition.x) < (int)(m_pSprite->PlayerDecalPos.x) - BLOCKSIZE
		|| (int)(m_pSprite->mPosition.y) < (int)(m_pSprite->PlayerDecalPos.y) - BLOCKSIZE
		)
	{
		// Setam velocitatea jucatorului la 0 pentru ca a ajuns in centrul urmatorului block
		m_pSprite->mVelocity = Vec2(0,0);

		// Pentru a nu aparea erori de pozitie, setam pozitia jucatorului manual, odata ce acesta s-a oprit
		// Pozitia o calculam in functie de vechea pozitie a lui +/- BLOCKSIZE
		// Mai trebuie testat, pot aparea probleme la velocitati mai mari de BLOCKSIZE.
		// Erori precum: X = 392.14687 => X = 390.0 SAU X = 349.24454 => X = 350.0
		if ((int)m_pSprite->mPosition.x > (int)m_pSprite->PlayerDecalPos.x + BLOCKSIZE)
			m_pSprite->mPosition.x = (int)m_pSprite->PlayerOldPos.x + BLOCKSIZE;

		if ((int)(m_pSprite->mPosition.y) > (int)(m_pSprite->PlayerDecalPos.y) + BLOCKSIZE)
			m_pSprite->mPosition.y = (int)m_pSprite->PlayerOldPos.y + BLOCKSIZE;

		if ((int)(m_pSprite->mPosition.x) < (int)(m_pSprite->PlayerDecalPos.x) - BLOCKSIZE)
			m_pSprite->mPosition.x = (int)m_pSprite->PlayerOldPos.x - BLOCKSIZE;

		if ((int)(m_pSprite->mPosition.y) < (int)(m_pSprite->PlayerDecalPos.y) - BLOCKSIZE)
			m_pSprite->mPosition.y = (int)m_pSprite->PlayerOldPos.y - BLOCKSIZE;

		// Memoram pozitia decalata, care o sa fie egala cu cea curenta (verificati F1 in joc pentru exemplu)
		m_pSprite->PlayerDecalPos = m_pSprite->mPosition;

		// Acum jucatorul se poate misca in alte directii
		m_pCanMove = true;
	}

	// Deplasam jucatorul cu cate o casuta, in centrul acesteia
	// O casuta are dimensiunea BLOCKSIZE x BLOCKSIZE
	if( ulDirection & CPlayer::DIR_LEFT )
		m_pSprite->mVelocity.x = -100;

	if( ulDirection & CPlayer::DIR_RIGHT )
		m_pSprite->mVelocity.x = 100;

	if( ulDirection & CPlayer::DIR_FORWARD )
		m_pSprite->mVelocity.y = -100;

	if( ulDirection & CPlayer::DIR_BACKWARD )
		m_pSprite->mVelocity.y = 100;
}

bool& CPlayer::CanMove()
{
	return m_pCanMove; // Returnam daca jucatorul se poate misca sau nu
}

Vec2& CPlayer::Position()
{
	return m_pSprite->mPosition;
}

Vec2& CPlayer::PlayerDecalPos()
{
	return m_pSprite->PlayerDecalPos; // Accesam pozitia decalata a jucatorului
}

Vec2& CPlayer::PlayerOldPos()
{
	return m_pSprite->PlayerOldPos; // Accesam pozitia veche a jucatorului
}

Vec2& CPlayer::Velocity()
{
	return m_pSprite->mVelocity;
}

void CPlayer::PlaceBomb(CBomb** Bomb, BackBuffer* BBuffer)
{
	// Daca bomba nu a fost deja creata, o incarcam in memorie
	if ((*Bomb) == NULL)
		*Bomb = new CBomb(BBuffer);

	// Daca bomba a fost activata si jucatorul se poate misca (pentru a nu pune bomba aiurea)
	if (!(*Bomb)->m_BombIsActive && m_pCanMove == true)
	{
		// Setam pozitia bombei pe pozitia jucatorului si o marcam ca fiind activa
		(*Bomb)->BombPosition() = m_pSprite->mPosition;
		(*Bomb)->m_BombIsActive = true;
	}
}

void CPlayer::Explode(int ID)
{
	m_pExplosionSprite->mPosition = m_pSprite->mPosition;
	m_pExplosionSprite->SetFrame(0);
	//PlaySound("data/explosion.wav", NULL, SND_FILENAME | SND_ASYNC);
	m_bExplosion = true;
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