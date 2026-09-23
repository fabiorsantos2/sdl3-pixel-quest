// src/Game.h (PixelQuest v0.1)
#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

class Game {
public:
	Game();
	~Game();

	// Do not allow instance copy or move
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;

	bool Initialize();
	void Run();

private:
	void Shutdown();
	bool HandleEvents();
	void Update(float deltaTime);
	void Render();

	SDL_Window* m_window = nullptr;
	SDL_Renderer* m_renderer = nullptr;
	bool m_running = false;

	// PixelQuest state
	int m_screenWidth = 800;
	int m_screenHeight = 600;
	float m_lastDelta = 0.0f;

	// Textures
	SDL_Texture* m_playerTexture = nullptr;
	SDL_Texture* m_tileTexture = nullptr;

	// Tile map (simple 2D grid)
	static constexpr int TILE_SIZE = 32;
	static constexpr int MAP_WIDTH = 25;	// 800 / 32
	static constexpr int MAP_HEIGHT = 19;	// ~600 / 32

	// Player state
	float m_playerX = 100.0f;
	float m_playerY = 400.0f;
	float m_playerSpeed = 1000.0f; // pixels per second
};