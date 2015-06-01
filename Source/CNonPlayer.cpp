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
	//m_pNPCSprite = new Sprite("data/planeimg.bmp", "data/planemask.bmp");
	if (ID == 0)
	{
		m_pNPCSprite = new Sprite("data/characters/enemy.bmp", RGB(0xff,0x00,0xff));
	}
	if (ID == 1)
	{
		m_pNPCSprite = new Sprite("data/guns/bulletandmask.bmp", RGB(0xff,0x00,0xff));
	}

	m_pNPCSprite->setBackBuffer(pBackBuffer);
	
	// Animation frame crop rectangle
	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = 64;
	r.bottom = 64;

	m_pNPCExplosionSprite	= new AnimatedSprite("data/explosions/explosion.bmp", "data/explosions/explosionmask.bmp", r, 25);
	//m_pExplosionSprite	= new AnimatedSprite("data/explosionandmask.bmp", RGB(0xff,0x00,0xff), r, 16);
	m_pNPCExplosionSprite->setBackBuffer(pBackBuffer);
	m_bNPCExplosion			= false;
	m_iNPCExplosionFrame	= 0;

	m_pNPCSprite->isSpriteVisible = true; // Sprite este vizibil
	MoveTimer = 0;
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

void CNonPlayer::UpdateNPC(float dt)
{
	// Update sprite
	m_pNPCSprite->update(dt);
}

void CNonPlayer::DrawNPC()
{
	if(!m_bNPCExplosion)
		m_pNPCSprite->draw();
	else
		m_pNPCExplosionSprite->draw();
}

void CNonPlayer::NPCMove(CPlayer* Player, CMap* Map, CBomb* Bomb)
{
	// Crestem timer-ul
	MoveTimer++;

	// Daca timer-ul a ajuns egal cu NPC_SPEED
	if (MoveTimer == NPC_SPEED)
	{
		// Selectam o velocitate random
		SelectVelocity(Player, Map, Bomb);

		// Timer-ul este iar 0
		MoveTimer = 0;
	}

	// Verificam daca pozitia curenta a NPC-ului are o diferenta de BLOCKSIZE fata de cea anterioara
	// Astfel se creeaza pozitia decalata cu BLOCKSIZE a NPC-ului (mDecalPos)
	if ( (int)m_pNPCSprite->mPosition.x > (int)m_pNPCSprite->mDecalPos.x + BLOCKSIZE
		|| (int)(m_pNPCSprite->mPosition.y) > (int)(m_pNPCSprite->mDecalPos.y) + BLOCKSIZE
		|| (int)(m_pNPCSprite->mPosition.x) < (int)(m_pNPCSprite->mDecalPos.x) - BLOCKSIZE
		|| (int)(m_pNPCSprite->mPosition.y) < (int)(m_pNPCSprite->mDecalPos.y) - BLOCKSIZE
		)
	{
		// Setam velocitatea NPC-ului la 0 pentru ca a ajuns in centrul urmatorului block
		m_pNPCSprite->mVelocity = Vec2(0,0);

		// Pentru a nu aparea erori de pozitie, setam pozitia NPC-ului manual, odata ce acesta s-a oprit
		// Pozitia o calculam in functie de vechea pozitie a lui +/- BLOCKSIZE
		// Mai trebuie testat, pot aparea probleme la velocitati mai mari de BLOCKSIZE.
		// Erori precum: X = 392.14687 => X = 390.0 SAU X = 349.24454 => X = 350.0
		if ((int)m_pNPCSprite->mPosition.x > (int)m_pNPCSprite->mDecalPos.x + BLOCKSIZE)
			m_pNPCSprite->mPosition.x = (int)m_pNPCSprite->mOldPos.x + BLOCKSIZE;

		if ((int)(m_pNPCSprite->mPosition.y) > (int)(m_pNPCSprite->mDecalPos.y) + BLOCKSIZE)
			m_pNPCSprite->mPosition.y = (int)m_pNPCSprite->mOldPos.y + BLOCKSIZE;

		if ((int)(m_pNPCSprite->mPosition.x) < (int)(m_pNPCSprite->mDecalPos.x) - BLOCKSIZE)
			m_pNPCSprite->mPosition.x = (int)m_pNPCSprite->mOldPos.x - BLOCKSIZE;

		if ((int)(m_pNPCSprite->mPosition.y) < (int)(m_pNPCSprite->mDecalPos.y) - BLOCKSIZE)
			m_pNPCSprite->mPosition.y = (int)m_pNPCSprite->mOldPos.y - BLOCKSIZE;

		// Memoram pozitia decalata, care o sa fie egala cu cea curenta
		m_pNPCSprite->mDecalPos = m_pNPCSprite->mPosition;
	}

	// Daca distanta dintre NPC si jucator este mai mica sau egala decat 0, inseamna ca jucatorul a murit
	if (abs(m_pNPCSprite->mPosition.x - Player->Position().x) <= 0 && abs(m_pNPCSprite->mPosition.y - Player->Position().y) <= 0)
	{
		Player->get_m_pLives().back()->removed = true;
		Player->Killed();
		Player->ResetPosition = true;
	}
}

