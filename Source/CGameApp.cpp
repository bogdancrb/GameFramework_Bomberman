//-----------------------------------------------------------------------------
// File: CGameApp.cpp
//
// Desc: Game Application class, this is the central hub for all app processing
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CGameApp Specific Includes
//-----------------------------------------------------------------------------
#include "CGameApp.h"

extern HINSTANCE g_hInst;

//-----------------------------------------------------------------------------
// CGameApp Member Functions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CGameApp () (Constructor)
// Desc : CGameApp Class Constructor
//-----------------------------------------------------------------------------
CGameApp::CGameApp()
{
	//PlaySound("data/menu/song1.wav", NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);

	// Reset / Clear all required values
	m_hWnd			= NULL;
	m_hIcon			= NULL;
	m_hMenu			= NULL;
	m_pBBuffer		= NULL;
	m_pPlayer		= NULL;
	m_Map           = NULL;
	m_MMenu			= NULL;
	m_SMenu			= NULL;
	m_HighMenu		= NULL;

	for (int index = 0; index < MAX_CRATE; index++)
		m_pCratesAndBombs[index] = NULL;

	m_LastFrameRate = 0;

	m_CurrentGameLevel = m_LoadGameLevel = 1;

	LoadGame();
}

//-----------------------------------------------------------------------------
// Name : ~CGameApp () (Destructor)
// Desc : CGameApp Class Destructor
//-----------------------------------------------------------------------------
CGameApp::~CGameApp()
{
	// Shut the engine down
	ShutDown();
}

//-----------------------------------------------------------------------------
// Name : InitInstance ()
// Desc : Initialises the entire Engine here.
//-----------------------------------------------------------------------------
bool CGameApp::InitInstance( LPCTSTR lpCmdLine, int iCmdShow )
{
	// Create the primary display device
	if (!CreateDisplay()) { ShutDown(); return false; }

	// Build Objects
	if (!BuildObjects()) 
	{ 
		MessageBox( 0, _T("Error code: 1\n\nFailed to initialize properly. Reinstalling the application may solve this problem.\nIf the problem persists, please contact technical support."), _T("Fatal Error"), MB_OK | MB_ICONSTOP);
		ShutDown(); 
		return false; 
	}

	// Set up all required game states
	SetupGameState();

	// Success!
	return true;
}

//-----------------------------------------------------------------------------
// Name : CreateDisplay ()
// Desc : Create the display windows, devices etc, ready for rendering.
//-----------------------------------------------------------------------------
bool CGameApp::CreateDisplay()
{
	LPTSTR			WindowTitle		= _T("Bomberman");
	LPCSTR			WindowClass		= _T("GameFramework_Class");
	USHORT			Width;
	USHORT			Height;
	RECT			rc;
	WNDCLASSEX		wcex;
	HMONITOR		hmon;
	MONITORINFO		m_inf = { sizeof(m_inf) };
	MSG msg;

	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc	= CGameApp::StaticWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= g_hInst;
	wcex.hIcon			= LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= WindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON));

	if(RegisterClassEx(&wcex)==0)
		return false;

	// Retrieve the final client size of the window
	::GetClientRect( m_hWnd, &rc );
	m_nViewX		= rc.left;
	m_nViewY		= rc.top;
	m_nViewWidth	= rc.right - rc.left;
	m_nViewHeight	= rc.bottom - rc.top;

	hmon = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
	GetMonitorInfo(hmon, &m_inf);

	Width = m_inf.rcMonitor.right;
	Height = m_inf.rcMonitor.bottom;

	m_hWnd = CreateWindow(WindowClass, WindowTitle, WS_POPUP | WS_VISIBLE,
		m_inf.rcMonitor.left, m_inf.rcMonitor.top, m_inf.rcMonitor.right - m_inf.rcMonitor.left,
		m_inf.rcMonitor.bottom - m_inf.rcMonitor.top, NULL, NULL, g_hInst, this);

	if (!m_hWnd)
		return false;

	// Show the window
	ShowWindow(m_hWnd, SW_SHOWMAXIMIZED);
	SetForegroundWindow(m_hWnd);
    SetFocus(m_hWnd);

	// Success!!
	return true;
}

//-----------------------------------------------------------------------------
// Name : BeginGame ()
// Desc : Signals the beginning of the physical post-initialisation stage.
//		From here on, the game engine has control over processing.
//-----------------------------------------------------------------------------
int CGameApp::BeginGame()
{
	MSG		msg;

	// Start main loop
	while(true) 
	{
		// Did we recieve a message, or are we idling ?
		if ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) 
		{
			if (msg.message == WM_QUIT) break;
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		} 
		else 
		{
			// Advance Game Frame.
			FrameAdvance();

		} // End If messages waiting
	
	} // Until quit message is receieved

	return 0;
}

//-----------------------------------------------------------------------------
// Name : ShutDown ()
// Desc : Shuts down the game engine, and frees up all resources.
//-----------------------------------------------------------------------------
bool CGameApp::ShutDown()
{
	// Release any previously built objects
	ReleaseObjects ( );
	
	// Destroy menu, it may not be attached
	if ( m_hMenu ) DestroyMenu( m_hMenu );
	m_hMenu		 = NULL;

	// Destroy the render window
	SetMenu( m_hWnd, NULL );
	if ( m_hWnd ) DestroyWindow( m_hWnd );
	m_hWnd		  = NULL;
	
	// Shutdown Success
	return true;
}

