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

void CBomb::UpdateBomb(float dt)
{
	// Update sprite
	m_pBombSprite->update(dt);
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

void CBomb::BombExplode(CMap* Map, int ID)
{
	//-------------------------------------------------------------------
	// Forma explozie cu vectori
	// Daca EXPLOSION_RANGE = 13 avem:
	//								  Y:
	//								  12
	//								  10
	//								   8
	//					X:		5 3 1 (0) 2 4 6  
	//							       7
	//								   9
	//								  11
	// Unde numerele reprezinta indicii din vector m_pBombExplosionSprite 
	//-------------------------------------------------------------------

	int k = 0; // Folosim o variabila pentru a memora pozitiile bombei fata de jucator
	bool colisionPareX = false, colisionImpareX = false, colisionPareY = false, colisionImpareY = false;
	
	m_pBombExplosionSprite[0]->mPosition = m_pBombSprite->mPosition; // Prima explozie are loc pe centru (pe pozitia bombei)

	for (int index = 1; index <= (EXPLOSION_RANGE-1)/2; index+=2)
	{
		k += BLOCKSIZE; // Setam viitoarele coordonate in functie de BLOCKSIZE (60, 120, 180 etc.)

		// Daca coliziunea nu a avut loc pe axa -X
		if (!colisionImpareX)
		{
			if (!colisionImpareX)
				// Setam o parte din explozii sa aiba loc pe axa -X
				m_pBombExplosionSprite[index]->mPosition = m_pBombSprite->mPosition + Vec2(-k,0);

			// Daca exista coliziune intre bomba si harta
			if (BombColision(m_pBombSprite->mPosition + Vec2(-k,0), Map) == true)
				// Setam coliziune pe axa -X, pentru a nu incarca urmatoarele explozii
				// Ex: 1 => nu mai au loc nici exploziile 3 si 5
				colisionImpareX = true;
		}

		// Daca coliziunea nu a avut loc pe axa +X
		if (!colisionPareX)
		{
			if (!colisionPareX)
				// Setam o parte din explozii sa aiba loc pe axa +X
				m_pBombExplosionSprite[index+1]->mPosition = m_pBombSprite->mPosition + Vec2(k,0);

			// Daca exista coliziune intre bomba si harta
			if (BombColision(m_pBombSprite->mPosition + Vec2(k,0), Map) == true)
				// Setam coliziune pe axa +X, pentru a nu incarca urmatoarele explozii
				// Ex: 2 => nu mai au loc nici exploziile 4 si 6
				colisionPareX = true;
		}

		// Daca coliziunea nu a avut loc pe axa -Y
		if (!colisionImpareY)
		{
			if (!colisionImpareY)
				// Setam o parte din explozii sa aiba loc pe axa -Y
				m_pBombExplosionSprite[index+(EXPLOSION_RANGE)/2]->mPosition = m_pBombSprite->mPosition + Vec2(0,-k);

			// Daca exista coliziune intre bomba si harta
			if (BombColision(m_pBombSprite->mPosition + Vec2(0,-k), Map) == true)
				// Setam coliziune pe axa -Y, pentru a nu incarca urmatoarele explozii
				// Ex: 7 => nu mai au loc nici exploziile 9 si 11
				colisionImpareY = true;
		}

		// Daca coliziunea nu a avut loc pe axa +Y
		if (!colisionPareY)
		{
			if (!colisionPareY)
				// Setam o parte din explozii sa aiba loc pe axa +Y
				m_pBombExplosionSprite[index+(EXPLOSION_RANGE+1)/2]->mPosition = m_pBombSprite->mPosition + Vec2(0,k);

			// Daca exista coliziune intre bomba si harta
			if (BombColision(m_pBombSprite->mPosition + Vec2(0,k), Map) == true)
				// Setam coliziune pe axa +Y, pentru a nu incarca urmatoarele explozii
				// Ex: 8 => nu mai au loc nici exploziile 10 si 12
				colisionPareY = true;
		}
	}

	m_bBombExplosion = true;
}

bool CBomb::BombColision(Vec2 Position, CMap* Map)
{
	for (int id = 1; id < 3; id++)
	{
		for (int index = 0; index < Map->NrOfWalls[id]; index++)
		{
			if (id == 1) // indestructable
			{
				// Daca pozitia exploziei este egala cu pozitia unui block indestructibil, returnam true pentru coliziune
				if (Position == Map->IndesctructPosition(index))
				{
					return true;
				}
			}

			if (id == 2) // destructable
			{
				// Daca pozitia exploziei este egala cu pozitia unui block destructibil si block-ul este vizibil (nu a fost distrus)
				// Returnam true pentru coliziune si setam block-ul respectiv ca fiind invizibil
				if (Position == Map->DesctructPosition(index) && Map->isDesctructVisible(index))
				{
					Map->isDesctructVisible(index) = false;
					return true;
				}
			}
		}
	}

	// Daca nu s-au facut coliziuni, returnam fals
	return false;
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