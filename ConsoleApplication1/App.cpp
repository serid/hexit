#include "App.h"

#include <stdexcept>
#include <utility>
#include <string>
#include <iostream>
#include <fstream>

#include "ints.h"
#include "SDLH_util.h"
#include "Util.h"
#include "Sprite.h"

using namespace std;

static_assert(SDL_GetError == TTF_GetError, "This application assumes that (SDL_GetError == TTF_GetError)");

const SDL_Color black{ 0, 0, 0, 0xff };
const SDL_Color white{ 0xff, 0xff, 0xff, 0xff, };
const SDL_Color primary_gray{ 0x30, 0x30, 0x30, 0xff };
const SDL_Color background_gray{ 0x20, 0x20, 0x20, 0xff };
const SDL_Color deep_blue{ 0x30, 0x30, 0x60, 0xff };
const SDL_Color light_blue{ 0x00, 0xf0, 0xc0, 0xff };

const int font_size = 16;

// Returns a reference to a sprite for text if it was rendered before, or renders it on demand
Sprite& App::get_text_sprite_lazy(const string& text, SDL_Color color)
{
	// Lookup sprite in cache. If there is no sprite for text,color pair, then `sprite` will be an "empty" default-constructed Sprite() object
	Sprite& sprite = text_cache[TextAndColor{ text, color }];

	// If there is no sprite associated with text, render it
	if (sprite.is_empty())
	{
		sprite = Sprite(renderer.get(), font.get(), text, color);
		/*new (&sprite) Sprite(renderer.get(), font.get(), text, color);*/
	}

	return sprite;
}

void App::draw_text(const string& text, int x, int y, SDL_Color color)
{
	get_text_sprite_lazy(text, color).draw(x, y);
}

App::App()
{
	SDLH_Unwrap(SDL_Init(SDL_INIT_VIDEO) < 0, "could not initialize SDL: ");

	SDLH_Unwrap(TTF_Init() == -1, "could not initialize SDL_ttf: ");

	font = FontP(TTF_OpenFont("./rsc/SourceCodePro-Regular.otf", font_size));
	SDLH_UnwrapPointer(font.get(), "could not open font: ");

	std::tuple<int, int> glyph_sizes = get_glyph_sizes(font.get());
	glyph_width = get<0>(glyph_sizes);
	glyph_height = get<1>(glyph_sizes);

	// Compute GUI layout
	top_bar = { 0, 0, -1, 60 };
	top_gutter = { 0, top_bar.y + top_bar.h, -1, glyph_height };
	left_bar = { 0, top_gutter.y + top_gutter.h, glyph_width * 11, 234 };
	main_table = { left_bar.x + left_bar.w, left_bar.y, glyph_width * 52, 234 };
	right_bar = { main_table.x + main_table.w, left_bar.y, glyph_width * 18, 234 };

	top_bar.w = left_bar.w + main_table.w + right_bar.w;
	top_gutter.w = top_bar.w;

	open_button = { top_bar.x + 5, top_bar.y + 5, 50, 50 };

	int window_w = top_bar.w;
	int window_h = 800;

	// Create window taking borders into account
	window = WindowP(SDL_CreateWindow("Hexit", 100, 100, window_w, window_h, 0 /*SDL_WINDOW_RESIZABLE*/));
	SDLH_UnwrapPointer(window.get(), "could not create window: ");

	renderer = RendererP(SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE));
	SDLH_UnwrapPointer(renderer.get(), "could not create renderer: ");

	target = TextureP(SDL_CreateTexture(renderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, window_w, window_h));
	SDLH_UnwrapPointer(target.get(), "could not create target texture: ");
}

void App::handle_input(bool& continue_eh) {
	SDL_Event event;

	while (SDL_PollEvent(&event) == 1)
	{
		if (event.type == SDL_QUIT) {
			continue_eh = false;
		}
		else if (event.type == SDL_MOUSEBUTTONUP &&
			event.button.button == SDL_BUTTON_LEFT &&
			SDLH_RectContainsPointEh(open_button, event.button.x, event.button.y)) {
			// When "open file" button is pressed
			wstring filename = move(open_file_dialog());

			// The dialog was closed
			if (filename.empty())
				continue;

			bytes.clear();

			ifstream file(filename, ios::in | ios::binary);

			if (!file.is_open())
				throw runtime_error("error opening file");

			while (true)
			{
				u8 byte = 0xcc;
				file.read(reinterpret_cast<char*>(&byte), 1);

				if (file.eof())
					break;
				if (file.fail())
					throw runtime_error("error reading file");

				bytes.push_back(byte);
			}
		}
		else if (event.type == SDL_MOUSEWHEEL) {
			scroll_velocity += event.wheel.y * 10;
		}
	}
}