void CNonPlayer::SelectVelocity(CPlayer* Player, CMap* Map, CBomb* Bomb)
{
	// Pozitia veche a NPC-ului este pozitia NPC-ului inainte de a fi modificata de velocitate
	m_pNPCSprite->mOldPos = m_pNPCSprite->mPosition;

	// Stabilim distanta care se afla intre NPC si jucator
	DistanceToPlayer.x = abs(m_pNPCSprite->mPosition.x - Player->Position().x);
	DistanceToPlayer.y = abs(m_pNPCSprite->mPosition.y - Player->Position().y);

	// Daca jucatorul se afla la o distanta de NPC_FOLLOW_DISTANCE fata de NPC, inseamna ca NPC-ul o sa inceapa sa urmareasca pe jucator
	if (abs(m_pNPCSprite->mPosition.x - Player->Position().x) < NPC_FOLLOW_DISTANCE && abs(m_pNPCSprite->mPosition.y - Player->Position().y) < NPC_FOLLOW_DISTANCE)
	{
		// Selectam random ce o sa faca NPC-ul, pentru a oferii sanse jucatorului sa scape
		int select = rand() % 2 + 1;

		if (select == 1)
		{
			// Daca adaugam BLOCKSIZE la (pozitia curenta a NPC-ului) si facem diferenta dintre aceasta si (pozitia jucatorului),
			// stabilim pe unde trebuie sa se deplaseze NPC-ul astfel incat sa ajunga la jucator, stiind distanta dintre cei doi
			// Verificam in acelasi timp si daca s-a creat coliziune intre NPC si block-uri, pentru ca acesta sa nu se blocheze

			if ( abs( (m_pNPCSprite->mPosition.x+BLOCKSIZE) - Player->Position().x) < DistanceToPlayer.x 
				&& !NPCDetectColision(Map, Bomb, m_pNPCSprite->mPosition.x+BLOCKSIZE, m_pNPCSprite->mPosition.y)
				)
				m_pNPCSprite->mVelocity.x = 300;

			else if ( abs( (m_pNPCSprite->mPosition.x-BLOCKSIZE) - Player->Position().x) < DistanceToPlayer.x
				&& !NPCDetectColision(Map, Bomb, m_pNPCSprite->mPosition.x-BLOCKSIZE, m_pNPCSprite->mPosition.y)
				)
				m_pNPCSprite->mVelocity.x = -300;

			else if ( abs( (m_pNPCSprite->mPosition.y+BLOCKSIZE) - Player->Position().y) < DistanceToPlayer.y
				&& !NPCDetectColision(Map, Bomb, m_pNPCSprite->mPosition.x, m_pNPCSprite->mPosition.y+BLOCKSIZE)
				)
				m_pNPCSprite->mVelocity.y = 300;

			else if ( abs( (m_pNPCSprite->mPosition.y-BLOCKSIZE) - Player->Position().y) < DistanceToPlayer.y
				&& !NPCDetectColision(Map, Bomb, m_pNPCSprite->mPosition.x, m_pNPCSprite->mPosition.y-BLOCKSIZE)
				)
				m_pNPCSprite->mVelocity.y = -300;
		}
	}
	else
	{
		// Selectam o varianta random
		int select = rand() % 4 + 1;

		if (select == 1) // Velocitate dreapta
			m_pNPCSprite->mVelocity.x = 300;

		else if (select == 2) // Velocitate stanga
			m_pNPCSprite->mVelocity.x = -300;

		else if (select == 3) // Velocitate jos
			m_pNPCSprite->mVelocity.y = 300;

		else if (select == 4) // Velocitate sus
			m_pNPCSprite->mVelocity.y = -300;
	}
}

