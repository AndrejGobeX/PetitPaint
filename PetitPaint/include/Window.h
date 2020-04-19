#ifndef WINDOW_H
#define WINDOW_H

#include<string>
#include<SDL.h>
#include"Layer.h"
#include<windows.h>
#include<winuser.h>
#include<vector>
#include"Formater.h"
#include<regex>

class Window
{
    public:
        Window(const int& SCREEN_HEIGHT, const int& SCREEN_WIDTH);
        virtual ~Window();
        SDL_Window* get_window(){return window;};
        SDL_Surface* get_surface(){return screenSurface;};

        void set_background(std::string path);
        void blit_to_background(SDL_Surface* surface);

        void handle_event(SDL_Event &e);

        int get_width(){return width;}
        bool get_height(){return height;}
        bool hasMouseFocus(){return MouseFocus;}
        bool hasKeyboardFocus(){return KeyboardFocus;}
        bool isMinimized(){return Minimized;}
        SDL_PixelFormat* get_format(){return format;}

        void refresh();
        void clear();
        void reload();

        void add(std::string path);
        void out(std::string path);

        bool n_quit=true;

        void swap_background(SDL_Surface* surface);

        void delete_layers();

    protected:

    private:
        SDL_Window* window=nullptr;
        SDL_Surface* screenSurface=nullptr;
        SDL_Surface* menu=nullptr;
        SDL_Surface* background=nullptr;

        SDL_PixelFormat* format=nullptr;

        SDL_Texture* texture=nullptr;
        SDL_Renderer* renderer=nullptr;

        SDL_Rect rect, rect2, menu_rect, surface_rect;

        std::vector<Layer*> layers;

        int width, height;
        bool MouseFocus=false;
        bool KeyboardFocus=false;
        bool Minimized=false;
};

#endif // WINDOW_H