void App::run()
{
	Sprite open_icon(renderer.get(), "rsc/open.png");

	// GUI main table sizes
	const int columns = 16;
	const int rows = 30;

	// Bytes to be displayed
	//vector<u8> bytes(rows * columns - 10);
	//for (int i = 0; i < bytes.size(); i++)
	//{
	//	bytes[i] = i;
	//}

	string str_colon(1, ':');
	string str1(1, '\xfd');
	string str2(2, '\xfd');

	while (true) {
		bool continue_game_loop = true;

		handle_input(continue_game_loop);
		if (!continue_game_loop)
			break;

		// Scroll physics
		if (scroll > 0) {
			scroll_velocity = -(scroll / 10) - 1;
		}
		else {
			if (scroll_velocity < 0)
			{
				scroll_velocity += 3;
				if (scroll_velocity > 0)
					scroll_velocity = 0;
			}
			else {
				scroll_velocity -= 3;
				if (scroll_velocity < 0)
					scroll_velocity = 0;
			}
		}

		scroll += scroll_velocity;

		// Switch render target from window to the texture
		SDL_SetRenderTarget(renderer.get(), target.get());

		// Clear target
		SDL_SetRenderDrawColor(renderer.get(), background_gray.r, background_gray.g, background_gray.b, background_gray.a);
		SDL_RenderClear(renderer.get());

		// Draw bytes
		int cursor_x = 0;
		int cursor_y = 0;

		bool reached_eof = false;
		for (int i = 0; !reached_eof; i++)
		{
			// Draw left bar
			cursor_x++;
			// Iterate over pairs of characters
			for (int j = 0; j < 4; j++) {
				int shift = (3 - j) * 8;
				byte_to_string(str2, (i & (0xff << shift)) >> shift);
				draw_text(str2, left_bar.x + cursor_x * glyph_width, scroll + left_bar.y + cursor_y * glyph_height, white);
				cursor_x += 2;
			}
			draw_text(str_colon, left_bar.x + cursor_x * glyph_width, scroll + left_bar.y + cursor_y * glyph_height, white);
			cursor_x = 0;

			// Draw main table
			cursor_x++;
			for (int j = 0; j < columns; j++) {
				//string str = to_string(int(bytes[i * 32 + j]));
				//printf("%s", str.c_str());

				int index = i * columns + j;
				if (index >= bytes.size())
				{
					reached_eof = true;
					break;
				}

				byte_to_string(str2, bytes[index]);
				draw_text(str2, main_table.x + cursor_x * glyph_width, scroll + main_table.y + cursor_y * glyph_height, white);

				cursor_x += 3;
				if (j % 4 == 3)
					cursor_x++;
			}
			cursor_x = 0;

			// Draw right bar
			cursor_x++;
			// Iterate over characters
			for (int j = 0; j < columns; j++) {
				int index = i * columns + j;
				if (index >= bytes.size())
				{
					reached_eof = true;
					break;
				}

				str1[0] = to_printable_char(bytes[index]);
				draw_text(str1, right_bar.x + cursor_x * glyph_width, scroll + right_bar.y + cursor_y * glyph_height, white);
				cursor_x += 1;
			}

			cursor_y++;
			cursor_x = 0;
		}

		SDL_SetRenderDrawColor(renderer.get(), primary_gray.r, primary_gray.g, primary_gray.b, primary_gray.a);
		//SDL_SetRenderDrawColor(renderer.get(), deep_blue.r, deep_blue.g, deep_blue.b, deep_blue.a);
		SDL_RenderFillRect(renderer.get(), &top_bar);
		SDL_SetRenderDrawColor(renderer.get(), light_blue.r, light_blue.g, light_blue.b, light_blue.a);
		SDL_RenderFillRect(renderer.get(), &top_gutter);

		// Draw icon "open file"
		open_icon.draw(open_button.x, open_button.y);

		// Draw top gutter
		draw_text("00 01 02 03  04 05 06 07  08 09 0a 0b  0c 0d 0e 0f", main_table.x + glyph_width, top_gutter.y, black);

		// Switch render target from texture to window
		SDL_SetRenderTarget(renderer.get(), NULL);
		// Copy image from texture to window and present it
		SDL_RenderCopy(renderer.get(), target.get(), NULL, NULL);
		SDL_RenderPresent(renderer.get());

		//SDL_Delay(16);
	}
}

App::~App()
{
	//SDL_DestroyTexture(target);
	//target = nullptr;

	//SDL_DestroyRenderer(renderer);
	//renderer = nullptr;

	//SDL_DestroyWindow(window);
	//window = nullptr;

	//TTF_CloseFont(font);
	//font = nullptr;

	// Release resources
	text_cache.clear();
	target.reset();
	renderer.reset();
	window.reset();
	font.reset();

	SDL_Quit();
	TTF_Quit();
}
