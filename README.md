# Hexit

Binary file viewer for all your precious files.

# Building

1) Build SDL
2) Build libpng
3) Build SDL_ttf
This will probably produce SDL_ttf.obj that has to be manually linked with libfreetype-6.lib. Run this line in "Developer PowerShell for VS 2019"

`$ lib.exe .\SDL_ttf_build\lib\SDL_ttf.obj .\SDL_ttf_build\lib\libfreetype-6.lib /OUT:.\SDL_ttf_build\lib\MySDL_ttf.lib`

MySDL_ttf.lib would be your static SDL_ttf library.