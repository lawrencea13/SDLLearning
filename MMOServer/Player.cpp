#include "Player.h"
#include "Game.h"

// server player logic

Player::Player(int x, int y, int w, int h, Game& game) : GameObject(x, y, w, h, game)
{
	collisionEnabled = true;
}

void Player::Update() {
	// another temp meh
	//last frame x/y change
	lf_xchange = xchange;
	lf_ychange = ychange;
	// reset for this frame
	xchange = 0;
	ychange = 0;
	float deTime = gameInstance.getDeltaTime();

	ychange += s_ychange * speed * deTime;
	xchange += s_xchange * speed * deTime;

	handleCollision();

	destRect.x += xchange;
	destRect.y += ychange;
}

void Player::ApplyInput(const PlayerInputPacket& input)
{
	s_xchange = input.moveX;
	s_ychange = input.moveY;
	s_lastReceivedFrame = input.clientInputFrame;
}
