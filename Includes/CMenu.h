//-----------------------------------------------------------------------------
// File: CMenu.h
//
// Desc: This file stores the menu objects
//
//-----------------------------------------------------------------------------

#ifndef _CMENU_H_
#define _CMENU_H_

//-----------------------------------------------------------------------------
// CMenu Specific Includes
//-----------------------------------------------------------------------------
#include "Main.h"
#include "Sprite.h"
#include "BackBuffer.h"

//-----------------------------------------------------------------------------
// Main Class Definitions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CMenu (Class)
// Desc : Menu class handles all menu manipulation, update and management.
//-----------------------------------------------------------------------------
class MainMenu
{
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
	MainMenu(BackBuffer* pBackBuffer, USHORT w, USHORT h);
	//~MainMenu();

	//-------------------------------------------------------------------------
	// Public Functions for This Class.
	//-------------------------------------------------------------------------
	void				Draw();

private:
	//-------------------------------------------------------------------------
	// Private Variables for This Class.
	//-------------------------------------------------------------------------
	Sprite*				m_Play;
	Sprite*				m_Options;
	Sprite*				m_Credits;
	Sprite*				m_Quit;
	Sprite*				m_HighScores;

public:
	//-------------------------------------------------------------------------
	// Keep these public because they need to be modified externally.
	//-------------------------------------------------------------------------
	Sprite*				m_Menu;
	bool				m_Active;
};

class HighScoreMenu
{
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
	HighScoreMenu(BackBuffer* pBackBuffer, USHORT w, USHORT h);
	//~MainMenu();

	//-------------------------------------------------------------------------
	// Public Functions for This Class.
	//-------------------------------------------------------------------------
	void				Draw();

private:
	//-------------------------------------------------------------------------
	// Private Variables for This Class.
	//-------------------------------------------------------------------------
	Sprite*				m_GameOverMenu;
	Sprite*				m_ScoreMenu;

public:
	//-------------------------------------------------------------------------
	// Keep these public because they need to be modified externally.
	//-------------------------------------------------------------------------
	bool				m_ActiveGameOver;
	bool				m_ActiveScore;
};

class InGameMenu
{
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
	InGameMenu(BackBuffer* pBackBuffer, USHORT w, USHORT h);
	//~InGameMenu();

	//-------------------------------------------------------------------------
	// Public Functions for This Class.
	//-------------------------------------------------------------------------
	void				Draw();

private:
	//-------------------------------------------------------------------------
	// Private Variables for This Class.
	//-------------------------------------------------------------------------
	Sprite*				pause_button;
	Sprite*				popup_menu;

public:
	//-------------------------------------------------------------------------
	// Keep these public because they need to be modified externally.
	//-------------------------------------------------------------------------
	bool				m_Active;
};

#endif // _CMENU_H_