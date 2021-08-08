#include "SDLH_util.h"

#include <stdexcept>
#include <tuple>
#include <cmath>

using namespace std;

/* OVERENGINEERING:
// Type for function pointer to SDL_GetError and TTF_GetError
typedef const char* (*GetError_t)();

template<GetError_t GetError>
void unwrap(bool is_error, const char* error_message)
{
	if (is_error) {
		throw runtime_error(string(error_message) + GetError());
	}
}
*/

void SDLH_Unwrap(bool is_error, const char* error_message)
{
	if (is_error) {
		throw runtime_error(string(error_message) + SDL_GetError());
	}
}

void SDLH_UnwrapPointer(void* pointer, const char* error_message)
{
	SDLH_Unwrap(pointer == nullptr, error_message);
}

bool SDLH_RectContainsPointEh(SDL_Rect rect, int x, int y)
{
	return (rect.x <= x) && (x < rect.x + rect.w) &&
		(rect.y <= y) && (y < rect.y + rect.h);
}

std::tuple<int, int> get_glyph_sizes(TTF_Font* font)
{
	SDL_Surface* text_surface = TTF_RenderText_Blended(font, "0", { 0,0,0,0xff });
	SDLH_UnwrapPointer(text_surface, "could not render text to a surface: ");
	auto glyph_sizes = make_tuple(text_surface->w, text_surface->h);
	SDL_FreeSurface(text_surface);
	return glyph_sizes;
}
