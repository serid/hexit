#include "Sprite.h"

#include <stdexcept>

#include "read_png.h"

#include "SDLH_util.h"

using namespace std;

Sprite::Sprite(SDL_Renderer* renderer, TextureP&& backing_texture, int width, int height) : renderer(renderer), backing_texture(forward<TextureP>(backing_texture)), width(width), height(height)
{
}

Sprite::Sprite() : renderer(nullptr), backing_texture(nullptr), width(-1), height(-1)
{
}

Sprite::Sprite(SDL_Renderer* renderer, TTF_Font* font, const string& text, SDL_Color color) : renderer(renderer), backing_texture(nullptr)
{
	SDL_Surface* text_surface = TTF_RenderText_Blended(font, text.c_str(), color);
	SDLH_UnwrapPointer(text_surface, "could not render text to a surface: ");

	SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
	SDLH_UnwrapPointer(text_texture, "could not copy surface to texture: ");

	backing_texture.reset(text_texture);
	width = text_surface->w;
	height = text_surface->h;

	SDL_FreeSurface(text_surface);
}

Sprite::Sprite(SDL_Renderer* renderer, const std::string& filename) : renderer(renderer), width(-1), height(-1)
{
	u8* pixels = read_png(filename.c_str(), &width, &height);

	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STATIC, width, height);
	SDLH_UnwrapPointer(texture, "texture creation error");
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

	SDLH_Unwrap(SDL_UpdateTexture(texture, nullptr, pixels, width * 4) != 0, "texture update error");
	
	backing_texture.reset(texture);

	free(pixels);
}

#pragma warning (suppress : 26439)
Sprite::Sprite(Sprite&& other) : renderer(other.renderer), backing_texture(forward<TextureP>(other.backing_texture)), width(other.width), height(other.height)
{
	other.renderer = nullptr;
}

Sprite& Sprite::operator=(Sprite&& other) noexcept
{
	if (this == &other)
		return *this;

	renderer = other.renderer;
	backing_texture = move(other.backing_texture);
	width = other.width;
	height = other.height;

	other.renderer = nullptr;
	other.width = -1;
	other.height = -1;

	return *this;
}

Sprite::~Sprite()
{
	// Free `backing_texture` if present
	//if (backing_texture != nullptr)
	//	SDL_DestroyTexture(backing_texture);
	// TextureP destructor will do this for us
}

bool Sprite::is_empty() const
{
	if (renderer == nullptr && backing_texture == nullptr && width == -1 && height == -1)
		return true;
	else if (renderer != nullptr && backing_texture != nullptr && width != -1 && height != -1)
		return false;
	else
		throw runtime_error("Invalid Sprite state");
}

void Sprite::draw(int x, int y) const
{
	SDL_Rect rect{ x, y, width, height };
	SDL_RenderCopy(renderer, backing_texture.get(), NULL, &rect);
}

int Sprite::get_width() const
{
	return width;
}

int Sprite::get_height() const
{
	return height;
}