//-----------------------------------------------------------------------------
// Name : StaticWndProc () (Static Callback)
// Desc : This is the main messge pump for ALL display devices, it captures
//		the appropriate messages, and routes them through to the application
//		class for which it was intended, therefore giving full class access.
// Note : It is VITALLY important that you should pass your 'this' pointer to
//		the lpParam parameter of the CreateWindow function if you wish to be
//		able to pass messages back to that app object.
//-----------------------------------------------------------------------------
LRESULT CALLBACK CGameApp::StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	// If this is a create message, trap the 'this' pointer passed in and store it within the window.
	if ( Message == WM_CREATE ) SetWindowLong( hWnd, GWL_USERDATA, (LONG)((CREATESTRUCT FAR *)lParam)->lpCreateParams);

	// Obtain the correct destination for this message
	CGameApp *Destination = (CGameApp*)GetWindowLong( hWnd, GWL_USERDATA );
	
	// If the hWnd has a related class, pass it through
	if (Destination) return Destination->DisplayWndProc( hWnd, Message, wParam, lParam );
	
	// No destination found, defer to system...
	return DefWindowProc( hWnd, Message, wParam, lParam );
}

//-----------------------------------------------------------------------------
// Name : DisplayWndProc ()
// Desc : The display devices internal WndProc function. All messages being
//		passed to this function are relative to the window it owns.
//-----------------------------------------------------------------------------
LRESULT CGameApp::DisplayWndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
	static UINT			fTimer;	

	// Determine message type
	switch (Message)
	{
		case WM_CREATE:
			break;
		
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		
		case WM_SIZE:
			if ( wParam == SIZE_MINIMIZED )
			{
				// App is inactive
				m_bActive = false;
			
			} // App has been minimized
			else
			{
				// App is active
				m_bActive = true;

				// Store new viewport sizes
				m_nViewWidth  = LOWORD( lParam );
				m_nViewHeight = HIWORD( lParam );
		
			
			} // End if !Minimized

			break;

		case WM_LBUTTONDOWN:
			// Capture the mouse
			SetCapture( m_hWnd );
			GetCursorPos( &m_OldCursorPos );
			break;

		case WM_LBUTTONUP:
			// Release the mouse
			ReleaseCapture( );
			break;

		case WM_KEYDOWN:
			switch(wParam)
			{
			/*case VK_ESCAPE:
				PostQuitMessage(0);
				break;*/
			/*case VK_RETURN:
				fTimer = SetTimer(m_hWnd, 1, EXPLOSION_SPEED, NULL);
				m_pPlayer->Explode();
				break;*/
			case VK_SPACE: // Daca jucatorul apasa tasta SPACE
				if ( (m_LoadGameLevel >= 1 || m_LoadGameLevel == TEST_LEVEL) && m_LoadGameLevel != BOSS_LEVEL)
				{
 					m_pPlayer->PlaceBomb(&m_pBomb,m_pBBuffer, AnimationID); // Plasam bomba pe harta
					SetTimer(m_hWnd, 2, BOMB_TIMER*5, NULL);
					//KillTimer(m_hWnd,2);
				}
				break;
			case VK_F1:
				F1Pressed ? F1Pressed = false : F1Pressed = true;
				break;
			case VK_F2:
				// Folosit doar pentru teste
				m_LoadGameLevel = TEST_LEVEL;
				LoadLevel();
				break;
			}
			break;

		case WM_TIMER:
			switch(wParam)
			{
			case 1:
				// Verificam daca bomba a fost creata (plasata)
				for (int index = 0; index < m_pBomb.size(); index++)
				{
					if (m_pBomb[index] != NULL)
					{
						// Daca bomba a explodat, o stergem din memorie si inchidem timer-ul
						if(!m_pBomb[index]->BombAdvanceExplosion())
						{
							delete m_pBomb[index];
							m_pBomb[index] = NULL;
							m_pBomb.erase(m_pBomb.begin() + index);
							KillTimer(m_hWnd, 1);
						}
					}
				}
				break;

			case 2:
				for (int index = 0; index < m_pBomb.size(); index++)
				{
					if(!m_pBomb[index]->BombAdvancePlacement())
						KillTimer(m_hWnd, 2);
				}
				break;

			case 3:
				if(!m_pPlayer->AdvanceMovement(AnimationID))
					KillTimer(m_hWnd, 3);
			}
			break;

		case WM_COMMAND:
			break;

		default:
			return DefWindowProc(hWnd, Message, wParam, lParam);

	} // End Message Switch
	
	return 0;
}

//-----------------------------------------------------------------------------
// Name : BuildObjects ()
// Desc : Build our demonstration meshes, and the objects that instance them
//-----------------------------------------------------------------------------
bool CGameApp::BuildObjects()
{
	bool ok = true;

	m_pBBuffer = new BackBuffer(m_hWnd, m_nViewWidth, m_nViewHeight); 

	if (m_MMenu == NULL)
		m_MMenu = new MainMenu(m_pBBuffer, 1366, 768);

	if (!m_MMenu->m_Active)
	{
		m_pPlayer = new CPlayer(m_pBBuffer);

		for (int index = 0; index < MAX_NPCS; index++)
		{
			m_pNPC.resize(index+1);
			m_pNPC[index] = new CNonPlayer(m_pBBuffer);
		}

		for (int index = 0; index < MAX_CRATE; index++)
			m_pCratesAndBombs[index] = new CObject(m_pBBuffer);

		if (m_SMenu == NULL)
			m_SMenu = new InGameMenu(m_pBBuffer, 1366, 768);
	}

	ok = LoadLevel(); // Incarcam nivelul (harta + background)

	// Success!
	return ok;
}

