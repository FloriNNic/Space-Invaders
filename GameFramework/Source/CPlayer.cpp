//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//
// Desc: This file stores the player object class. This class performs tasks
//       such as player movement, some minor physics as well as rendering.
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CPlayer Specific Includes
//-----------------------------------------------------------------------------
#include "CPlayer.h"
#include "CGameApp.h"

extern CGameApp g_App;

//-----------------------------------------------------------------------------
// Name : CPlayer () (Constructor)
// Desc : CPlayer Class Constructor
//-----------------------------------------------------------------------------
CPlayer::CPlayer(const BackBuffer *pBackBuffer):mBackBuffer(pBackBuffer),mNewDirection(DIRECTION::DIR_FORWARD)//,lives(3)
{
	//m_pSprite = new Sprite("data/planeimg.bmp", "data/planemask.bmp");
	m_pSprite = new Sprite("data/planeimgandmask.bmp", RGB(0xff,0x00, 0xff));
	m_pSprite->setBackBuffer( pBackBuffer );
	m_eSpeedState = SPEED_STOP;
	m_fTimer = 0;

	// Animation frame crop rectangle
	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = 128;
	r.bottom = 128;

	m_pExplosionSprite	= new AnimatedSprite("data/explosion.bmp", "data/explosionmask.bmp", r, 16);
	m_pExplosionSprite->setBackBuffer( pBackBuffer );
	m_bExplosion		= false;
	m_iExplosionFrame	= 0;
}

//-----------------------------------------------------------------------------
// Name : ~CPlayer () (Destructor)
// Desc : CPlayer Class Destructor
//-----------------------------------------------------------------------------
CPlayer::~CPlayer()
{
	delete m_pSprite;
	delete m_pExplosionSprite;
}

void CPlayer::Update(float dt)
{
	// Update sprite
	m_pSprite->update(dt);


	// Get velocity
	double v = m_pSprite->mVelocity.Magnitude();

	// NOTE: for each async sound played Windows creates a thread for you
	// but only one, so you cannot play multiple sounds at once.
	// This creation/destruction of threads also leads to bad performance
	// so this method is not recommanded to be used in complex projects.

	// update internal time counter used in sound handling (not to overlap sounds)
	m_fTimer += dt;

	// A FSM is used for sound manager 
	switch(m_eSpeedState)
	{
	case SPEED_STOP:
		if(v > 35.0f)
		{
			m_eSpeedState = SPEED_START;
			PlaySound(TEXT("data\\jet-start.wav"), NULL, SND_FILENAME | SND_ASYNC);
			m_fTimer = 0;
		}
		break;
	case SPEED_START:
		if(v < 25.0f)
		{
			m_eSpeedState = SPEED_STOP;
			PlaySound("data/jet-stop.wav", NULL, SND_FILENAME | SND_ASYNC);
			m_fTimer = 0;
		}
		else
			if(m_fTimer > 1.f)
			{
				PlaySound("data/jet-cabin.wav", NULL, SND_FILENAME | SND_ASYNC);
				m_fTimer = 0;
			}
		break;
	}

	// NOTE: For sound you also can use MIDI but it's Win32 API it is a bit hard
	// see msdn reference: http://msdn.microsoft.com/en-us/library/ms711640.aspx
	// In this case you can use a C++ wrapper for it. See the following article:
	// http://www.codeproject.com/KB/audio-video/midiwrapper.aspx (with code also)
}

void CPlayer::Draw()
{
	if (fireCooldown > 1) {
		fireCooldown--;
	}
	if (!m_bExplosion) {
		m_pSprite->draw();
	}
	else {
		m_pExplosionSprite->draw();
	}
}

void CPlayer::Move(ULONG ulDirection)
{

	if (ulDirection & CPlayer::DIR_LEFT) {
		if (Position().x < 50) {
			Position().x = 50;
			m_pSprite->mVelocity.x = 0;
		}
		else
			m_pSprite->mVelocity.x -= 3;
	}

	else if( ulDirection & CPlayer::DIR_RIGHT ){
		if (Position().x > 1380) {
			Position().x = 1380;
			m_pSprite->mVelocity.x = 0;
		}
		else
			m_pSprite->mVelocity.x += 3;
	}
		

	else if( ulDirection & CPlayer::DIR_FORWARD ){
		if (Position().y < 80) {
			Position().y = 80;
			m_pSprite->mVelocity.x = 0;
		}
		else
			m_pSprite->mVelocity.y -= 3;
	}
	

	else if (ulDirection & CPlayer::DIR_BACKWARD) {
		if (Position().y > 750) {
			Position().y = 750;
			m_pSprite->mVelocity.x = 0;
		}
		else
			m_pSprite->mVelocity.y += 3;

	}

 	else
		m_pSprite->mVelocity = Vec2(0, 0);
}

