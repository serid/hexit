#pragma once

#include <string>
#include <unordered_map>

#pragma warning (disable : 26812)
#include <SDL.h>

struct TextAndColor {
	std::string text;
	SDL_Color color;

	bool operator==(const TextAndColor& other) const {
		return text == other.text &&
			color.r == other.color.r &&
			color.g == other.color.g &&
			color.b == other.color.b &&
			color.a == other.color.a;
	}
};

template<>
struct std::hash<TextAndColor> {
	size_t operator()(const TextAndColor& tpl) const {
		auto byte_hasher = std::hash<uint8_t>();
		return std::hash<std::string>()(tpl.text) + byte_hasher(tpl.color.r) + byte_hasher(tpl.color.g) + byte_hasher(tpl.color.b) + byte_hasher(tpl.color.a);
	}
};
