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
float PlayerOldPosX, PlayerOldPosY;

//-----------------------------------------------------------------------------
// CGameApp Member Functions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CGameApp () (Constructor)
// Desc : CGameApp Class Constructor
//-----------------------------------------------------------------------------
CGameApp::CGameApp()
{
	// Reset / Clear all required values
	m_hWnd			= NULL;
	m_hIcon			= NULL;
	m_hMenu			= NULL;
	m_pBBuffer		= NULL;
	m_pPlayer		= NULL;
	m_Map           = NULL;
	m_pBomb			= NULL;
	m_LastFrameRate = 0;
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
		MessageBox( 0, _T("Failed to initialize properly. Reinstalling the application may solve this problem.\nIf the problem persists, please contact technical support."), _T("Fatal Error"), MB_OK | MB_ICONSTOP);
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
			case VK_ESCAPE:
				PostQuitMessage(0);
				break;
			/*case VK_RETURN:
				fTimer = SetTimer(m_hWnd, 1, EXPLOSION_SPEED, NULL);
				m_pPlayer->Explode();
				break;*/
			case VK_SPACE: // Daca jucatorul apasa tasta SPACE
				m_pPlayer->PlaceBomb(&m_pBomb,m_pBBuffer); // Plasam bomba pe harta
				break;
			case VK_F1:
				F1Pressed ? F1Pressed = false : F1Pressed = true;
				break;
			}
			break;

		case WM_TIMER:
			switch(wParam)
			{
			case 1:
				/*if(!m_pPlayer->AdvanceExplosion())
					KillTimer(m_hWnd, 1);*/

				// Verificam daca bomba a fost creata (plasata)
				if (m_pBomb != NULL)
				{
					// Daca bomba a explodat, o stergem din memorie si inchidem timer-ul
					if(!m_pBomb->BombAdvanceExplosion())
					{
						delete m_pBomb;
						m_pBomb = NULL;
						KillTimer(m_hWnd, 1);
					}
				}
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
	m_pBBuffer = new BackBuffer(m_hWnd, m_nViewWidth, m_nViewHeight); 
	m_pPlayer = new CPlayer(m_pBBuffer);
	m_Map = new CMap("data/1.gamemap", m_pBBuffer); // Incarcare harta din fisier (datele sunt intr-o matrice)
	
	if(!m_imgBackground.LoadBitmapFromFile("data/background/background1.bmp", GetDC(m_hWnd)))
		return false;

	// Success!
	return true;
}

//-----------------------------------------------------------------------------
// Name : SetupGameState ()
// Desc : Sets up all the initial states required by the game.
//-----------------------------------------------------------------------------
void CGameApp::SetupGameState()
{
	m_pPlayer->Position() = Vec2(390, 690);

	// Pozitia decalata si cea veche sunt egale cu pozitia de inceput a jucatorului
	m_pPlayer->PlayerDecalPos() = m_pPlayer->PlayerOldPos() = m_pPlayer->Position();
}

