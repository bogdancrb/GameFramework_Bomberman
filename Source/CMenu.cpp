//-----------------------------------------------------------------------------
// File: CMenu.cpp
//
// Desc: Map loading 
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CMenu Specific Includes
//-----------------------------------------------------------------------------
#include "CMenu.h"

MainMenu::MainMenu(BackBuffer* pBackBuffer, USHORT w, USHORT h)
{
	m_Menu = new Sprite("data/menu/background_menu.bmp", RGB(0xff, 0x00, 0xff));
	m_Menu->setBackBuffer(pBackBuffer);

	m_Play = new Sprite("data/menu/play.bmp", RGB(0xFF, 0x00, 0xFF));
	m_Play->setBackBuffer(pBackBuffer);

	m_HighScores = new Sprite("data/menu/highscores.bmp", RGB(0xFF, 0x00, 0xFF));
	m_HighScores->setBackBuffer(pBackBuffer);

	m_Quit = new Sprite("data/menu/quit.bmp", RGB(0xFF, 0x00, 0xFF));
	m_Quit->setBackBuffer(pBackBuffer);
	
	m_Menu->mPosition = Vec2(w / 2, h / 2); // Pozitie background
	m_Play->mPosition = Vec2(520,430);
	m_HighScores->mPosition = Vec2(520,520);
	m_Quit->mPosition = Vec2(520,600);

	m_Active = true;
}

void MainMenu::Draw()
{
	m_Menu->draw();
	m_Play->draw();
	m_HighScores->draw();
	m_Quit->draw();
}

//----------------------------------------------[InGameMenu class] -------------------------------------------------

HighScoreMenu::HighScoreMenu(BackBuffer* pBackBuffer, USHORT w, USHORT h)
{
	m_GameOverMenu = new Sprite("data/menu/gameover_menu.bmp", RGB(0xff, 0x00, 0xff));
	m_GameOverMenu->setBackBuffer(pBackBuffer);

	m_ScoreMenu = new Sprite("data/menu/highscores_menu.bmp", RGB(0xFF, 0x00, 0xFF));
	m_ScoreMenu->setBackBuffer(pBackBuffer);
	
	m_GameOverMenu->mPosition = Vec2(w / 2, h / 2);
	m_ScoreMenu->mPosition = Vec2(w / 2, h / 2);
	
	m_ActiveGameOver = false;
	m_ActiveScore = false;
}

void HighScoreMenu::Draw()
{
	if (m_ActiveGameOver)
		m_GameOverMenu->draw();

	if (m_ActiveScore)
		m_ScoreMenu->draw();
}

//----------------------------------------------[InGameMenu class] -------------------------------------------------

InGameMenu::InGameMenu(BackBuffer* pBackBuffer, USHORT w, USHORT h)
{
	pause_button = new Sprite("data/menu/pause.bmp", RGB(0xFF, 0x00, 0xFF));
	pause_button->setBackBuffer(pBackBuffer);

	popup_menu= new Sprite("data/menu/popup_menu.bmp", RGB(0xFF, 0x00, 0xFF));
	popup_menu->setBackBuffer(pBackBuffer);

	pause_button->mPosition = Vec2(1350,15);
	popup_menu->mPosition = Vec2(w / 2, h / 2);

	m_Active = false;
}

void InGameMenu::Draw()
{
	pause_button->draw();
	
	if (m_Active)
	{
		popup_menu->draw();
	}
}