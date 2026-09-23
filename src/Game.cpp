#include "Game.h"
#include <SDL3_image/SDL_image.h>
#include <cmath>

// Helper function
SDL_Texture* LoadTexture(SDL_Renderer* renderer, const char* path)
{
	SDL_Surface* surface = IMG_Load(path);
	if (!surface) {
		SDL_Log("IMG_Load failed for '%s': %s", path, SDL_GetError());
		return nullptr;
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_DestroySurface(surface);
	return texture;
}

Game::Game() {}
Game::~Game() { Shutdown(); }

bool Game::Initialize()
{
	SDL_SetAppMetadata("PixelQuest", "0.1.0", "com.example.pixelquest");

	// Init SDL video subsystem
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("SDL_Init failed: %s", SDL_GetError());
		return false;
	}

	// Create window and renderer together to avoid flicker
	if (!SDL_CreateWindowAndRenderer(
			"PixelQuest v0.1", m_screenWidth, m_screenHeight,
			SDL_WINDOW_RESIZABLE,
			&m_window, &m_renderer)) {
		SDL_Log("Failed to create window/renderer: %s", SDL_GetError());
		return false;
	}

	// Set renderer clear color (dark blue)
	SDL_SetRenderDrawColor(m_renderer, 30, 30, 60, 255);

	// Load textures
	m_playerTexture = LoadTexture(m_renderer, "assets/player.png");
	if (!m_playerTexture) {
		SDL_LogWarn(SDL_LOG_CATEGORY_RENDER,
			"Player texture not found, using fallback");
	}

	m_tileTexture = LoadTexture(m_renderer, "assets/tiles.png");
	if (!m_tileTexture) {
		SDL_LogWarn(SDL_LOG_CATEGORY_RENDER,
			"Tile texture not found, using fallback");
	}

	m_running = true;
	return true;
}

void Game::Run()
{
	const int TARGET_FPS = 60;
	const int FRAME_TIME = 1000 / TARGET_FPS;
	Uint32 frameStart;
	float deltaTime = 0.0f;

	while (m_running) {
		frameStart = SDL_GetTicks();

		if (!HandleEvents()) break;
		Update(deltaTime);
		Render();

		// Frame timing: cap to target FPS
		Uint32 frameTime = SDL_GetTicks() - frameStart;
		deltaTime = frameTime / 1000.0f;
		if (frameTime < FRAME_TIME) {
			SDL_Delay(FRAME_TIME - frameTime);
		}
	}
}

void Game::Shutdown()
{
	if (m_playerTexture) {
		SDL_DestroyTexture(m_playerTexture);
		m_playerTexture = nullptr; 
	}

	if (m_tileTexture) {
		SDL_DestroyTexture(m_tileTexture);
		m_tileTexture = nullptr;
	}

	if (m_renderer) {
		SDL_DestroyRenderer(m_renderer);
		m_renderer = nullptr;
	}

	if (m_window) {
		SDL_DestroyWindow(m_window);
		m_window = nullptr; 
	}
	SDL_Quit();
}

bool Game::HandleEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_EVENT_QUIT:
				return false;
			case SDL_EVENT_KEY_DOWN:
				if (event.key.key == SDLK_ESCAPE)
					return false;
				break;
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				if (event.button.button == SDL_BUTTON_LEFT) {
					SDL_Log("Clicked at (%.0f, %.0f)",
						event.button.x, event.button.y);
				}
			default:
				break;
		}
	}

	// Poll keyboard state for continuous movement
	int numKeys;
	const bool *keys = SDL_GetKeyboardState(&numKeys);

	float dx = 0.0f, dy = 0.0f;
	if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A]) dx -= 1.0f;
	if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D]) dx += 1.0f;
	if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W]) dy -= 1.0f;
	if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S]) dy += 1.0f;

	// Normalize diagonal movement
	if (dx != 0.0f && dy != 0.0f) {
		float len = sqrtf(dx * dx + dy * dy);
		dx /= len;
		dy /= len;
	}

	m_playerX += dx * m_playerSpeed * m_lastDelta;
	m_playerY += dy * m_playerSpeed * m_lastDelta;

	// if (dx != 0 || dy != 0)
	// 	SDL_Log("m_playerX: %.0f / m_playerY: %.0f", m_playerX, m_playerY);

	// Clamp to screen bounds
	if (m_playerX < 0) m_playerX = 0;
	if (m_playerY < 0) m_playerY = 0;

	int w, h;
	SDL_GetWindowSize(m_window, &w, &h);

	if (m_playerX > w - 32) m_playerX = w - 32;
	if (m_playerY > h - 32) m_playerY = h - 32;

	return true;
}

void Game::Update(float deltaTime)
{
	// Update game logic here
	m_lastDelta = deltaTime;
}

void Game::Render()
{
	SDL_SetRenderDrawColor(m_renderer, 30, 30, 60, 255);
	SDL_RenderClear(m_renderer);

	// Draw tile map
	if (m_tileTexture) {
		SDL_FRect srcRect = {0, 0, (float)TILE_SIZE, (float)TILE_SIZE};
		for (int y = 0; y < MAP_HEIGHT; y++) {
			for (int x = 0; x < MAP_WIDTH; x++) {
				SDL_FRect dstRect = {
					(float)(x * TILE_SIZE),
					(float)(y * TILE_SIZE),
					(float)TILE_SIZE,
					(float)TILE_SIZE
				};
				SDL_RenderTexture(m_renderer, m_tileTexture,
					&srcRect, &dstRect);
			}
		}
	}

	// Draw Player
	if (m_playerTexture) {
		SDL_FRect srcRect = {0, 0, 32, 64};
		SDL_FRect dstRect = {m_playerX, m_playerY, 32.0f, 64.0f};
		SDL_RenderTexture(m_renderer, m_playerTexture,
			&srcRect, &dstRect);
	} else {
		// Fallback: colored rectangle
		SDL_SetRenderDrawColor(m_renderer, 0, 200, 100, 255);
		SDL_FRect playerRect = {m_playerX, m_playerY, 32.0f, 32.0f};
		SDL_RenderFillRect(m_renderer, &playerRect);
	}

	// Present the rendered frame
	SDL_RenderPresent(m_renderer);
}