//-----------------------------------------------------------------------------
// Name : ReleaseObjects ()
// Desc : Releases our objects and their associated memory so that we can
//		rebuild them, if required, during our applications life-time.
//-----------------------------------------------------------------------------
void CGameApp::ReleaseObjects( )
{
	if(m_pPlayer != NULL)
	{
		delete m_pPlayer;
		m_pPlayer = NULL;
	}

	if(m_pBBuffer != NULL)
	{
		delete m_pBBuffer;
		m_pBBuffer = NULL;
	}
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

	// Animate the game objects
	AnimateObjects();

	// Detecteaza coliziune jucator - ziduri
	m_pPlayer->PlayerColision(m_Map);

	// Detecteaza daca vreo bomba a fost acivata
	CheckBombs();
	
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
	POINT		CursorPos;
	float		X = 0.0f, Y = 0.0f;

	// Retrieve keyboard state
	if ( !GetKeyboardState( pKeyBuffer ) ) return;

	// Check the relevant keys
	if (m_pPlayer->CanMove()) // Verificam daca jucatorul se poate misca
	{
		if ( pKeyBuffer[ VK_UP ] & 0xF0 )
		{
			Direction |= CPlayer::DIR_FORWARD;
			m_pPlayer->PlayerOldPos() = m_pPlayer->Position(); // Memoram vechea pozitie a jucatorului
			m_pPlayer->CanMove() = false; // Jucatorul nu se mai poate misca in alte directii
		}
		else if ( pKeyBuffer[ VK_DOWN ] & 0xF0 )
		{
			Direction |= CPlayer::DIR_BACKWARD;
			m_pPlayer->PlayerOldPos() = m_pPlayer->Position();
			m_pPlayer->CanMove() = false;
		}
		else if ( pKeyBuffer[ VK_LEFT ] & 0xF0 )
		{
			Direction |= CPlayer::DIR_LEFT;
			m_pPlayer->PlayerOldPos() = m_pPlayer->Position();
			m_pPlayer->CanMove() = false;
		}
		else if ( pKeyBuffer[ VK_RIGHT ] & 0xF0 )
		{
			Direction |= CPlayer::DIR_RIGHT;
			m_pPlayer->PlayerOldPos() = m_pPlayer->Position();
			m_pPlayer->CanMove() = false;
		}
	}

	m_pPlayer->Move(Direction);

	// Now process the mouse (if the button is pressed)
	if ( GetCapture() == m_hWnd )
	{
		// Hide the mouse pointer
		SetCursor( NULL );

		// Retrieve the cursor position
		GetCursorPos( &CursorPos );

		// Reset our cursor position so we can keep going forever :)
		SetCursorPos( m_OldCursorPos.x, m_OldCursorPos.y );

	} // End if Captured
}

//-----------------------------------------------------------------------------
// Name : AnimateObjects () (Private)
// Desc : Animates the objects we currently have loaded.
//-----------------------------------------------------------------------------
void CGameApp::AnimateObjects()
{
	m_pPlayer->Update(m_Timer.GetTimeElapsed());

	if (m_pBomb != NULL)
		m_pBomb->UpdateBomb(m_Timer.GetTimeElapsed());
}

//-----------------------------------------------------------------------------
// Name : DrawObjects () (Private)
// Desc : Draws the game objects
//-----------------------------------------------------------------------------
void CGameApp::DrawObjects()
{
	m_pBBuffer->reset();

	m_imgBackground.Paint(m_pBBuffer->getDC(), 0, 0);

	if (m_pBomb != NULL)
		m_pBomb->DrawBomb(); // Desenam bomba, daca aceasta a fost creata

	m_pPlayer->Draw();

	m_Map->Draw(0,0); // Desenare harta

	DrawInfo();

	m_pBBuffer->present();
}

