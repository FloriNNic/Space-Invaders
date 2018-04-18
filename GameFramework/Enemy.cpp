#include "Enemy.h"
#include "CGameApp.h"

extern CGameApp g_App;

Enemy::Enemy(const BackBuffer *pBackBuffer)
{
	m_pSprite = new Sprite("data/enemy.bmp", RGB(0xff, 0x00, 0xff));
	m_pSprite->setBackBuffer(pBackBuffer);
}


Enemy::~Enemy()
{
}

void Enemy::move()
{
	this->m_pSprite->mPosition.x += .8;
}

void Enemy::shoot()
{
	if (shootCooldown < 5) {
		Bullet bullet(g_App.m_pBBuffer);
		bullet.m_pSprite->mPosition = this->m_pSprite->mPosition;
		g_App.bullets3.push_back(bullet);
		shootCooldown = 300;
	}
}
