#include "Window.h"
#include<SDL.h>
#include<iostream>

const int INIT_ERROR=1, WINDOW_ERROR=2;

Window::Window(const int& SCREEN_HEIGHT, const int& SCREEN_WIDTH)
{
    if(SDL_Init(SDL_INIT_VIDEO)<0)
    {
        std::cout<<"SDL could not initialize. Error: "<<SDL_GetError()<<"\n";
        throw INIT_ERROR;
    }
    window = SDL_CreateWindow( "PetitPaint", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if(window==nullptr)
    {
        std::cout<<"SDL window not created. Error: "<<SDL_GetError()<<"\n";
        throw WINDOW_ERROR;
    }

    screenSurface=SDL_GetWindowSurface(window);
    SDL_FillRect(screenSurface, nullptr, SDL_MapRGB(screenSurface->format, 0x0, 0x0, 0x0));
    SDL_UpdateWindowSurface(window);
}

Window::~Window()
{
    SDL_DestroyWindow(window);
    if(background!=nullptr)
    {
        SDL_FreeSurface(background);
        background=nullptr;
    }
    SDL_Quit();
}

void Window::set_background(std::string path)
{
    background=SDL_LoadBMP(path.c_str());
    if(background==nullptr)
    {
        std::cout<<"Could not load image. Error: "<<SDL_GetError()<<"\n";
        return;
    }
    SDL_BlitSurface(background, nullptr, screenSurface, nullptr);
    SDL_UpdateWindowSurface(window);
}

void Window::set_background(SDL_Surface* surface)
{
    SDL_BlitSurface(surface, nullptr, screenSurface, nullptr);
    SDL_UpdateWindowSurface(window);
}
