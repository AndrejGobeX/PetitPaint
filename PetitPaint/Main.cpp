#include<iostream>
#include<SDL.h>

using namespace std;

const int SCREEN_HEIGHT=500;
const int SCREEN_WIDTH=500;

int main(int argc, char* argv[])
{
    SDL_Window* window=nullptr;
    SDL_Surface* screenSurface=nullptr;

    if(SDL_Init(SDL_INIT_VIDEO)<0)
    {
        cout<<"SDL could not initialize. Error: "<<SDL_GetError()<<"\n";
        return 0;
    }
    window = SDL_CreateWindow( "PetitPaint", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if(window==nullptr)
    {
        cout<<"SDL window not created. Error: "<<SDL_GetError()<<"\n";
        return 0;
    }

    screenSurface=SDL_GetWindowSurface(window);
    SDL_FillRect(screenSurface, nullptr, SDL_MapRGB(screenSurface->format, 0x0, 0x0, 0x0));
    SDL_UpdateWindowSurface(window);
    SDL_Delay(2000);
    SDL_DestroyWindow(window);
    SDL_Quit();


    return 0;
}
