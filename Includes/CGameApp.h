//-----------------------------------------------------------------------------
// File: CGameApp.h
//
// Desc: Game Application class, this is the central hub for all app processing
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

#ifndef _CGAMEAPP_H_
#define _CGAMEAPP_H_

//-----------------------------------------------------------------------------
// CGameApp Specific Includes
//-----------------------------------------------------------------------------
#include "Main.h"
#include "CTimer.h"
#include "CPlayer.h"
#include "CObject.h"
#include "CNonPlayer.h"
#include "CBomb.h"
#include "CMap.h"
#include "BackBuffer.h"
#include "ImageFile.h"
#include "CMenu.h"

//-----------------------------------------------------------------------------
// Forward Declarations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CGameApp (Class)
// Desc : Central game engine, initialises the game and handles core processes.
//-----------------------------------------------------------------------------
class CGameApp
{
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
			 CGameApp();
	virtual ~CGameApp();

	//-------------------------------------------------------------------------
	// Public Functions for This Class
	//-------------------------------------------------------------------------
	LRESULT		DisplayWndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam );
	bool		InitInstance( LPCTSTR lpCmdLine, int iCmdShow );
	int			BeginGame( );
	bool		ShutDown( );
	
private:
	//-------------------------------------------------------------------------
	// Private Functions for This Class
	//-------------------------------------------------------------------------
	bool		BuildObjects	  ( );
	void		ReleaseObjects	( int dontDeleteBuff = 0 );
	void		FrameAdvance	  ( );
	bool		CreateDisplay	 ( );
	void		ChangeDevice	  ( );
	void		SetupGameState	( );
	void		AnimateObjects	( );
	void		DrawObjects	   ( );
	void		DrawInfo		( );
	void		ProcessInput	  ( );
	void		ProcessMenuButtons   ( );
	void		CheckBombs		( ); // Verificam daca bombele sunt active sau nu, plus coliziunea cu explozia
	void		NPCStartingPosition ( ); // Pentru a stabilii pozitia de inceput a NPC-ului
	void		CheckPositions	( ); // Verificam diferite pozitii legate de jucator
	bool		LoadLevel		 ( ); // Incarcarea nivelurilor
	void		LoadGame		( ); // Incarcarea jocului din fisier
	void		SaveGame	  ( ); // Salvarea jocului in fisier

	//-------------------------------------------------------------------------
	// Private Static Functions For This Class
	//-------------------------------------------------------------------------
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	//-------------------------------------------------------------------------
	// Private Variables For This Class
	//-------------------------------------------------------------------------
	CTimer					m_Timer;			// Game timer
	ULONG					m_LastFrameRate;	// Used for making sure we update only when fps changes.
	
	HWND					m_hWnd;			 // Main window HWND
	HICON					m_hIcon;			// Window Icon
	HMENU					m_hMenu;			// Window Menu
	
	bool					m_bActive;		  // Is the application active ?

	ULONG					m_nViewX;		   // X Position of render viewport
	ULONG					m_nViewY;		   // Y Position of render viewport
	ULONG					m_nViewWidth;	   // Width of render viewport
	ULONG					m_nViewHeight;	  // Height of render viewport

	POINT					m_OldCursorPos;	 // Old cursor position for tracking
	HINSTANCE				m_hInstance;

	CImageFile				m_imgBackground;

	BackBuffer*				m_pBBuffer;
	CPlayer*				m_pPlayer;
	vector<CNonPlayer*>		m_pNPC;
	CObject*				m_pCratesAndBombs[MAX_CRATE];

	CMap*					m_Map;	// Harta cu tot cu obiecte si coliziuni ziduri
	CMap*					m_BonusMap;	// Harta bonus cu tot cu obiecte si coliziuni ziduri
	CMap*					m_BossMap;	// Harta cu tot cu obiecte si coliziunia ziduri

	vector<CBomb*>			m_pBomb;
	
	bool					F1Pressed;		// Pentru a putea desena pe ecran informatii precum FPS, etc.

	MainMenu*				m_MMenu;
	InGameMenu*				m_SMenu;
	HighScoreMenu*			m_HighMenu;
	
	int						m_LoadGameLevel; // Folosim pentru a trece dintr-un nivel in altul
	int						m_CurrentGameLevel; // Folosim pentru a memora nivelul principal
	long int				SavedPlayerPoints;

	int						AnimationID;

	priority_queue<long int> BestScores;
};

#endif // _CGAMEAPP_H_