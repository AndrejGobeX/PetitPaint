#include<iostream>
#include<SDL.h>
#include"Window.h"
#include<fstream>
#include<string>
#include<regex>
#include"Formater.h"
#include"Image.h"

using namespace std;

const int SCREEN_HEIGHT=417;
const int SCREEN_WIDTH=417;

int main(int argc, char* argv[])
{
    Window win(SCREEN_HEIGHT, SCREEN_WIDTH);
    win.set_background("samples\\PetitPaint.bmp");
    win.refresh();

    bool n_quit=true;
    SDL_Event e;

    while(n_quit)
    {
        while(SDL_PollEvent(&e)!=0)
        {
            if(e.type==SDL_QUIT)n_quit=false;
            else
            {
                win.handle_event(e);
            }
        }
    }

    return 0;
}
