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
    window = SDL_CreateWindow( "PetitPaint", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH+100, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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
    rect.x=100;
    rect.y=0;
    rect2=rect;
    menu_rect.h=height;
    menu_rect.w=100;
    menu_rect.x=0;
    menu_rect.y=0;
    menu=SDL_LoadBMP("menu\\menu.bmp");
    //SDL_BlitSurface(menu, nullptr, screenSurface, nullptr);

    texture=SDL_CreateTextureFromSurface(renderer, menu);
    SDL_RenderCopy(renderer, texture, &menu_rect, &menu_rect);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(texture);
    texture=nullptr;

    format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
}

Window::~Window()
{
    SDL_DestroyWindow(window);
    if(renderer!=nullptr)
    {
        SDL_DestroyRenderer(renderer);
        renderer=nullptr;
    }
    if(menu!=nullptr)
    {
        SDL_FreeSurface(menu);
        menu=nullptr;
    }
    if(background!=nullptr)
    {
        SDL_FreeSurface(background);
        background=nullptr;
    }
    if(format!=nullptr)
    {
        SDL_FreeFormat(format);
        format=nullptr;
    }
    delete_layers();
    SDL_Quit();
}

void Window::delete_layers()
{
    std::for_each(layers.begin(), layers.end(), [](Layer* l){delete l;});
    layers.clear();
}

void Window::clear()
{
    SDL_FillRect(background, nullptr, SDL_MapRGBA(format, 0x0, 0x0, 0x0, 0xFF));
}

void Window::swap_background(SDL_Surface* surface)
{
    if(background==nullptr)
    {
        SDL_FreeSurface(background);
    }
    background=surface;
}

void Window::set_background(std::string path)
{
    SDL_Surface* srf=SDL_LoadBMP(path.c_str());
    if(srf==nullptr)
    {
        std::cout<<"Could not load image. Error: "<<SDL_GetError()<<"\n";
        return;
    }
    background=SDL_ConvertSurface(srf, format, 0);
    SDL_FreeSurface(srf);
}

void Window::blit_to_background(SDL_Surface* surface)
{
    SDL_BlitSurface(surface, nullptr, background, nullptr);
}

void Window::refresh()
{
    texture=SDL_CreateTextureFromSurface(renderer, screenSurface);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);

    SDL_DestroyTexture(texture);
    texture=nullptr;

    texture=SDL_CreateTextureFromSurface(renderer, menu);
    SDL_RenderCopy(renderer, texture, &menu_rect, &menu_rect);

    SDL_DestroyTexture(texture);
    texture=nullptr;

    texture=SDL_CreateTextureFromSurface(renderer, background);
    SDL_RenderCopy(renderer, texture, nullptr, &rect);

    SDL_DestroyTexture(texture);
    texture=nullptr;

    SDL_RenderDrawLine(renderer, rect.w+1+100, 0, rect.w+1+100, rect.h+1);
    SDL_RenderDrawLine(renderer, 100, rect.h+1, rect.w+1+100, rect.h+1);
    SDL_RenderDrawLine(renderer, 99, 0, 99, rect.h+1);

    SDL_RenderPresent(renderer);
}

void Window::reload()
{
    clear();
    for(Layer* l:layers)
    {
        if(l->isVisible())SDL_BlitSurface(l->get_image()->get_surface(), nullptr, background, nullptr);
    }
    refresh();
}

void Window::out(std::string path)
{
    std::regex bmp("(.*)\u002Ebmp");
    std::regex pam("(.*)\u002Epam");

    std::smatch match;

    if(regex_match(path, match, bmp))
    {
        Formater::export_BMP(background, path);
    }
    else if(regex_match(path, match, pam))
    {
        Formater::export_PAM(background, path);
    }
    else
    {
        std::cout<<"File type not supported (yet).\n";
        return;
    }
}

void Window::add(std::string path)
{
    SDL_Surface* surface=nullptr;
    Image* img;

    std::regex bmp("(.*)\u002Ebmp");
    std::regex pam("(.*)\u002Epam");

    std::smatch match;

    if(regex_match(path, match, bmp))
    {
        surface=SDL_LoadBMP(path.c_str());
        img=new Image(SDL_ConvertSurface(surface, format, 0), surface->w, surface->h, 4);
        SDL_FreeSurface(surface);
    }
    else if(regex_match(path, match, pam))
    {
        Image* temp;
        temp=Formater::read_PAM(path);
        if(temp!=nullptr){
        img=new Image(SDL_ConvertSurface(temp->get_surface(), format, 0), temp->get_width(), temp->get_height(), 4);
        delete temp;
        }
    }
    else
    {
        std::cout<<"File type not supported (yet).\n";
        return;
    }
    if(img->get_surface()==nullptr)
    {
        std::cout<<"Error processing image.\n";
        delete img;
        return;
    }
    layers.push_back((new Layer(img, true)));

    rect.h=std::max((unsigned)rect.h, img->get_height());//Resizing background
    rect.w=std::max((unsigned)rect.w, img->get_width()); //if necessary
    rect2=rect;

    //Swap new dimensions
    swap_background(SDL_CreateRGBSurfaceWithFormat(0, rect.w, rect.h, 32, format->format));
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
    else if ( e.type == SDL_MOUSEBUTTONUP)
    {
        int x, y;
        SDL_GetMouseState(&x, &y);

        //Menu
        if(x>0 && x<100)
        {
            //New
            if(y>0 && y<30)
            {
                rect2.w=0;
                rect2.h=0;
                rect=rect2;
                delete_layers();
                clear();
                refresh();
            }
            //Import
            if(y>120 && y<150)
            {
                std::string s;
                std::cout<<"Path to file: ";
                std::cin>>s;
                add(s);
                reload();
            }
            //Export
            if(y>150 && y<180)
            {
                std::string s;
                std::cout<<"Path to file: ";
                std::cin>>s;
                out(s);
            }
            //Quit
            if(y>210 && y<240)
            {
                n_quit=false;
            }
        }
    }
}
