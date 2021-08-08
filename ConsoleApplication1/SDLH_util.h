#pragma once

#include <memory>

#pragma warning (disable : 26812)
#include <SDL.h>
#include <SDL_ttf.h>

void SDLH_Unwrap(bool is_error, const char* error_message);
void SDLH_UnwrapPointer(void* pointer, const char* error_message);
bool SDLH_RectContainsPointEh(SDL_Rect rect, int x, int y);

std::tuple<int, int> get_glyph_sizes(TTF_Font* font);

// Type of pointer to function like SDL_DestroyWindow
template<typename T>
using Destroyer_t = void (*)(T* pointer);

// Deleter that deletes a pointer to type `T` using function `destroyer`
template<typename T, Destroyer_t<T> destroyer>
class Deleter {
public:
	void operator()(T* pointer) {
		destroyer(pointer);
	}
};

// unique_ptr that deletes a pointer using SDL_DestroyWindow
typedef std::unique_ptr<SDL_Window, Deleter<SDL_Window, SDL_DestroyWindow>> WindowP;
typedef std::unique_ptr<SDL_Renderer, Deleter<SDL_Renderer, SDL_DestroyRenderer>> RendererP;
typedef std::unique_ptr<SDL_Texture, Deleter<SDL_Texture, SDL_DestroyTexture>> TextureP;
typedef std::unique_ptr<SDL_Surface, Deleter<SDL_Surface, SDL_FreeSurface>> SurfaceP;

typedef std::unique_ptr<TTF_Font, Deleter<TTF_Font, TTF_CloseFont>> FontP;