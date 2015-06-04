#include "CLives.h"


Lives::Lives(const BackBuffer *pBackBuffer)
{
	m_lSprite = new Sprite("data/objects/life.bmp", RGB(0xff, 0x00, 0xff));
	m_lSprite->setBackBuffer(pBackBuffer);

	removed = false;
}

Lives::Lives(const BackBuffer *pBackBuffer, const char *szImageFile, COLORREF crTransparentColor)
{
	m_lSprite = new Sprite(szImageFile, crTransparentColor);
	m_lSprite->setBackBuffer(pBackBuffer);
}
Lives::~Lives()
{
	delete m_lSprite;
}

void Lives::Draw()
{
	m_lSprite->draw();
}

void Lives::Update(float dt)
{
	m_lSprite->update(dt);
}

Vec2& Lives::Position()
{
	return m_lSprite->mPosition;
}
Vec2& Lives::Velocity()
{
	return m_lSprite->mVelocity;
}
