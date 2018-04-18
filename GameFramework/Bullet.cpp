#include "Bullet.h"

Bullet::Bullet(const BackBuffer *pBackBuffer)
{
	m_pSprite = new Sprite("data/bullet.bmp", "data/bmask.bmp");
	m_pSprite->setBackBuffer(pBackBuffer);
}

Bullet::~Bullet(){}

void Bullet::Move1()
{
	this->m_pSprite->mPosition.x -= 5;
}

void Bullet::Move2()
{
	this->m_pSprite->mPosition.x += 5;
}

void Bullet::Move3() {
	this->m_pSprite->mPosition.y += 2;
}
