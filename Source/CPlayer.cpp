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

	m_pPoints = 0;
	ResetPosition = false;

	is_dead = false;
	//initial playerul are 3 vieti
	no_lives = 3;
	for (unsigned int i = 0; i < get_no_lives(); i++)
	{
		this->m_pLives.push_back(new Lives(pBackBuffer));
	}
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

void CPlayer::Update(float dt)
{
	// Update sprite
	m_pSprite->update(dt);

	int space = 1366 - 135;

	for (vector<Lives*>::iterator life = m_pLives.begin(); life != m_pLives.end(); ++life)
	{
		if (!(*life)->removed)
		{
			(*life)->Position().x = space;
			(*life)->Position().y = this->Height();
			space -= 2*this->Width();
		}
		(*life)->Update(dt);
	}
}

void CPlayer::Draw()
{
	if (!m_bExplosion)
	{
		m_pSprite->draw();
	}
	else
	{
		m_pExplosionSprite->draw();
	}

	for (vector<Lives*>::iterator life = m_pLives.begin(); life != m_pLives.end(); ++life)
	{
		(*life)->Draw();
	}
}

void CPlayer::Move(ULONG ulDirection, int ID)
{
	// Deplasam jucatorul cu cate o casuta, in centrul acesteia
	// O casuta are dimensiunea BLOCKSIZE x BLOCKSIZE
	if( ulDirection & CPlayer::DIR_LEFT )
		m_pSprite->mVelocity.x = -(BLOCKSIZE + 40);

	if( ulDirection & CPlayer::DIR_RIGHT )
		m_pSprite->mVelocity.x = BLOCKSIZE + 40;

	if( ulDirection & CPlayer::DIR_FORWARD )
		m_pSprite->mVelocity.y = -(BLOCKSIZE + 40);

	if( ulDirection & CPlayer::DIR_BACKWARD )
		m_pSprite->mVelocity.y = BLOCKSIZE + 40;

	// Verificam daca pozitia curenta a jucatorului are o diferenta de BLOCKSIZE fata de cea anterioara
	// Astfel se creeaza pozitia decalata cu BLOCKSIZE a jucatorului (mDecalPos)
	if ( (int)m_pSprite->mPosition.x > (int)m_pSprite->mDecalPos.x + BLOCKSIZE
		|| (int)(m_pSprite->mPosition.y) > (int)(m_pSprite->mDecalPos.y) + BLOCKSIZE
		|| (int)(m_pSprite->mPosition.x) < (int)(m_pSprite->mDecalPos.x) - BLOCKSIZE
		|| (int)(m_pSprite->mPosition.y) < (int)(m_pSprite->mDecalPos.y) - BLOCKSIZE
		)
	{
		// Setam velocitatea jucatorului la 0 pentru ca a ajuns in centrul urmatorului block
		m_pSprite->mVelocity = Vec2(0,0);

		// Pentru a nu aparea erori de pozitie, setam pozitia jucatorului manual, odata ce acesta s-a oprit
		// Pozitia o calculam in functie de vechea pozitie a lui +/- BLOCKSIZE
		// Mai trebuie testat, pot aparea probleme la velocitati mai mari de BLOCKSIZE.
		// Erori precum: X = 392.14687 => X = 390.0 SAU X = 349.24454 => X = 350.0
		if ((int)m_pSprite->mPosition.x > (int)m_pSprite->mDecalPos.x + BLOCKSIZE)
			m_pSprite->mPosition.x = (int)m_pSprite->mOldPos.x + BLOCKSIZE;

		if ((int)(m_pSprite->mPosition.y) > (int)(m_pSprite->mDecalPos.y) + BLOCKSIZE)
			m_pSprite->mPosition.y = (int)m_pSprite->mOldPos.y + BLOCKSIZE;

		if ((int)(m_pSprite->mPosition.x) < (int)(m_pSprite->mDecalPos.x) - BLOCKSIZE)
			m_pSprite->mPosition.x = (int)m_pSprite->mOldPos.x - BLOCKSIZE;

		if ((int)(m_pSprite->mPosition.y) < (int)(m_pSprite->mDecalPos.y) - BLOCKSIZE)
			m_pSprite->mPosition.y = (int)m_pSprite->mOldPos.y - BLOCKSIZE;

		// Memoram pozitia decalata, care o sa fie egala cu cea curenta (verificati F1 in joc pentru exemplu)
		m_pSprite->mDecalPos = m_pSprite->mPosition;

		// Acum jucatorul se poate misca in alte directii
		m_pCanMove = true;
	}
}

