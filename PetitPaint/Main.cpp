#include<iostream>
#include<SDL.h>
#include"Window.h"

using namespace std;

const int SCREEN_HEIGHT=500;
const int SCREEN_WIDTH=500;

int main(int argc, char* argv[])
{
    Window win(SCREEN_HEIGHT, SCREEN_WIDTH);
    win.set_background("PetitPaint.bmp");

    bool n_quit=true;
    SDL_Event e;

    while(n_quit)
    {
        while(SDL_PollEvent(&e)!=0)
        {
            if(e.type==SDL_QUIT)n_quit=false;
        }
    }

    return 0;
}
