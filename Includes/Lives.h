#ifndef _LIVES_H_
#define _LIVES_H_

#include "Main.h"
#include "Sprite.h"

class Lives
{
public:
	Lives(const BackBuffer *pBackBuffer);
	Lives(const BackBuffer *pBackBuffer, const char *szImageFile, COLORREF crTransparentColor);
	virtual ~Lives();

	void			Update(float dt);
	void			Draw();
	Vec2&			Position();
	Vec2&			Velocity();
	int				Width(){ return m_lSprite->width(); }
	int				Height(){ return m_lSprite->height(); }
	bool			removed=false;

protected:
	Sprite*			m_lSprite;

};

#endif