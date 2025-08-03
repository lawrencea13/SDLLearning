#include "Player.h"
#include "Game.h"
#include "Logger.h"

#define LOG(...) Logger::Log(__VA_ARGS__)

// client player logic


Player::Player(int x, int y, int w, int h, std::shared_ptr<SDL_Texture> tex, Game& game) : GameObject(x, y, w, h, tex, game), input(game.getInputHandler()), posX(float(x)), posY(float(y))
{
	collisionEnabled = true;
}

void Player::Update() {
	/*
	//last frame x/y change
	lf_xchange = xchange;
	lf_ychange = ychange;
	xchange = 0;
	ychange = 0;

	uint32_t currentFrame = gameInstance.getFrameCount();
	float deTime = gameInstance.getDeltaTime();

	if (localTesting) {
		return localTestUpdate();
	}

	if (isLocalPlayer && input) {

		int moveX = 0;
		int moveY = 0;

		if (input->isKeyDown(SDLK_w)) {
			ychange -= speed * deTime;
			moveY = -1;
		}
		if (input->isKeyDown(SDLK_a)) {
			xchange -= speed * deTime;
			moveX = -1;
		}
		if (input->isKeyDown(SDLK_s)) {
			ychange += speed * deTime;
			moveY = 1;
		}
		if (input->isKeyDown(SDLK_d)) {
			xchange += speed * deTime;
			moveX = 1;
		}

		InputFrame inputFrame{ moveX, moveY, gameInstance.getFrameCount() };
		inputHistory.push_back(inputFrame);

		if (inputHistory.size() > MAX_HISTORY_SIZE)
			inputHistory.pop_front();

		sendInput(currentFrame);

		
	}

	
	if (isLocalPlayer && !localTesting && bufCount > 0) {
		auto& lastServerState = stateBuf[bufHead];
		// 1. Snap to server authoritative position
		destRect.x = int(lastServerState.x);
		destRect.y = int(lastServerState.y);

		//// 2. Remove confirmed inputs
		while (!inputHistory.empty() && inputHistory.front().frame <= lastServerState.inputFrame) {
			inputHistory.pop_front();
		}

		//// 3. Re-apply unconfirmed inputs
		float deTime = gameInstance.getDeltaTime();
		for (const auto& input : inputHistory) {
			destRect.x += static_cast<int>(input.moveX * speed * deTime);
			destRect.y += static_cast<int>(input.moveY * speed * deTime);
		}
	}


	handleCollision();

	
	destRect.x += xchange;
	destRect.y += ychange;

	*/

	lf_xchange = xchange;
	lf_ychange = ychange;
	xchange = 0;
	ychange = 0;

	uint32_t currentFrame = gameInstance.getFrameCount();
	float deTime = gameInstance.getDeltaTime();

	if (localTesting) {
		return localTestUpdate();
	}

	if (isLocalPlayer && input) {
		int moveX = 0, moveY = 0;

		if (input->isKeyDown(SDLK_w)) { ychange -= speed * deTime; moveY = -1; }
		if (input->isKeyDown(SDLK_a)) { xchange -= speed * deTime; moveX = -1; }
		if (input->isKeyDown(SDLK_s)) { ychange += speed * deTime; moveY = 1; }
		if (input->isKeyDown(SDLK_d)) { xchange += speed * deTime; moveX = 1; }

		InputFrame inputFrame{ moveX, moveY, currentFrame };
		inputHistory.push_back(inputFrame);

		if (inputHistory.size() > MAX_HISTORY_SIZE)
			inputHistory.pop_front();

		sendInput(currentFrame);
	}

	// --- Client-side Prediction & Reconciliation ---
	if (isLocalPlayer && !localTesting && bufCount > 0) {
		auto& lastServerState = stateBuf[bufHead];
		// 1. Snap to server authoritative position
		posX = lastServerState.x;
		posY = lastServerState.y;

		// 2. Remove confirmed inputs
		while (!inputHistory.empty() && inputHistory.front().frame <= lastServerState.inputFrame) {
			inputHistory.pop_front();
		}

		// 3. Re-apply unconfirmed inputs as FLOATS
		for (const auto& input : inputHistory) {
			posX += input.moveX * speed * deTime;
			posY += input.moveY * speed * deTime;
		}
	}

	// Add this frame's local input (if any) as float
	posX += xchange;
	posY += ychange;

	// Handle collision using float position (you may need to adapt your collision to support float)
	// Example: set destRect.x/y to float before collision
	destRect.x = int(std::round(posX));
	destRect.y = int(std::round(posY));
	handleCollision();

	// Sync float positions back from collision result
	posX = float(destRect.x);
	posY = float(destRect.y);
}

