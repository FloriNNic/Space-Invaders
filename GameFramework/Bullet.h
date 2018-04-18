#pragma once

#include "Main.h"
#include "Sprite.h"


class Bullet
{
public:
	Bullet(const BackBuffer *pBackBuffer);
	~Bullet();

	Sprite*m_pSprite;

	void Move1();
	void Move2();
	void Move3();
};