/*void CPlayer::Move(ULONG ulDirection)
{
	if (ulDirection & CPlayer::DIR_LEFT)
		m_pSprite->mVelocity.x -= .7;

	if (m_pSprite->mPosition.x < 0) {
		m_pSprite->mPosition.x = 0;
		m_pSprite->mVelocity.x = 0;
	}

	if (ulDirection & CPlayer::DIR_RIGHT)
		m_pSprite->mVelocity.x += .7;

	if (m_pSprite->mPosition.x > g_App.Width) {
		m_pSprite->mPosition.x = g_App.Width;
		m_pSprite->mVelocity.x = 0;
	}

	if (ulDirection & CPlayer::DIR_FORWARD)
		m_pSprite->mVelocity.y -= .7;

	if (m_pSprite->mPosition.y < 0) {
		m_pSprite->mPosition.y = 0;
		m_pSprite->mVelocity.y = 0;
	}

	if (ulDirection & CPlayer::DIR_BACKWARD)
		m_pSprite->mVelocity.y += .7;

	if (m_pSprite->mPosition.y > g_App.Height) {
		m_pSprite->mPosition.y = g_App.Height;
		m_pSprite->mVelocity.y = 0;
	}
}
*/
Vec2& CPlayer::Position()
{
	return m_pSprite->mPosition;
}

Vec2& CPlayer::Velocity()
{
	return m_pSprite->mVelocity;
}

void CPlayer::Explode()
{
	m_pExplosionSprite->mPosition = m_pSprite->mPosition;
	m_pExplosionSprite->SetFrame(0);
	DecreaseLives();

	//PlaySound("data/fart.wav", NULL, SND_FILENAME | SND_SYNC);
	
	m_bExplosion = true;
}

bool CPlayer::AdvanceExplosion()
{
	if(m_bExplosion)
	{
		m_pExplosionSprite->SetFrame(m_iExplosionFrame++);
		if(m_iExplosionFrame==m_pExplosionSprite->GetFrameCount())
		{
			m_bExplosion = false;
			m_iExplosionFrame = 0;
			m_pSprite->mVelocity = Vec2(0,0);
			m_eSpeedState = SPEED_STOP;
			return false;
		}
	}

	return true;
}

void CPlayer::Shoot()
{
	if (fireCooldown < 5) {
		Bullet bullet(g_App.m_pBBuffer);
		bullet.m_pSprite->mPosition = this->m_pSprite->mPosition;
		g_App.bullets.push_back(bullet);
		fireCooldown = 200;
	}
}

void CPlayer::Shoot2()
{
	if (fireCooldown < 5) {
		Bullet bullet(g_App.m_pBBuffer);
		bullet.m_pSprite->mPosition = this->m_pSprite->mPosition;
		g_App.bullets2.push_back(bullet);
		fireCooldown = 200;
	}
}


void CPlayer::RotateLeft()
{
	auto position = m_pSprite->mPosition;
	auto velocity = m_pSprite->mVelocity;

	delete m_pSprite;

	switch (mNewDirection)
	{
	case DIRECTION::DIR_FORWARD:
		m_pSprite = new Sprite("data/leftRotate.bmp", "data/leftPlaneMask.bmp");
		mNewDirection = DIRECTION::DIR_LEFT;
		break;
	case DIRECTION::DIR_BACKWARD:
		m_pSprite = new Sprite("data/rightRotate.bmp", "data/rightPlaneMask.bmp");
		mNewDirection = DIRECTION::DIR_RIGHT;
		break;
	case DIRECTION::DIR_LEFT:
		mNewDirection = DIRECTION::DIR_BACKWARD;
		m_pSprite = new Sprite("data/downRotate.bmp", "data/downPlaneMask.bmp");
		break;
	case DIRECTION::DIR_RIGHT:
		m_pSprite = new Sprite("data/PlaneImg.bmp", "data/PlaneMask.bmp");
		mNewDirection = DIRECTION::DIR_FORWARD;
		break;
	}
	m_pSprite->mPosition = position;
	m_pSprite->mVelocity = velocity;
	m_pSprite->setBackBuffer(mBackBuffer);
}

void CPlayer::RotateRight()
{
	auto position = m_pSprite->mPosition;
	auto velocity = m_pSprite->mVelocity;

	delete m_pSprite;

	switch (mNewDirection)
	{
	case DIRECTION::DIR_FORWARD:
		m_pSprite = new Sprite("data/rightRotate.bmp", "data/rightPlaneMask.bmp");
		mNewDirection = DIRECTION::DIR_RIGHT;
		break;
	case DIRECTION::DIR_BACKWARD:
		m_pSprite = new Sprite("data/leftRotate.bmp", "data/leftPlaneMask.bmp");
		mNewDirection = DIRECTION::DIR_LEFT;
		break;
	case DIRECTION::DIR_LEFT:
		m_pSprite = new Sprite("data/PlaneImg.bmp", "data/PlaneMask.bmp");
		mNewDirection = DIRECTION::DIR_FORWARD;
		break;
	case DIRECTION::DIR_RIGHT:
		m_pSprite = new Sprite("data/downRotate.bmp", "data/downPlaneMask.bmp");
		mNewDirection = DIRECTION::DIR_BACKWARD;
		break;
	}

	m_pSprite->mPosition = position;
	m_pSprite->mVelocity = velocity;
	m_pSprite->setBackBuffer(mBackBuffer);
}


void CPlayer::DecreaseLives() {
	lives--;
}