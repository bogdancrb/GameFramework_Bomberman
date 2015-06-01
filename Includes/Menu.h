#pragma once

#include "Main.h"
#include "Sprite.h"
#include "BackBuffer.h"

class MainMenu
{
private:
	Sprite*				m_Play;
	Sprite*				m_Options;
	Sprite*				m_Credits;
	Sprite*				m_Quit;

	BackBuffer*         m_Buffer;
	USHORT				m_Width;
	USHORT				m_Height;

public:
	MainMenu(BackBuffer* buffer, USHORT w, USHORT h);
	void Draw();
	bool				m_Active;

	Sprite*		m_Menu;
};

class InGameMenu
{
private:
	Sprite*  In_game_Menu_button;
	Sprite*	 Popup_menu;
	BackBuffer*         m_Buffer;
	USHORT				m_Width;
	USHORT				m_Height;
public:
	InGameMenu(BackBuffer* buffer, USHORT w, USHORT h);
	void Draw();
	bool	m_S_M_Active;

};