//-----------------------------------------------------------------------------
// Name : LoadLevel ()
// Desc : Incarcam nivelul in functie de m_LoadGameLevel
//-----------------------------------------------------------------------------
bool CGameApp::LoadLevel()
{
	char mapName[255], bgName[255], mapNr[20];

	itoa(m_LoadGameLevel,mapNr,10);

	if (!m_MMenu->m_Active)
	{
		if (m_LoadGameLevel >= 1 && m_LoadGameLevel != BOSS_LEVEL)
		{
			strcpy(mapName,"data/"); strcat(mapName,mapNr); strcat(mapName,".gamemap");
		
			// Incarcam harta doar daca nu exista alta incarcata momentan
			if (m_Map == NULL)
				m_Map = new CMap(mapName, m_pBBuffer); // Incarcare harta din fisier (datele sunt intr-o matrice)

			m_CurrentGameLevel = m_LoadGameLevel;
		}
		else if (m_LoadGameLevel <= BONUS_LEVEL1)
		{
			strcpy(mapName,"data/bonus"); strcat(mapName,mapNr); strcat(mapName,".gamemap");

			m_BonusMap = new CMap(mapName, m_pBBuffer);
		}
		else if (m_LoadGameLevel == BOSS_LEVEL)
		{
			itoa(BOSS_LEVEL,mapNr,10);
			strcpy(mapName,"data/"); strcat(mapName,mapNr); strcat(mapName,".gamemap");

			m_BossMap = new CMap(mapName, m_pBBuffer);
		}
		else if (m_LoadGameLevel == TEST_LEVEL)
		{
			m_Map = new CMap("data/test.gamemap", m_pBBuffer);
		}
	}

	strcpy(bgName,"data/background/background"); strcat(bgName,mapNr); strcat(bgName,".bmp");

	if(!m_imgBackground.LoadBitmapFromFile(bgName, GetDC(m_hWnd)))
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// Name : SetupGameState ()
// Desc : Sets up all the initial states required by the game.
//-----------------------------------------------------------------------------
void CGameApp::SetupGameState()
{
	if (!m_MMenu->m_Active)
	{
		if (SavedPlayerPoints)
			m_pPlayer->m_pPoints = SavedPlayerPoints;

		if (m_LoadGameLevel == 1 || m_LoadGameLevel == 2 || m_LoadGameLevel == TEST_LEVEL) // Level 1 + level test
		{
			for (int index = 0; index < 4; index++)
				m_pPlayer->Position(index) = Vec2(390, 690);
		}
		else if (m_LoadGameLevel == 3)
		{
			for (int index = 0; index < 4; index++)
				m_pPlayer->Position(index) = Vec2(690, 390);
		}
		else if (m_LoadGameLevel <= BONUS_LEVEL1) // Levele bonus
		{
			for (int index = 0; index < 4; index++)
				m_pPlayer->Position(index) = Vec2(690, 690);

			for (int index = 0; index < MAX_CRATE; index++)
				m_pCratesAndBombs[index]->StartMoving();
		}
		else if (m_LoadGameLevel == BOSS_LEVEL) // Level boss
		{
			for (int index = 0; index < 4; index++)
				m_pPlayer->Position(index) = Vec2(690, 690);
		}

		// Pozitia decalata si cea veche sunt egale cu pozitia de inceput a jucatorului
		for (int index = 0; index < 4; index++)
			m_pPlayer->PlayerDecalPos(index) = m_pPlayer->PlayerOldPos(index) = m_pPlayer->Position(index);
	}
}

//-----------------------------------------------------------------------------
// Name : ReleaseObjects ()
// Desc : Releases our objects and their associated memory so that we can
//		rebuild them, if required, during our applications life-time.
//-----------------------------------------------------------------------------
void CGameApp::ReleaseObjects( int dontDeleteBuff )
{	
	if(m_pPlayer != NULL)
	{
		if (m_pPlayer->m_pPoints != NULL)
			SavedPlayerPoints = m_pPlayer->m_pPoints;

		delete m_pPlayer;
		m_pPlayer = NULL;
	}

	for (int index = 0; index < m_pNPC.size(); index++)
	{
		if(m_pNPC[index] != NULL)
		{
			delete m_pNPC[index];
			m_pNPC[index] = NULL;
			m_pNPC.erase(m_pNPC.begin() + index);
		}
	}

	for (int index = 0; index < MAX_CRATE; index++)
	{
		if(m_pCratesAndBombs[index] != NULL)
		{
			delete m_pCratesAndBombs[index];
			m_pCratesAndBombs[index] = NULL;
		}
	}

	if(m_Map != NULL)
	{
		delete m_Map;
		m_Map = NULL;
	}

	if(m_BonusMap != NULL)
	{
		delete m_BonusMap;
		m_BonusMap = NULL;
	}

	if(m_BossMap != NULL)
	{
		delete m_BossMap;
		m_BossMap = NULL;
	}

	if (m_SMenu != NULL)
	{
		delete m_SMenu;
		m_SMenu = NULL;
	}

	if(m_pBBuffer != NULL && !dontDeleteBuff)
	{
		delete m_pBBuffer;
		m_pBBuffer = NULL;
	}

	// Pentru a da restart la level curent, nu la tot jocul
	m_LoadGameLevel = m_CurrentGameLevel;
}

//-----------------------------------------------------------------------------
// Name : FrameAdvance () (Private)
// Desc : Called to signal that we are now rendering the next frame.
//-----------------------------------------------------------------------------
void CGameApp::FrameAdvance()
{
	// Advance the timer
	m_Timer.Tick( );

	// Skip if app is inactive
	if ( !m_bActive ) return;

	// Poll & Process input devices
	ProcessInput();

	// Verificam daca s-au apasat butoane pe ecran
	ProcessMenuButtons();

	if (!m_MMenu->m_Active && !m_SMenu->m_Active)
	{
		// Animate the game objects
		AnimateObjects();

		// Daca jucatorul a murit, atunci resetam pozitia acestuia
		if (m_pPlayer->ResetPosition)
		{
			SetupGameState();
			m_pPlayer->ResetPosition = false;
		}

		if ( (m_LoadGameLevel >= 1 || m_LoadGameLevel == TEST_LEVEL) && m_LoadGameLevel != BOSS_LEVEL) // Levele principale + level test
		{
			// Detecteaza coliziune [jucator - ziduri]
			m_pPlayer->PlayerColision(m_Map, AnimationID);

			for (int npcIndex = 0; npcIndex < m_pNPC.size(); npcIndex++)
			{
				if (m_pNPC[npcIndex] != NULL)
				{
					// Punem NPC-ul in miscare, cu detecare daca jucatorul este prin apropiere si daca se face coliziune [NPC - jucator]
					m_pNPC[npcIndex]->NPCMove(m_pPlayer, m_Map, m_pBomb, AnimationID);

					// Detecteaza coliziune [NPC - ziduri - bomba]
					m_pNPC[npcIndex]->NPCColision(m_Map, m_pBomb);
				}
			}

			// Detecteaza daca vreo bomba a fost acivata si daca exista coliziune intre [explozie - NPC - jucator]
			CheckBombs();
		}
		else if (m_LoadGameLevel <= BONUS_LEVEL1) // Levele bonus
		{
			m_pPlayer->PlayerColision(m_BonusMap, AnimationID);

			for (int index = 0; index < MAX_CRATE; index++)
			{
				// Daca o bomba a facut coliziune cu jucatorul
				if(m_pCratesAndBombs[index]->ObjColision(m_pPlayer))
				{
					// Incarcam nivelul princpal la care ne aflam
					m_LoadGameLevel = m_CurrentGameLevel;
					LoadLevel();
					SetupGameState();
					SavedPlayerPoints = m_pPlayer->m_pPoints;
					break;
				}
			}
		}
		else if (m_LoadGameLevel == BOSS_LEVEL) // Level boss
		{
			m_pPlayer->PlayerColision(m_BossMap, AnimationID);
		}	

		// Verificam diferite pozitii legate de jucator
		CheckPositions();
	}
	
	// Drawing the game objects
	DrawObjects();
}

//-----------------------------------------------------------------------------
// Name : ProcessInput () (Private)
// Desc : Simply polls the input devices and performs basic input operations
//-----------------------------------------------------------------------------
void CGameApp::ProcessInput( )
{
	static UCHAR pKeyBuffer[ 256 ];
	ULONG		Direction = 0;

	// Retrieve keyboard state
	if ( !GetKeyboardState( pKeyBuffer ) ) return;

	if (!m_MMenu->m_Active)
	{
		// Check the relevant keys
		if (m_pPlayer->CanMove() && !m_SMenu->m_Active) // Verificam daca jucatorul se poate misca
		{
			RECT r;
			r.left = 0;
			r.top = 0;
			r.right = 60;
			r.bottom = 60;

			if ( (pKeyBuffer[ VK_UP ] & 0xF0) && (m_LoadGameLevel >= 1 || m_LoadGameLevel == TEST_LEVEL) && m_LoadGameLevel != BOSS_LEVEL)
			{
				m_pPlayer->PlayerOldPos(1) = m_pPlayer->Position(AnimationID);
				AnimationID = 1;
				SetTimer(m_hWnd, 3, 120, NULL);
				Direction |= CPlayer::DIR_FORWARD;
				m_pPlayer->PlayerOldPos(AnimationID) = m_pPlayer->Position(AnimationID); // Memoram vechea pozitie a jucatorului
				m_pPlayer->CanMove() = false; // Jucatorul nu se mai poate misca in alte directii
			}
			else if ( (pKeyBuffer[ VK_DOWN ] & 0xF0) && (m_LoadGameLevel >= 1 || m_LoadGameLevel == TEST_LEVEL) && m_LoadGameLevel != BOSS_LEVEL)
			{
				m_pPlayer->PlayerOldPos(0) = m_pPlayer->Position(AnimationID);
				AnimationID = 0;
				SetTimer(m_hWnd, 3, 120, NULL);
				Direction |= CPlayer::DIR_BACKWARD;
				m_pPlayer->CanMove() = false;
			}
			else if ( pKeyBuffer[ VK_LEFT ] & 0xF0 )
			{
				m_pPlayer->PlayerOldPos(3) = m_pPlayer->Position(AnimationID);
				AnimationID = 3;
				SetTimer(m_hWnd, 3, 120, NULL);
				Direction |= CPlayer::DIR_LEFT;
				m_pPlayer->PlayerOldPos(AnimationID) = m_pPlayer->Position(AnimationID);
				m_pPlayer->CanMove() = false;
			}
			else if ( pKeyBuffer[ VK_RIGHT ] & 0xF0 )
			{
				m_pPlayer->PlayerOldPos(2) = m_pPlayer->Position(AnimationID);
				AnimationID = 2;
				SetTimer(m_hWnd, 3, 120, NULL);
				Direction |= CPlayer::DIR_RIGHT;
				m_pPlayer->PlayerOldPos(AnimationID) = m_pPlayer->Position(AnimationID);
				m_pPlayer->CanMove() = false;
			}
		}

		m_pPlayer->Move(Direction,AnimationID);
	}
}

//-----------------------------------------------------------------------------
// Name : ProcessButtons () (Private)
// Desc : Verifica daca s-au apasat butoane pe ecran (in general cele de menu)
//-----------------------------------------------------------------------------
void CGameApp::ProcessMenuButtons()
{
	POINT		CursorPos;
	
	// Daca menu principal este inactiv
	if (!m_MMenu->m_Active)
	{
		// Verificam daca s-a apasat mouse
		if ( GetCapture() == m_hWnd )
		{
			// Memoram pozitia cursorului
			GetCursorPos( &CursorPos );

			// Buton pentru pause in-game
			if (CursorPos.x >= 1330 && CursorPos.x <= 1365 && CursorPos.y >= 0 && CursorPos.y <= 30)
			{
				// Setam menu secundar ca fiind activ
				m_SMenu->m_Active = true;
			}

			// Daca menu secundar este activ
			if (m_SMenu->m_Active)
			{
				// Daca cursorul se afla pe menu secundar
				if (CursorPos.x >= 605 && CursorPos.x <= 760)
				{
					// Menu secundar - Continue
					if (CursorPos.y >= 290 && CursorPos.y <= 340)
					{
						// Ascundem menu secundar
						m_SMenu->m_Active = false;
					}

					// Menu secundar - Restart
					if (CursorPos.y >= 350 && CursorPos.y <= 400)
					{
						// Stergem toate obiectele
						ReleaseObjects();

						//Incarcam obiectele
						BuildObjects();

						// Incarcam setarile initiale
						SetupGameState();

						// Ascundem menu-ul secundar
						m_SMenu->m_Active = false;
					}

					// Menu secundar - Main Menu
					if (CursorPos.y >= 405 && CursorPos.y <= 450)
					{
						// Stergem toate obiectele
						ReleaseObjects(1);

						// Creem menu principal
						m_MMenu = new MainMenu(m_pBBuffer, 1366, 768);

						// Menu principal este activ
						m_MMenu->m_Active = true;
					}

					// Menu secundar - Exit
					if (CursorPos.y >= 460 && CursorPos.y <= 505)
					{
						SaveGame();
						PostQuitMessage(0);
					}
				}
			}
		}
	}
	else // Daca menu principal este activ
	{
		// Verificam daca s-a apasat mouse
		if (GetCapture() == m_hWnd)
		{
			// Memoram pozitia cursorului
			GetCursorPos(&CursorPos);

			// Menu principal - Play
			if (CursorPos.x >= 460 && CursorPos.x <= 580 && CursorPos.y >= 390 && CursorPos.y <= 460)
			{
				// Ascundem menu principal
				m_MMenu->m_Active = false;

				//Incarcam obiectele
				BuildObjects();

				// Incarcam setarile initiale
				SetupGameState();
			}

			// Menu principal - High Scores
			if (CursorPos.x >= 430 && CursorPos.x <= 620 && CursorPos.y >= 500 && CursorPos.y <= 540)
			{
				LoadGame();

				m_HighMenu = new HighScoreMenu(m_pBBuffer, 1366, 768);

				m_HighMenu->m_ActiveScore = true;
			}

			if (m_HighMenu != NULL && m_HighMenu->m_ActiveScore)
			{
				if (CursorPos.x >= 600 && CursorPos.x <= 760 && CursorPos.y >= 490 && CursorPos.y <= 540)
				{
					m_HighMenu->m_ActiveScore = false;

					delete m_HighMenu;
					m_HighMenu = NULL;
				}
				
			}

			// Menu principal - Exit
			if (CursorPos.x >= 480 && CursorPos.x <= 550 && CursorPos.y >= 580 && CursorPos.y <= 610)
			{
				SaveGame();
				PostQuitMessage(0);	
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Name : AnimateObjects () (Private)
// Desc : Animates the objects we currently have loaded.
//-----------------------------------------------------------------------------
void CGameApp::AnimateObjects()
{
	m_pPlayer->Update(m_Timer.GetTimeElapsed(),AnimationID);

	for (int index = 0; index < m_pNPC.size(); index++)
		if (m_pNPC[index] != NULL)
			m_pNPC[index]->UpdateNPC(m_Timer.GetTimeElapsed());

	for (int index = 0; index < MAX_CRATE; index++)
		m_pCratesAndBombs[index]->UpdateObj(m_Timer.GetTimeElapsed());

	for (int index = 0; index < m_pBomb.size(); index++)
		if (m_pBomb[index] != NULL)
			m_pBomb[index]->UpdateBomb(m_Timer.GetTimeElapsed());
}

//-----------------------------------------------------------------------------
// Name : DrawObjects () (Private)
// Desc : Draws the game objects
//-----------------------------------------------------------------------------
void CGameApp::DrawObjects()
{
	m_pBBuffer->reset();

	// Daca menu principal este activ
	if (m_MMenu->m_Active)
	{
		// Afisam menu principal
		m_MMenu->Draw();

		if (m_HighMenu != NULL && m_HighMenu->m_ActiveScore)
		{
			m_HighMenu->Draw();
			//TextOut(m_pBBuffer->getDC(), 360, 360, BestScores.top(), strlen(FrameRate));
			BestScores.pop();
		}
	}
	else
	{
		m_imgBackground.Paint(m_pBBuffer->getDC(), 0, 0);

		if ( (m_LoadGameLevel >= 1 || m_LoadGameLevel == TEST_LEVEL) && m_LoadGameLevel != BOSS_LEVEL) // Levele principale + level test
		{
			// Desenam bomba, daca aceasta a fost creata
			for (int index = 0; index < m_pBomb.size(); index++)
				if (m_pBomb[index] != NULL)
					m_pBomb[index]->DrawBomb();

			// Desenam NPC-urile
			for (int index = 0; index < m_pNPC.size(); index++)
				if (m_pNPC[index] != NULL)
					m_pNPC[index]->DrawNPC();

			if (m_Map->GetPortal()->isSpriteVisible)
				m_Map->GeneratePortal(); // Generare portal random pe harta
			m_Map->Draw(0,0); // Desenare harta

			// Generam pozitii random pentru NPC pe harta, daca este cazul
			NPCStartingPosition();
		}
		else if (m_LoadGameLevel <= BONUS_LEVEL1) // Levele bonus
		{
			for (int index = 0; index < MAX_CRATE; index++)
				if (m_pCratesAndBombs[index]->Visible())
					m_pCratesAndBombs[index]->DrawObj();

			m_BonusMap->Draw(0,0); // Desenare harta bonus
		}
		else if (m_LoadGameLevel == BOSS_LEVEL) // Level boss
		{
			m_BossMap->Draw(0,0); // Desenare harta bonus
		}

		m_pPlayer->Draw(AnimationID);

		// Afisam menu secundar
		m_SMenu->Draw();
	}

	// Afisam informatii legate de FPS etc. daca s-a apasat F1
	DrawInfo();

	// Daca jucatorul a murit
	if (m_pPlayer != NULL && m_pPlayer->get_if_is_dead() == true)
	{
		SaveGame();
		PostQuitMessage(0);
		//OR 	ReleaseObjects ( );	 AND return to main menu
	}

	m_pBBuffer->present();
}

//-----------------------------------------------------------------------------
// Name : CheckBombs () (Private)
// Desc : Verificam daca bombele sunt active sau nu, plus coliziunea cu explozia
//-----------------------------------------------------------------------------
void CGameApp::CheckBombs()
{
	//static int BombTimer; // Cronometram cat timp s-a scurs de cand a fost plasata bomba
	  
	for (int index = 0; index < m_pBomb.size(); index++)
	{
		if (m_pBomb[index] != NULL)
		{
			// Daca bomba a fost activata, atunci crestem cronometrul
			if (m_pBomb[index]->m_BombIsActive)
			{
				m_pBomb[index]->m_BombTimer++;
			}

			// Daca cronometrul a ajuns la timpul stabilit de BOMB_TIMER aceasta poate exploda
			if (m_pBomb[index]->m_BombTimer == BOMB_TIMER)
			{
				SetTimer(m_hWnd, 1, EXPLOSION_SPEED, NULL);
				m_pBomb[index]->BombExplode(m_Map);
				m_pBomb[index]->m_BombTimer = 0;

				// Verificare coliziuni [explozie - NPC - jucator]
				for (int indexBomb = 0; indexBomb < m_pBomb[index]->m_BombExplosionRange; indexBomb++)
				{
					// Daca (pozitia jucatorului - pozitia exploziei) este mai mica de BLOCKSIZE, atunci avem coliziune intre [jucator - explozie]
					if ((abs(m_pPlayer->Position(AnimationID).x - m_pBomb[index]->BombExplosionPosition(indexBomb).x) < BLOCKSIZE-5 && abs(m_pPlayer->Position(AnimationID).y - m_pBomb[index]->BombExplosionPosition(indexBomb).y) < BLOCKSIZE-5)) // coliziune jucator & explozie
					{
						// Setam ca jucatorul a murit
						m_pPlayer->ResetPosition = true;
						m_pPlayer->get_m_pLives().back()->removed = true;
						m_pPlayer->Killed();
						break;
					}

					for (int indexNPC = 0; indexNPC < m_pNPC.size(); indexNPC++)
					{
						if (m_pNPC[indexNPC] != NULL)
						{
							// Daca (pozitia NPC-ului - pozitia exploziei) este mai mica de BLOCKSIZE, atunci avem coliziune intre [NPC - explozie]
							if ((abs(m_pNPC[indexNPC]->NPCPosition().x - m_pBomb[index]->BombExplosionPosition(indexBomb).x) < BLOCKSIZE-5 && abs(m_pNPC[indexNPC]->NPCPosition().y - m_pBomb[index]->BombExplosionPosition(indexBomb).y) < BLOCKSIZE-5)) // coliziune NPC & explozie
							{
								// Stergem NPC din memorie
								delete m_pNPC[indexNPC];
								m_pNPC[indexNPC] = NULL;
								m_pNPC.erase(m_pNPC.begin() + indexNPC);
								m_pPlayer->m_pPoints += rand() % 50 + 20;
							}
						}
					}

					// Daca toti inamicii au murit, trecem la nivelul urmator
					if (m_pNPC.empty())
					{
						ReleaseObjects();

						m_LoadGameLevel++;

						if (m_LoadGameLevel > MAX_LEVELS)
							m_LoadGameLevel = BOSS_LEVEL;

						BuildObjects();

						SetupGameState();
					}
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Name : NPCStartingPosition () (Private)
// Desc : Stabileste pozitiile de inceput ale NPC-ului, in functie de celalalte 
// pozitii ale celorlalti NPC si ziduri
//-----------------------------------------------------------------------------
void CGameApp::NPCStartingPosition()
{
	int i, j;

	for (int indexNPC = 0; indexNPC < m_pNPC.size(); indexNPC++)
	{
		for (int id = 0; id < 3; id++)
		{
			for (int indexMap = 0; indexMap < m_Map->NrOfWalls[id]; indexMap++)
			{
				if (m_pNPC[indexNPC] != NULL)
				{
					if (id == 0) // wall
					{
						// Cat timp pozitia NPC-ului este egala cu cea a unui zid sau pozitia este (0,0)
						while (
							(m_pNPC[indexNPC]->NPCPosition() == m_Map->WallPosition(indexMap)) 
							|| m_pNPC[indexNPC]->NPCPosition() == Vec2(0,0)
							)
						{
							// Generam niste coordonate random
							i = rand() % m_Map->m_MapMatrix.size() + 0;
							j = rand() % m_Map->m_MapMatrix[i].size() + 0;

							// Daca (viitoarea pozitie a NPC-ului - pozitia jucatorului) este mai mare sau egala decat NPC_SPAWN_DISTANCE
							if (abs(m_pPlayer->Position(AnimationID).x - j * BLOCKSIZE + BLOCKSIZE / 2) >= NPC_SPAWN_DISTANCE 
								&& abs(m_pPlayer->Position(AnimationID).y - i * BLOCKSIZE + BLOCKSIZE / 2) >= NPC_SPAWN_DISTANCE)
							{
								// Setam coordonatele random pentru pozitia jucatorului, dar si cea veche si decalata
								m_pNPC[indexNPC]->NPCPosition().x = m_pNPC[indexNPC]->NPCOldPosition().x = m_pNPC[indexNPC]->NPCDecalPosition().x = j * BLOCKSIZE + BLOCKSIZE / 2;
								m_pNPC[indexNPC]->NPCPosition().y = m_pNPC[indexNPC]->NPCOldPosition().y = m_pNPC[indexNPC]->NPCDecalPosition().y = i * BLOCKSIZE + BLOCKSIZE / 2;
							}

							// Trebuie ca index sa fie 0, pentru a verifica pozitiile cu zidurile precedente
							indexMap = 0;
						}
					}
					else if (id == 1) // indestructable
					{
						while (
							(m_pNPC[indexNPC]->NPCPosition() == m_Map->IndesctructPosition(indexMap)) 
							|| m_pNPC[indexNPC]->NPCPosition() == Vec2(0,0)
							)
						{
							i = rand() % m_Map->m_MapMatrix.size() + 0;
							j = rand() % m_Map->m_MapMatrix[i].size() + 0;

							if (abs(m_pPlayer->Position(AnimationID).x - j * BLOCKSIZE + BLOCKSIZE / 2) >= NPC_SPAWN_DISTANCE 
								&& abs(m_pPlayer->Position(AnimationID).y - i * BLOCKSIZE + BLOCKSIZE / 2) >= NPC_SPAWN_DISTANCE)
							{
								m_pNPC[indexNPC]->NPCPosition().x = m_pNPC[indexNPC]->NPCOldPosition().x = m_pNPC[indexNPC]->NPCDecalPosition().x = j * BLOCKSIZE + BLOCKSIZE / 2;
								m_pNPC[indexNPC]->NPCPosition().y = m_pNPC[indexNPC]->NPCOldPosition().y = m_pNPC[indexNPC]->NPCDecalPosition().y = i * BLOCKSIZE + BLOCKSIZE / 2;
							}

							indexMap = 0;
						}
					}
					else if (id == 2) // destructable
					{
						while (
							(m_pNPC[indexNPC]->NPCPosition() == m_Map->DesctructPosition(indexMap) && m_Map->isDesctructVisible(indexMap)) 
							|| m_pNPC[indexNPC]->NPCPosition() == Vec2(0,0)
							)
						{
							i = rand() % m_Map->m_MapMatrix.size() + 0;
							j = rand() % m_Map->m_MapMatrix[i].size() + 0;

							if (abs(m_pPlayer->Position(AnimationID).x - j * BLOCKSIZE + BLOCKSIZE / 2) >= NPC_SPAWN_DISTANCE 
								&& abs(m_pPlayer->Position(AnimationID).y - i * BLOCKSIZE + BLOCKSIZE / 2) >= NPC_SPAWN_DISTANCE)
							{
								m_pNPC[indexNPC]->NPCPosition().x = m_pNPC[indexNPC]->NPCOldPosition().x = m_pNPC[indexNPC]->NPCDecalPosition().x = j * BLOCKSIZE + BLOCKSIZE / 2;
								m_pNPC[indexNPC]->NPCPosition().y = m_pNPC[indexNPC]->NPCOldPosition().y = m_pNPC[indexNPC]->NPCDecalPosition().y = i * BLOCKSIZE + BLOCKSIZE / 2;
							}

							indexMap = 0;
						}
					}
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Name : CheckPositions () (Private)
// Desc : Verificam diferite pozitii legate de jucator
//-----------------------------------------------------------------------------
void CGameApp::CheckPositions()
{
	if (m_Map)
	{
		// Daca pozitia jucatorului este egala cu cea a portalului si nu ne aflam intr-un nivel bonus/boss si portalul este vizibil
		if ( m_pPlayer->Position(AnimationID) == m_Map->PortalPosition() 
			&& ( (m_LoadGameLevel >= 1 || m_LoadGameLevel == TEST_LEVEL) && m_LoadGameLevel != BOSS_LEVEL) 
			&& m_Map->isPortalVisible()
			)
		{
			// Selectam un nivel bonus random
			//int selectBonusLevel = rand() % BONUS_LEVEL2 + BONUS_LEVEL1;
			int selectBonusLevel = BONUS_LEVEL1;

			// Setam portalul ca fiind invizibil
			m_Map->isPortalVisible() = false;

			// Setam ca ne aflam in nivelul bonus ales random
			m_LoadGameLevel = selectBonusLevel;

			// Incarcam harta si background
			LoadLevel();

			// Pozitionam jucatorul 
			SetupGameState();
		}
	}
}

void CGameApp::SaveGame()
{
	if (m_pPlayer != NULL && SavedPlayerPoints != 0)
	{
		ofstream f("data/scores.data", ofstream::out);

		BestScores.push(SavedPlayerPoints);

		while (!BestScores.empty())
		{
			f << BestScores.top();
			BestScores.pop();
			if (!BestScores.empty())
				f << endl;
		}

		f.close();
	}
}

void CGameApp::LoadGame()
{
	ifstream f("data/scores.data", ifstream::in);
	int x;

	while (!f.eof())
	{
		f >> x;

		if (x != 0 && x > 0)
			BestScores.push(x);
	}
	
	f.close();
}

//-----------------------------------------------------------------------------
// Name : DrawInfo () (Private)
// Desc : Draws the game FPS, COORDONATES etc.
//-----------------------------------------------------------------------------
void CGameApp::DrawInfo()
{
	static TCHAR FrameRate[50];
	TCHAR DisplayInfo[255];
	char ConvToString[255] = {0};
	POINT		CursorPos;
	HFONT		hFont, hOldFont;

	SetTextColor(m_pBBuffer->getDC(), 0x00000000);
	SetBkMode(m_pBBuffer->getDC(), OPAQUE);

	if (F1Pressed) // Daca s-a apasat F1 atunci afisam niste date tehnice pe ecran
	{
		//draw framerate on buffer
		m_Timer.GetFrameRate(FrameRate, 50);
		TextOut(m_pBBuffer->getDC(), 10, 10, FrameRate, strlen(FrameRate));

		strcpy(DisplayInfo,"Player Real Pos X: "); // Copiem in PlayerXY: "Player Real Pos X:"
		itoa( (m_pPlayer != NULL ? m_pPlayer->Position(AnimationID).x : 0), ConvToString,10); // Convertim pozitia jucatorului pe X, din (int) in (char)
		strcat(DisplayInfo,ConvToString); // Adaugam pozitia in PlayerXY: "Player Real Pos X: %d"
		strcat(DisplayInfo,", Y: "); // Adaugam in PlayerXY: "Player Real Pos X: %d, Y:"
		itoa( (m_pPlayer != NULL ? m_pPlayer->Position(AnimationID).y : 0), ConvToString,10); // Convertim pozitia jucatorului pe Y, din (int) in (char)
		strcat(DisplayInfo,ConvToString); //  Adaugam pozitia in PlayerXY: "Player Real Pos X: %d, Y: %d"
		TextOut(m_pBBuffer->getDC(), 10, 50, DisplayInfo, strlen(DisplayInfo)); // Afisam PlayerXY pe ecran

		strcpy(DisplayInfo,"Player Dec Pos X: ");
		itoa( (m_pPlayer != NULL ? m_pPlayer->PlayerDecalPos(AnimationID).x : 0), ConvToString,10);
		strcat(DisplayInfo,ConvToString);
		strcat(DisplayInfo,", Y: ");
		itoa( (m_pPlayer != NULL ? m_pPlayer->PlayerDecalPos(AnimationID).y : 0), ConvToString,10);
		strcat(DisplayInfo,ConvToString);
		TextOut(m_pBBuffer->getDC(), 10, 80, DisplayInfo, strlen(DisplayInfo));

		strcpy(DisplayInfo,"Player Old Pos X: ");
		itoa( (m_pPlayer != NULL ? m_pPlayer->PlayerOldPos(AnimationID).x : 0), ConvToString,10);
		strcat(DisplayInfo,ConvToString);
		strcat(DisplayInfo,", Y: ");
		itoa( (m_pPlayer != NULL ? m_pPlayer->PlayerOldPos(AnimationID).y : 0), ConvToString,10);
		strcat(DisplayInfo,ConvToString);
		TextOut(m_pBBuffer->getDC(), 10, 110, DisplayInfo, strlen(DisplayInfo));

		strcpy(DisplayInfo,"Player Vel X: ");
		itoa( (m_pPlayer != NULL ? m_pPlayer->Velocity(AnimationID).x : 0), ConvToString,10);
		strcat(DisplayInfo,ConvToString);
		strcat(DisplayInfo,", Y: ");
		itoa( (m_pPlayer != NULL ? m_pPlayer->Velocity(AnimationID).y : 0), ConvToString,10);
		strcat(DisplayInfo,ConvToString);
		TextOut(m_pBBuffer->getDC(), 10, 140, DisplayInfo, strlen(DisplayInfo));

		GetCursorPos(&CursorPos);
		strcpy(DisplayInfo, "Cursor pos X: ");
		itoa(CursorPos.x, ConvToString, 10);
		strcat(DisplayInfo, ConvToString);
		strcat(DisplayInfo, ", Y: ");
		itoa(CursorPos.y, ConvToString, 10);
		strcat(DisplayInfo, ConvToString);
		TextOut(m_pBBuffer->getDC(), 10, 170, DisplayInfo, strlen(DisplayInfo));
	}

	if ( !m_MMenu->m_Active)
	{
		hFont = CreateFont(35, 0, 0, 0, FW_EXTRABOLD, 0, 0, 0, 0, 0, 0, 4, 0, "SYSTEM_FIXED_FONT");
		SetTextColor(m_pBBuffer->getDC(), 0x00FFFFFF);
		SetBkMode(m_pBBuffer->getDC(), TRANSPARENT);

		// Select the variable stock font into the specified device context. 
		hOldFont = (HFONT)SelectObject(m_pBBuffer->getDC(), hFont);

		if (m_pPlayer != NULL)
		{
			strcpy(DisplayInfo,"Puncte: ");
			itoa(m_pPlayer->m_pPoints,ConvToString,10);
			strcat(DisplayInfo,ConvToString);
			TextOut(m_pBBuffer->getDC(), 70, 15, DisplayInfo, strlen(DisplayInfo));
		}
		//SelectObject(m_pBBuffer->getDC(), hOldFont);
	}
}