void CNonPlayer::NPCColision(CMap* Map, CBomb* Bomb)
{
	// Daca exista coliziune intre NPC, block si bomba
	if (NPCDetectColision(Map, Bomb, m_pNPCSprite->mPosition.x, m_pNPCSprite->mPosition.y))
	{
		// Setam pozitia NPC-ului la o pozitie anterioara si velocitatea pe 0
		m_pNPCSprite->mPosition.x = m_pNPCSprite->mOldPos.x;
		m_pNPCSprite->mPosition.y = m_pNPCSprite->mOldPos.y;
		m_pNPCSprite->mVelocity = Vec2(0,0);
	}
}

bool CNonPlayer::NPCDetectColision(CMap* Map, CBomb* Bomb, double PosX, double PosY)
{
	for (int id = 0; id < 3; id++)
	{
		for (int index = 0; index < Map->NrOfWalls[id]; index++)
		{
			if (id == 0) // wall
			{
				// Daca (pozitia NPC-ului - pozitia zidului) au o diferenta de (BLOCKSIZE-5), inseamna ca NPC-ul o sa intalneasca un zid
				// Avem (BLOCKSIZE-5) deoarece se creeaza un bug la unele margini si trebuie scazut 1 pixel
				if ((abs(PosX - Map->WallPosition(index).x) < BLOCKSIZE-5 && abs(Map->WallPosition(index).y - PosY) < BLOCKSIZE-5))
				{
					return true;
				} 
			}
			else if (id == 1) // indesctrutable
			{
				if ((abs(PosX - Map->IndesctructPosition(index).x) < BLOCKSIZE-5 && abs(Map->IndesctructPosition(index).y - PosY) < BLOCKSIZE-5))
				{
					return true;
				}
			}
			else if (id == 2) // destructable
			{
				
				if (((abs(PosX - Map->DesctructPosition(index).x) < BLOCKSIZE-5 && abs(Map->DesctructPosition(index).y - PosY) < BLOCKSIZE-5)) && Map->isDesctructVisible(index))
				{
					return true;
				}
			}
		}
	}

	if (Bomb != NULL)
		// Daca (pozitia NPC-ului - pozitia bombei) au o diferenta de (BLOCKSIZE-5), inseamna ca NPC-ul o sa intalneasca o bomba
		if ((abs(PosX - Bomb->BombPosition().x) < BLOCKSIZE-5 && abs(Bomb->BombPosition().y - PosY) < BLOCKSIZE-5))
			return true;

	// Daca nu s-au facut coliziuni, returnam fals
	return false;
}

Vec2& CNonPlayer::NPCPosition()
{
	return m_pNPCSprite->mPosition;
}

Vec2& CNonPlayer::NPCOldPosition()
{
	return m_pNPCSprite->mOldPos;
}

Vec2& CNonPlayer::NPCDecalPosition()
{
	return m_pNPCSprite->mDecalPos;
}

Vec2& CNonPlayer::NPCVelocity()
{
	return m_pNPCSprite->mVelocity;
}

bool& CNonPlayer::Visible()
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