void Player::localTestUpdate()
{
	float deTime = gameInstance.getDeltaTime();
	int moveX = 0;
	int moveY = 0;

	if (input->isKeyDown(SDLK_w)) {
		ychange -= speed * deTime;
		moveY = -1;
	}
	if (input->isKeyDown(SDLK_a)) {
		xchange -= speed * deTime;
		moveX = -1;
	}
	if (input->isKeyDown(SDLK_s)) {
		ychange += speed * deTime;
		moveY = 1;
	}
	if (input->isKeyDown(SDLK_d)) {
		xchange += speed * deTime;
		moveX = 1;
	}

	InputFrame inputFrame{ moveX, moveY, gameInstance.getFrameCount() };
	inputHistory.push_back(inputFrame);

	if (inputHistory.size() > MAX_HISTORY_SIZE)
		inputHistory.pop_front();

	handleCollision();


	destRect.x += xchange;
	destRect.y += ychange;
}

void Player::Render() {
	if(localTesting ) {
		return localTestRender();
	}

	double now = Seconds(Clock::now().time_since_epoch()).count();
	double renderTime = now - INTERP_DELAY;


	if (bufCount < 2) {
		drawX = stateBuf[bufHead].x;
		drawY = stateBuf[bufHead].y;
	}
	else {

		size_t i = bufHead;
		for (size_t s = 0; s < bufCount; ++s) {
			size_t prev = (i + STATE_BUFFER_SIZE - 1) % STATE_BUFFER_SIZE;
			if (stateBuf[prev].time <= renderTime) {

				double span = stateBuf[i].time - stateBuf[prev].time;
				double t = span > 1e-6
					? std::clamp((renderTime - stateBuf[prev].time) / span, 0.0, 1.0)
					: 0.0;
				drawX = float(stateBuf[prev].x * (1 - t) + stateBuf[i].x * t);
				drawY = float(stateBuf[prev].y * (1 - t) + stateBuf[i].y * t);
				break;
			}
			i = prev;
		}
	}

	if (isLocalPlayer) {

		SDL_Rect previewRect = destRect;
		previewRect.x = int(std::round(drawX));
		previewRect.y = int(std::round(drawY));

		// Set alpha to 0.2 (51 out of 255)
		SDL_SetTextureAlphaMod(texture.get(), 51); 
		SDL_RenderCopy(renderer, texture.get(), nullptr, &previewRect);


		SDL_Rect serverRect = destRect;
		serverRect.x = int(std::round(posX));
		serverRect.y = int(std::round(posY));
		SDL_SetTextureAlphaMod(texture.get(), 255); // Restore to opaque
		SDL_RenderCopy(renderer, texture.get(), nullptr, &serverRect);
	}
	else {
		// For other players, just draw interpolated position as usual
		SDL_Rect r = destRect;
		r.x = int(std::round(drawX));
		r.y = int(std::round(drawY));
		SDL_SetTextureAlphaMod(texture.get(), 255);
		SDL_RenderCopy(renderer, texture.get(), nullptr, &destRect);
	}

}

void Player::localTestRender() {
	SDL_RenderCopy(renderer, texture.get(), nullptr, &destRect);
}

void Player::ApplyServerState(const ServerStatePacket& packet)
{
	/*
	if(localTesting) {
		
		return;
	}


	//// also reset your sim position & history here if you do prediction…
	setLocation(static_cast<int>(packet.posX), static_cast<int>(packet.posY));
	//printf("Player position updated to (%f, %f)\n", packet.posX, packet.posY);
	LOG("[CLIENT] Received server state for player id=%d at tick=%llu (t=%.3f) pos=(%d,%d)",
		this->getSteamID(), gameInstance.clientTickNum, SDL_GetTicks() / 1000.0f, packet.posX, packet.posY);



	

	double now = Seconds(Clock::now().time_since_epoch()).count();
	bufHead = (bufHead + 1) % STATE_BUFFER_SIZE;
	stateBuf[bufHead] = { packet.posX, packet.posY, now, packet.inputFrame};
	if (bufCount < STATE_BUFFER_SIZE) ++bufCount;
	//TODO: latestConfirmedFrame = packet.inputFrame;
	// maybe later for reconciliation
	*/

	if (localTesting) return;

	// Reset sim position (float), for prediction/reconciliation
	posX = packet.posX;
	posY = packet.posY;

	// Also update destRect (for direct movement fallback)
	destRect.x = int(std::round(posX));
	destRect.y = int(std::round(posY));

	// Update interpolation buffer (for rendering smoothing)
	double now = Seconds(Clock::now().time_since_epoch()).count();
	bufHead = (bufHead + 1) % STATE_BUFFER_SIZE;
	stateBuf[bufHead] = { packet.posX, packet.posY, now, packet.inputFrame };
	if (bufCount < STATE_BUFFER_SIZE) ++bufCount;
}


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
		.sendPacket(PACKET_INPUT_COMMAND, &pkt, sizeof(pkt), 0, ENET_PACKET_FLAG_UNSEQUENCED);

	//LOG("[CLIENT] Sent input for tick=%llu at %.3f", gameInstance.clientTickNum, SDL_GetTicks() / 1000.0f);
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

