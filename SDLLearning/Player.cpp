#include "Player.h"
#include "Game.h"

Player::Player(int x, int y, int w, int h, std::shared_ptr<SDL_Texture> tex, Game& game) : GameObject(x, y, w, h, tex, game), input(game.getInputHandler())
{
	collisionEnabled = true;
}

void Player::Update() {
	// currently only updating with applyInput()
#ifndef DEDICATED_SERVER
	xchange = 0;
	ychange = 0;
	uint32_t currentFrame = gameInstance.getFrameCount();

	if (isLocalPlayer && input) {
		sendInput(currentFrame);
	}
	//last frame x/y change
	lf_xchange = xchange;
	lf_ychange = ychange;

	if (!input) {
		return;
	}

	if (input->isKeyDown(SDLK_w)) {
		ychange -= 10;
	}
	if (input->isKeyDown(SDLK_a)) {
		xchange -= 10;
	}
	if (input->isKeyDown(SDLK_s)) {
		ychange += 10;
	}
	if (input->isKeyDown(SDLK_d)) {
		xchange += 10;
	}


	handleCollision();

	
	destRect.x += xchange;
	destRect.y += ychange;

	storeState(currentFrame);
#endif
}

void Player::Render() {

	Camera& camera = gameInstance.getCamera();
	SDL_Rect screenRect = camera.apply(destRect);

	SDL_RenderCopy(renderer, texture.get(), nullptr, &screenRect);

}

#ifdef DEDICATED_SERVER
void Player::ApplyInput(const PlayerInputPacket& input)
{

}
#else
void Player::ApplyServerState(const ServerStatePacket& packet)
{
}
#endif


void Player::sendInput(uint32_t currentFrame)
{
	if (!input) return;

	PlayerInputPacket pkt{};
	if (input->isKeyDown(SDLK_a)) pkt.moveX = -1;
	else if (input->isKeyDown(SDLK_d)) pkt.moveX = +1;
	else                                pkt.moveX = 0;

	if (input->isKeyDown(SDLK_w)) pkt.moveY = -1;
	else if (input->isKeyDown(SDLK_s)) pkt.moveY = +1;
	else                                pkt.moveY = 0;

	pkt.attack = input->isMouseButtonDown(SDL_BUTTON_LEFT);

	pkt.clientInputFrame = currentFrame;

	gameInstance.getNetworkManager()
		.sendPacket(PACKET_INPUT_COMMAND, &pkt, sizeof(pkt), 0);
}

void Player::storeState(uint32_t frame)
{
	if (stateHistory.size() >= MAX_HISTORY_SIZE) {
		stateHistory.pop_front();
	}

	PlayerState state;
	state.x = destRect.x;
	state.y = destRect.y;
	state.xchange = xchange;
	state.ychange = ychange;
	state.frame = frame;
	stateHistory.push_back(state);
}

PlayerState Player::getStateAtFrame(uint32_t frame) const
{
	if (stateHistory.empty()) {
		PlayerState defaultState = { destRect.x, destRect.y, 0, 0, frame };
		return defaultState;
	}

	auto closest = std::min_element(stateHistory.begin(), stateHistory.end(),
		[frame](const PlayerState& a, const PlayerState& b) {
			return std::abs(static_cast<int>(a.frame - frame))
				< std::abs(static_cast<int>(b.frame - frame));
		});

	return *closest;
}