//-----------------------------------------------------------------------------
// Name : CheckBombs () (Private)
// Desc : Checks if the bombs are active or not
//-----------------------------------------------------------------------------
void CGameApp::CheckBombs()
{
	static int BombTimer; // Cronometram cat timp s-a scurs de cand a fost plasata bomba
	
	if (m_pBomb != NULL)
	{
		// Daca bomba a fost activata, atunci crestem cronometrul
		if (m_pBomb->m_BombIsActive)
		{
			BombTimer++;
		}

		// Daca cronometrul a ajuns la timpul stabilit de BOMB_TIMER aceasta poate exploda
		if (BombTimer == BOMB_TIMER)
		{
			SetTimer(m_hWnd, 1, EXPLOSION_SPEED, NULL);
			m_pBomb->BombExplode(m_Map);
			BombTimer = 0;
			
			// Coliziuni cu explozia bombei
			for (int index = 0; index < EXPLOSION_RANGE; index++)
			{
				// Daca (pozitia jucatorului - pozitia exploziei) este mai mica de BLOCKSIZE, atunci avem coliziune intre jucator si explozie
				if ((abs(m_pPlayer->Position().x - m_pBomb->BombExplosionPosition(index).x) < BLOCKSIZE && abs(m_pPlayer->Position().y - m_pBomb->BombExplosionPosition(index).y) < BLOCKSIZE)) // coliziune jucator & explozie
				{
					// Setam jucatorul la coordonatele de inceput
					SetupGameState();
				}
			}
		}
	}
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

	if (F1Pressed) // Daca s-a apasat F1 atunci afisam niste date tehnice pe ecran
	{
		//draw framerate on buffer
		m_Timer.GetFrameRate(FrameRate, 50);
		TextOut(m_pBBuffer->getDC(), 10, 10, FrameRate, strlen(FrameRate));

		strcpy(DisplayInfo,"Player Real Pos X: "); // Copiem in PlayerXY: "Player Real Pos X:"
		itoa(m_pPlayer->Position().x,ConvToString,10); // Convertim pozitia jucatorului pe X, din (int) in (char)
		strcat(DisplayInfo,ConvToString); // Adaugam pozitia in PlayerXY: "Player Real Pos X: %d"
		strcat(DisplayInfo,", Y: "); // Adaugam in PlayerXY: "Player Real Pos X: %d, Y:"
		itoa(m_pPlayer->Position().y,ConvToString,10); // Convertim pozitia jucatorului pe Y, din (int) in (char)
		strcat(DisplayInfo,ConvToString); //  Adaugam pozitia in PlayerXY: "Player Real Pos X: %d, Y: %d"
		TextOut(m_pBBuffer->getDC(), 10, 40, DisplayInfo, strlen(DisplayInfo)); // Afisam PlayerXY pe ecran

		strcpy(DisplayInfo,"Player Dec Pos X: ");
		itoa(m_pPlayer->PlayerDecalPos().x,ConvToString,10);
		strcat(DisplayInfo,ConvToString);
		strcat(DisplayInfo,", Y: ");
		itoa(m_pPlayer->PlayerDecalPos().y,ConvToString,10);
		strcat(DisplayInfo,ConvToString);
		TextOut(m_pBBuffer->getDC(), 10, 70, DisplayInfo, strlen(DisplayInfo));

		strcpy(DisplayInfo,"Player Old Pos X: ");
		itoa(m_pPlayer->PlayerOldPos().x,ConvToString,10);
		strcat(DisplayInfo,ConvToString);
		strcat(DisplayInfo,", Y: ");
		itoa(m_pPlayer->PlayerOldPos().y,ConvToString,10);
		strcat(DisplayInfo,ConvToString);
		TextOut(m_pBBuffer->getDC(), 10, 100, DisplayInfo, strlen(DisplayInfo));

		strcpy(DisplayInfo,"Player Vel X: ");
		itoa(m_pPlayer->Velocity().x,ConvToString,10);
		strcat(DisplayInfo,ConvToString);
		strcat(DisplayInfo,", Y: ");
		itoa(m_pPlayer->Velocity().y,ConvToString,10);
		strcat(DisplayInfo,ConvToString);
		TextOut(m_pBBuffer->getDC(), 10, 130, DisplayInfo, strlen(DisplayInfo));
	}

	/*strcpy(DisplayInfo,"Puncte: ");
	itoa(m_pPlayer->m_pPoints,ToString,10);
	strcat(DisplayInfo,ToString);
	TextOut(m_pBBuffer->getDC(), GetSystemMetrics(SM_CXSCREEN)-100, GetSystemMetrics(SM_CYSCREEN)-100, DisplayInfo, strlen(DisplayInfo));

	strcpy(DisplayInfo,"Viata: ");
	itoa(m_pPlayer->m_pHealth,ToString,10);
	strcat(DisplayInfo,ToString);
	TextOut(m_pBBuffer->getDC(), GetSystemMetrics(SM_CXSCREEN)-200, GetSystemMetrics(SM_CYSCREEN)-100, DisplayInfo, strlen(DisplayInfo));*/
}