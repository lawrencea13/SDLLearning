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

	if (isLocalPlayer && input) {
		sendInput(0);
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
	// reset on new packet
	xchange = 0;
	ychange = 0;

	if (input.moveX == -1) xchange -= 10;
	if (input.moveX == 1)  xchange += 10;

	if (input.moveY == -1) ychange -= 10;
	if (input.moveY == 1)  ychange += 10;

	destRect.x += xchange;
	destRect.y += ychange;
}
#else
void Player::ApplyServerState(const ServerStatePacket& packet)
{
}
#endif


void Player::sendInput(uint32_t currentFrame)
{
	PlayerInputPacket packet;
	NetworkManager& net = gameInstance.getNetworkManager();
	packet.steamID = net.getLocalSteamID();
	packet.moveX = input->isKeyDown(SDLK_a) ? -1 : input->isKeyDown(SDLK_d) ? 1 : 0;
	packet.moveY = input->isKeyDown(SDLK_w) ? -1 : input->isKeyDown(SDLK_s) ? 1 : 0;
	packet.attack = input->isMouseButtonDown(SDL_BUTTON_LEFT);
	packet.clientInputFrame = currentFrame;

	net.sendPacket(PACKET_INPUT_COMMAND, &packet, sizeof(packet));
}

void Player::storeState(uint32_t frame)
{
	PlayerState state;
	state.x = destRect.x;
	state.y = destRect.y;
	state.xchange = xchange;
	state.ychange = ychange;
	state.frame = frame;
	stateHistory.push_back(state);
	if (stateHistory.size() > MAX_HISTORY_SIZE) {
		stateHistory.pop_front();
	}
}

PlayerState Player::getStateAtFrame(uint32_t frame) const
{
	if (stateHistory.empty()) {
		// Handle the case where there's no history (e.g., at the very beginning)
		PlayerState defaultState = { destRect.x, destRect.y, 0, 0, frame };
		return defaultState;
	}

	// Find the closest state *before* the given frame
	auto closest = std::min_element(stateHistory.begin(), stateHistory.end(),
		[frame](const PlayerState& a, const PlayerState& b) {
			return std::abs(static_cast<int>(a.frame - frame)) < std::abs(static_cast<int>(b.frame - frame));
		});

	return *closest; // Return the closest state
}
