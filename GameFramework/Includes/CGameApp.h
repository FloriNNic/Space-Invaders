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
#include "BackBuffer.h"
#include "ImageFile.h"
#include "../Bullet.h"
#include "../Enemy.h"
#include <list>
#include <string>
#include <vector>
#include <stdio.h>
#include <fstream>
#include <windows.h>
#include <MMSystem.h>

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
	LRESULT	 DisplayWndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam );
	bool		InitInstance( LPCTSTR lpCmdLine, int iCmdShow );
	int		 BeginGame( );
	bool		ShutDown( );
	USHORT					Width;
	USHORT					Height;
	BackBuffer*				m_pBBuffer;
	std::list<Bullet>	    bullets;
	std::list<Bullet>		bullets2;
	std::list<Bullet>		bullets3;
	std::list<Enemy>		enemies;
	HWND					m_hWnd;			 // Main window HWND
	
private:
	//-------------------------------------------------------------------------
	// Private Functions for This Class
	//-------------------------------------------------------------------------
	bool		BuildObjects	  ( );
	void		ReleaseObjects	( );
	void		FrameAdvance	  ( );
	bool		CreateDisplay	 ( );
	void		SetupGameState	( );
	void		AnimateObjects	( );
	void		DrawObjects	   ( );
	void		ProcessInput	  ( );
	void        Save_game();
	void        Load_game();


	int CGameApp::Sprite_Collide(Sprite * object1, Sprite * object2);
	void DrawBackground();
	
	//-------------------------------------------------------------------------
	// Private Static Functions For This Class
	//-------------------------------------------------------------------------
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	//-------------------------------------------------------------------------
	// Private Variables For This Class
	//-------------------------------------------------------------------------
	CTimer				  m_Timer;			// Game timer
	ULONG				   m_LastFrameRate;	// Used for making sure we update only when fps changes.
	
	HICON				   m_hIcon;			// Window Icon
	HMENU				   m_hMenu;			// Window Menu
	
	bool					m_bActive;		  // Is the application active ?

	ULONG				   m_nViewX;		   // X Position of render viewport
	ULONG				   m_nViewY;		   // Y Position of render viewport
	ULONG				   m_nViewWidth;	   // Width of render viewport
	ULONG				   m_nViewHeight;	  // Height of render viewport

	POINT				   m_OldCursorPos;	 // Old cursor position for tracking
	HINSTANCE				m_hInstance;

	CImageFile				m_imgBackground;
	CImageFile				m_imgBackground1;


	CPlayer*				m_pPlayer;
	CPlayer*				m_pPlayer2;
};

#endif // _CGAMEAPP_H_