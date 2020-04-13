#include "Image.h"
#include<SDL.h>

Image::Image(SDL_Surface* _surface, unsigned char* _pixels, unsigned _width,
    unsigned _height, unsigned _depth):surface(_surface), pixels(_pixels),
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
    if(pixels)
    {
        delete [] pixels;
        pixels=nullptr;
    }
}
