#pragma once

#include <vector>
#include <unordered_map>

#include "ints.h"
#include "TextAndColor.h"
#include "Sprite.h"

class App
{
	WindowP window;
	RendererP renderer;
	TextureP target;
	FontP font;

	// Bytes to be displayed
	std::vector<u8> bytes;

	// Scroll distance
	int scroll;
	// Scroll velocity to simulate inertia
	int scroll_velocity;

	// Sizes of a rendered character
	int glyph_width;
	int glyph_height;

	// GUI layout
	SDL_Rect top_bar;
	SDL_Rect top_gutter;
	SDL_Rect left_bar;
	SDL_Rect main_table;
	SDL_Rect right_bar;
	SDL_Rect open_button;

	// This cache allows to call `TTF_RenderText_Blended` once per text string and then
	// reuse already allocated texture every time text needs to be drawn
	std::unordered_map<TextAndColor, Sprite> text_cache;


	Sprite& get_text_sprite_lazy(const std::string& text, SDL_Color color);
	void draw_text(const std::string& text, int x, int y, SDL_Color color);

	void handle_input(bool& continue_eh);

	// Forbid copying and moving App
	App(const App& other) = delete;
	App(App&& other) = delete;

public:
	App();
	~App();

	void run();
};

