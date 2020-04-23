#include "Image.h"
#include<SDL.h>

Image::Image(SDL_Surface* _surface, unsigned _width,
    unsigned _height, unsigned _depth):surface(_surface),
    width(_width), height(_height), depth(_depth)
{
}

Image::~Image()
{
    if(surface)
    {
        SDL_FreeSurface(surface);
        surface=nullptr;
    }
}

void Image::refresh()
{
    width=surface->w;
    height=surface->h;
}
