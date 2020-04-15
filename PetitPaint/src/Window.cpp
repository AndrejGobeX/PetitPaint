#include "Window.h"
#include<iostream>

const int INIT_ERROR=1, WINDOW_ERROR=2, RENDER_ERROR=3;

Window::Window(const int& SCREEN_HEIGHT, const int& SCREEN_WIDTH): width(SCREEN_WIDTH), height(SCREEN_HEIGHT)
{
    if(SDL_Init(SDL_INIT_VIDEO)<0)
    {
        std::cout<<"SDL could not initialize. Error: "<<SDL_GetError()<<"\n";
        throw INIT_ERROR;
    }
    if(!(IMG_Init(IMG_INIT_PNG)&IMG_INIT_PNG))
    {
        std::cout<<"IMG could not initialize. Error: "<<IMG_GetError()<<"\n";
        throw INIT_ERROR;
    }
    window = SDL_CreateWindow( "PetitPaint", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT+20, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if(window==nullptr)
    {
        std::cout<<"SDL window not created. Error: "<<SDL_GetError()<<"\n";
        throw WINDOW_ERROR;
    }

    screenSurface=SDL_GetWindowSurface(window);
    SDL_FillRect(screenSurface, nullptr, SDL_MapRGBA(screenSurface->format, 0x0, 0x0, 0x0, 0xFF));
    SDL_UpdateWindowSurface(window);
    renderer=SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer==nullptr)
    {
        std::cout<<"SDL renderer not created. Error: "<<SDL_GetError()<<"\n";
        throw RENDER_ERROR;
    }
    SDL_SetRenderDrawColor(renderer, 0x00, 0xAA, 0xAA, 0xFF);

    rect.h=height;
    rect.w=width;
    rect.x=0;
    rect.y=0;

    SDL_SysWMinfo sysInfo;
    SDL_VERSION(&sysInfo.version);
    SDL_GetWindowWMInfo(window, &sysInfo);
    HWND hwnd=sysInfo.info.win.window;
    menubar=CreateMenu();
    HMENU file=CreateMenu();
    HMENU new_=CreateMenu();
    HMENU exit=CreateMenu();
    HMENU edit=CreateMenu();
    HMENU export_=CreateMenu();
    HMENU save=CreateMenu();
    HMENU import=CreateMenu();
    AppendMenu(menubar, MF_POPUP, (UINT_PTR)file, "File");
    AppendMenu(menubar, MF_POPUP, (UINT_PTR)edit, "Edit");
    AppendMenu(file, MF_STRING, (UINT_PTR)new_, "New");
    AppendMenu(file, MF_STRING, (UINT_PTR)save, "Save");
    AppendMenu(file, MF_STRING, (UINT_PTR)import, "Import");
    AppendMenu(file, MF_STRING, (UINT_PTR)export_, "Export");
    AppendMenu(file, MF_STRING, (UINT_PTR)exit, "Exit");
    SetMenu(hwnd, menubar);
}

Window::~Window()
{
    SDL_DestroyWindow(window);
    if(background!=nullptr)
    {
        SDL_FreeSurface(background);
        background=nullptr;
    }
    if(renderer!=nullptr)
    {
        SDL_DestroyRenderer(renderer);
        renderer=nullptr;
    }
    DestroyMenu(menubar);
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

    texture=SDL_CreateTextureFromSurface(renderer, background);
    SDL_RenderCopy(renderer, texture, &rect, &rect);

    SDL_DestroyTexture(texture);
    texture=nullptr;

    SDL_RenderDrawLine(renderer, rect.w+1, 0, rect.w+1, rect.h+1);
    SDL_RenderDrawLine(renderer, 0, rect.h+1, rect.w+1, rect.h+1);

    SDL_RenderPresent(renderer);
}

void Window::set_background(SDL_Surface* surface)
{
    SDL_Rect tr;
    tr.x=tr.y=tr.h=tr.w=200;
    SDL_BlitSurface(surface, nullptr, background, &tr);

    texture=SDL_CreateTextureFromSurface(renderer, background);
    SDL_RenderCopy(renderer, texture, &rect, &rect);

    SDL_DestroyTexture(texture);
    texture=nullptr;

    SDL_RenderDrawLine(renderer, rect.w+1, 0, rect.w+1, rect.h+1);
    SDL_RenderDrawLine(renderer, 0, rect.h+1, rect.w+1, rect.h+1);

    SDL_RenderPresent(renderer);
}

void Window::refresh()
{
    texture=SDL_CreateTextureFromSurface(renderer, background);
    SDL_RenderCopy(renderer, texture, &rect, &rect);

    SDL_DestroyTexture(texture);
    texture=nullptr;

    SDL_RenderDrawLine(renderer, rect.w+1, 0, rect.w+1, rect.h+1);
    SDL_RenderDrawLine(renderer, 0, rect.h+1, rect.w+1, rect.h+1);

    SDL_RenderPresent(renderer);
}

void Window::handle_event(SDL_Event &e)
{
    if( e.type == SDL_WINDOWEVENT )
    {
        switch( e.window.event )
        {
        //Get new dimensions and repaint on window size change
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            width = e.window.data1;
            height = e.window.data2;
            refresh();
            break;

        //Repaint on exposure
        case SDL_WINDOWEVENT_EXPOSED:
            refresh();
            break;

        //Mouse entered window
        case SDL_WINDOWEVENT_ENTER:
            MouseFocus = true;
            break;

        //Mouse left window
        case SDL_WINDOWEVENT_LEAVE:
            MouseFocus = false;
            break;

        //Window has keyboard focus
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            KeyboardFocus = true;
            break;

        //Window lost keyboard focus
        case SDL_WINDOWEVENT_FOCUS_LOST:
            KeyboardFocus = false;
            break;

        //Window minimized
        case SDL_WINDOWEVENT_MINIMIZED:
            Minimized = true;
            break;

        //Window maximized
        case SDL_WINDOWEVENT_MAXIMIZED:
            Minimized = false;
            break;

        //Window restored
        case SDL_WINDOWEVENT_RESTORED:
            Minimized = false;
            break;
        }
    }
}
