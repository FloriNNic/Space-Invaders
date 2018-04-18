#pragma once
#include "Main.h"
#include "Sprite.h"

class Enemy
{
public:
	Enemy(const BackBuffer *pBackBuffer);
	~Enemy();

	Sprite*					m_pSprite;
	int shootCooldown = 200;
	void move();
	void shoot();

};
