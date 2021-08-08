#pragma once

#include <string>

#include "SDLH_util.h"

class Sprite
{
	// Renderer associated with `backing_texture`
	SDL_Renderer* renderer;
	TextureP backing_texture;
	int width;
	int height;

	Sprite(SDL_Renderer* renderer, TextureP&& texture, int width, int height);

	// Forbid copying Sprite
	Sprite(const Sprite& other) = delete;
	Sprite operator=(const Sprite& other) = delete;

public:
	// Create an empty sprite
	Sprite();

	// Render text to a texture and return it
	Sprite(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_Color color);

	// Load texture from a file
	Sprite(SDL_Renderer* renderer, const std::string& filename);

	// Moves ownership of `backing_texture` to the new object
	Sprite(Sprite&& other);
	Sprite& operator=(Sprite&& other) noexcept;
	~Sprite();

	bool is_empty() const;
	void draw(int x, int y) const;
	int get_width() const;
	int get_height() const;
};

