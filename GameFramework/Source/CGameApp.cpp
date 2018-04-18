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
#include <algorithm>
#include <string>
#include <fstream>
#include <iostream>

extern HINSTANCE g_hInst;

using namespace std;
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
	m_pPlayer2      = nullptr;
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
	LPTSTR			WindowTitle		= _T("GameFramework");
	LPCSTR			WindowClass		= _T("GameFramework_Class");
	USHORT			Width			= 1200;
	USHORT			Height			= 900;
	RECT			rc;
	WNDCLASSEX		wcex;


	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
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

	m_hWnd = CreateWindow(WindowClass, WindowTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, Width, Height, NULL, NULL, g_hInst, this);

	if (!m_hWnd)
		return false;

	// Show the window
	ShowWindow(m_hWnd, SW_MAXIMIZE);

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
	m_hWnd		 = NULL;
	
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
			case VK_RETURN:
				fTimer = SetTimer(m_hWnd, 1, 70, NULL);
				m_pPlayer->Explode();
				break;
			case 'Q':
				fTimer = SetTimer(m_hWnd, 1, 70, NULL);
				m_pPlayer2->Explode();
				break;
			case 'H':
				m_pPlayer2->Shoot2();
				break;
			case VK_SPACE:
				m_pPlayer->Shoot();
				break;
			case 'N':
				m_pPlayer->RotateLeft();
				break;
			case 'M':
				m_pPlayer->RotateRight();
				break;
			case 'R':
				m_pPlayer2->RotateLeft();
				break;
			case 'T':
				m_pPlayer2->RotateRight();
				break;
			case 'Z':
				Save_game();
				break;
			case 'X':
				fTimer = SetTimer(m_hWnd, 1, 70, NULL);
				Load_game();
				break;
			}
			break;

		case WM_TIMER:
			switch(wParam)
			{
			case 1:
				if(!m_pPlayer->AdvanceExplosion())
					KillTimer(m_hWnd, 1);
				if (!m_pPlayer2->AdvanceExplosion())
					KillTimer(m_hWnd, 1);
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
	m_pPlayer->lives = 3;
	m_pPlayer2 = new CPlayer(m_pBBuffer);
	m_pPlayer2->lives = 3;

	
	if(!m_imgBackground.LoadBitmapFromFile("data/spacerrr.bmp", GetDC(m_hWnd)))
		return false;
	if (!m_imgBackground1.LoadBitmapFromFile("data/copy.bmp", GetDC(m_hWnd)))
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
	m_pPlayer->Position() = Vec2(1300, 500);
	m_pPlayer2->Position() = Vec2(100, 500);
	m_pPlayer2->RotateRight();
	m_pPlayer->RotateLeft();

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
	if (m_pPlayer2 != NULL)
	{
		delete m_pPlayer2;
		m_pPlayer2 = NULL;
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
	static TCHAR FrameRate[ 50 ];
	static TCHAR TitleBuffer[ 255 ];

	// Advance the timer
	m_Timer.Tick( );

	// Skip if app is inactive
	if ( !m_bActive ) return;
	
	// Get / Display the framerate
	if ( m_LastFrameRate != m_Timer.GetFrameRate() )
	{
		m_LastFrameRate = m_Timer.GetFrameRate( FrameRate, 50 );
		sprintf_s( TitleBuffer, _T("Game : %s  Lives: %d - %d"), FrameRate, m_pPlayer2->lives, m_pPlayer->lives );
		SetWindowText( m_hWnd, TitleBuffer );

	} // End if Frame Rate Altered

	if (m_pPlayer->lives == 0) {
		MessageBox(0, "BOZGORII CASTIGA", "GAME OVER", MB_OK);
		PostQuitMessage(0);
	}

	else if (m_pPlayer2->lives == 0) {
		MessageBox(0, "VADIM CASTIGA", "GAME OVER", MB_OK);
		PostQuitMessage(0);
	}

	// Poll & Process input devices
	ProcessInput();

	// Animate the game objects
	AnimateObjects();

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
	ULONG		DirectionP2 = 0;
	POINT		CursorPos;
	float		X = 0.0f, Y = 0.0f;

	// Retrieve keyboard state
	if ( !GetKeyboardState( pKeyBuffer ) ) return;

	// Check the relevant keys
	if ( pKeyBuffer[ VK_UP	] & 0xF0 ) Direction |= CPlayer::DIR_FORWARD;
	if ( pKeyBuffer[ VK_DOWN  ] & 0xF0 ) Direction |= CPlayer::DIR_BACKWARD;
	if ( pKeyBuffer[ VK_LEFT  ] & 0xF0 ) Direction |= CPlayer::DIR_LEFT;
	if ( pKeyBuffer[ VK_RIGHT ] & 0xF0 ) Direction |= CPlayer::DIR_RIGHT;

	if (pKeyBuffer['W'] & 0xF0) DirectionP2 |= CPlayer::DIR_FORWARD;
	if (pKeyBuffer['S'] & 0xF0) DirectionP2 |= CPlayer::DIR_BACKWARD;
	if (pKeyBuffer['A'] & 0xF0) DirectionP2 |= CPlayer::DIR_LEFT;
	if (pKeyBuffer['D'] & 0xF0) DirectionP2 |= CPlayer::DIR_RIGHT;

	// Move the player
	m_pPlayer->Move(Direction);
	m_pPlayer2->Move(DirectionP2);


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

void CGameApp::DrawBackground()
{
	static int currentY0 = 0;
	static int currentY1 = m_imgBackground.Width();

	static size_t lastTime = GetTickCount();
	size_t currentTime = GetTickCount();

	if (currentTime - lastTime > 150)
	{
		lastTime = currentTime;
		currentY0 -= 5;
		currentY1 -= 5;

		if (currentY0 < -m_imgBackground.Width())
			currentY0 = m_imgBackground.Width();

		if (currentY1 < -m_imgBackground1.Width())
			currentY1 = m_imgBackground1.Width();
	}
	m_imgBackground.Paint(m_pBBuffer->getDC(),currentY0,0);
	m_imgBackground.Paint(m_pBBuffer->getDC(),currentY1,0);
}

int CGameApp::Sprite_Collide(Sprite * object1, Sprite * object2) {

	int left1, left2;
	int right1, right2;
	int top1, top2;
	int bottom1, bottom2;

	left1 = object1->mPosition.x - object1->width()/2;
	left2 = object2->mPosition.x - object2->width()/2;
	right1 = left1 + object1->width()/2;
	right2 = left2 + object2->width()/2;
	top1 = object1->mPosition.y - object1->height()/2;
	top2 = object2->mPosition.y - object1->height()/2;
	bottom1 = top1 + object1->height()/2;
	bottom2 = top2 + object2->height()/2;

	if (bottom1 < top2) return(0);
	if (top1 > bottom2) return(0);

	if (right1 < left2) return(0);
	if (left1 > right2) return(0);

	return(1);
};
//-----------------------------------------------------------------------------
// Name : AnimateObjects () (Private)
// Desc : Animates the objects we currently have loaded.
//-----------------------------------------------------------------------------
void CGameApp::AnimateObjects()
{
	m_pPlayer->Update(m_Timer.GetTimeElapsed());
	m_pPlayer2->Update(m_Timer.GetTimeElapsed());
}

//-----------------------------------------------------------------------------
// Name : DrawObjects () (Private)
// Desc : Draws the game objects
//-----------------------------------------------------------------------------
void CGameApp::DrawObjects()
{
	static UINT fTimer;
	m_pBBuffer->reset();

//	m_imgBackground.Paint(m_pBBuffer->getDC(), 0, 0);
	DrawBackground();

	

	m_pPlayer->Draw();

	m_pPlayer2->Draw();

	if (enemies.size() < 3) {
		Enemy enemy(m_pBBuffer);
		enemy.m_pSprite->mPosition = Vec2(50, 100);
		enemies.push_back(enemy);
		Enemy enemy2(m_pBBuffer);
		enemy2.shootCooldown = 100;
		enemy2.m_pSprite->mPosition = Vec2(250, 100);
		enemies.push_back(enemy2);
		Enemy enemy3(m_pBBuffer);
		enemy3.m_pSprite->mPosition = Vec2(450, 100);
		enemies.push_back(enemy3);
	}
	
	for (auto &it : enemies) {
		it.shootCooldown--;
		it.m_pSprite->draw();
		it.move();
		it.shoot();
		if (Sprite_Collide(it.m_pSprite, m_pPlayer->m_pSprite)) {
			fTimer = SetTimer(m_hWnd, 1, 70, NULL);
			m_pPlayer->Explode();
			m_pPlayer->m_pSprite->mPosition = Vec2(400, 400);
		}
		if (Sprite_Collide(it.m_pSprite, m_pPlayer2->m_pSprite)) {
			fTimer = SetTimer(m_hWnd, 1, 70, NULL);
			m_pPlayer2->Explode();
			m_pPlayer2->m_pSprite->mPosition = Vec2(400, 400);
		}
	}

	for (auto &it : bullets) {
		it.m_pSprite->draw();
		it.Move1();
		if (Sprite_Collide(it.m_pSprite, m_pPlayer2->m_pSprite)) {
			fTimer = SetTimer(m_hWnd, 1, 50, NULL);
			m_pPlayer2->Explode();
			it = NULL;	
		}
	}

	for (auto &it : bullets2) {
		it.m_pSprite->draw();
		it.Move2();
		if (Sprite_Collide(it.m_pSprite, m_pPlayer->m_pSprite)) {
			fTimer = SetTimer(m_hWnd, 1, 50, NULL);
			m_pPlayer->Explode();
			it = NULL;
		}
	}

	for (auto &it : bullets3) {
		it.m_pSprite->draw();
		it.Move3();
		if (Sprite_Collide(it.m_pSprite, m_pPlayer2->m_pSprite)) {
			fTimer = SetTimer(m_hWnd, 1, 50, NULL);
			it = NULL;
			m_pPlayer2->Explode();
		}
		if (Sprite_Collide(it.m_pSprite, m_pPlayer->m_pSprite)) {
			fTimer = SetTimer(m_hWnd, 1, 50, NULL);
			m_pPlayer->Explode();
			it = NULL;
		}
	}

	if (Sprite_Collide(m_pPlayer->m_pSprite, m_pPlayer2->m_pSprite)) {
		fTimer = SetTimer(m_hWnd, 1, 70, NULL);
		m_pPlayer->Explode();
		m_pPlayer2->Explode();
		m_pPlayer->m_pSprite->mPosition = Vec2(1300, 500);
		m_pPlayer2->m_pSprite->mPosition = Vec2(100, 500);

	}

	enemies.remove_if([](Enemy c) {
		return c.m_pSprite->mPosition.x > 1300 ? true : false;
	});

	bullets3.remove_if([](Bullet c) {
		return c.m_pSprite->mPosition.y > 700 ? true : false;
	});


	m_pBBuffer->present();
}

void CGameApp::Save_game()
{
	
	ofstream fout;
	fout.open("game.txt", ofstream::out | ofstream::trunc); // the text file is freed every time

	fout << "Player1:" << " " << m_pPlayer->m_pSprite->mPosition.x << " " << m_pPlayer->m_pSprite->mPosition.y << endl;
	fout << "Player2:" << " " << m_pPlayer2->m_pSprite->mPosition.x << " " << m_pPlayer2->m_pSprite->mPosition.y << endl;

	fout << "Player1Lives:" << " " << m_pPlayer->lives << endl;
	fout << "Player2Lives:" << " " << m_pPlayer->lives << endl;

	fout.close();
}

// Function that will read data from a text file and initialize the game with the last coordinates
// of the planes (before the moment of save)
void CGameApp::Load_game()
{
	ifstream fin("game.txt");

	double p1x, p2x;
	int p1l, p2l;
	double p1y, p2y;

	string stuff;

	fin >> stuff >> p1x >> p1y;
	fin >> stuff >> p2x >> p2y;
	
	m_pPlayer->m_pSprite->mVelocity = Vec2(0, 0);
	m_pPlayer->Position() = Vec2(p1x, p1y);

	m_pPlayer2->m_pSprite->mVelocity = Vec2(0, 0);
	m_pPlayer2->Position() = Vec2(p2x, p2y);
	
	fin >> stuff >> p1l;
	fin >> stuff >> p2l;

	m_pPlayer->lives = p1l;
	m_pPlayer2->lives = p2l;

	fin.close();
}