void CPlayer::PlayerColision(CMap* Map)
{
	for (int id = 0; id < 3; id++)
	{
		for (int index = 0; index < Map->NrOfWalls[id]; index++)
		{
			if (id == 0) // wall
			{
				// Daca (pozitia jucatorului - pozitia zidului) au o diferenta de (BLOCKSIZE-5), inseamna ca jucatorul o sa intalneasca un zid
				// Avem (BLOCKSIZE-5) deoarece se creeaza un bug la unele margini si trebuie scazut 1 pixel
				if ((abs(m_pSprite->mPosition.x - Map->WallPosition(index).x) < BLOCKSIZE-5 && abs(Map->WallPosition(index).y - m_pSprite->mPosition.y) < BLOCKSIZE-5))
				{
					// Setam pozitia jucatorului la o pozitie anterioara si velocitatea pe 0, deoarece acesta a intalnit un zid
					m_pSprite->mPosition.x = m_pSprite->mOldPos.x;
					m_pSprite->mPosition.y = m_pSprite->mOldPos.y;
					m_pSprite->mVelocity = Vec2(0,0);
					m_pCanMove = true; // Jucatorul se poate misca iar
				} 
			}
			else if (id == 1) // indesctrutable
			{
				if ((abs(m_pSprite->mPosition.x - Map->IndesctructPosition(index).x) < BLOCKSIZE-5 && abs(Map->IndesctructPosition(index).y - m_pSprite->mPosition.y) < BLOCKSIZE-5))
				{
					m_pSprite->mPosition.x = m_pSprite->mOldPos.x;
					m_pSprite->mPosition.y = m_pSprite->mOldPos.y; 
					m_pSprite->mVelocity = Vec2(0,0);
					m_pCanMove = true;
				}
			}
			else if (id == 2) // destructable
			{
				
				if (((abs(m_pSprite->mPosition.x - Map->DesctructPosition(index).x) < BLOCKSIZE-5 && abs(Map->DesctructPosition(index).y - m_pSprite->mPosition.y) < BLOCKSIZE-5)) && Map->isDesctructVisible(index))
				{
					m_pSprite->mPosition.x = m_pSprite->mOldPos.x;
					m_pSprite->mPosition.y = m_pSprite->mOldPos.y;
					m_pSprite->mVelocity = Vec2(0,0);
					m_pCanMove = true;
				}
			}
		}
	}
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
	return m_pSprite->mDecalPos; // Accesam pozitia decalata a jucatorului
}

Vec2& CPlayer::PlayerOldPos()
{
	return m_pSprite->mOldPos; // Accesam pozitia veche a jucatorului
}

Vec2& CPlayer::Velocity()
{
	return m_pSprite->mVelocity;
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
			return false;
		}
	}

	return true;
}

void CPlayer::Killed()
{
	for (int i = m_pLives.size(); i >0; i--)
	{
		//Lives* life = this->m_pLives.at(i);
		if (!m_pLives[i - 1]->removed == false)
		{
			m_pLives[i-1]->removed = true;
			//m_pLives[i-1]->Position().y = -2000;
			delete m_pLives[i-1];
			m_pLives[i-1] = NULL;
			m_pLives.erase(m_pLives.begin() + i - 1);
		}
	}
	if (m_pLives.size() == 0)
	{
		is_dead = true;
		//PostQuitMessage(0);
	}
}