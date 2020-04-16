#ifndef WINDOW_H
#define WINDOW_H

#include<string>
#include<SDL.h>
#include"Layer.h"
#include<windows.h>
#include<winuser.h>
#include<vector>

class Window
{
    public:
        Window(const int& SCREEN_HEIGHT, const int& SCREEN_WIDTH);
        virtual ~Window();
        SDL_Window* get_window(){return window;};
        SDL_Surface* get_surface(){return screenSurface;};

        void set_background(std::string path);
        void set_background(SDL_Surface* surface);

        void handle_event(SDL_Event &e);

        int get_width(){return width;}
        bool get_height(){return height;}
        bool hasMouseFocus(){return MouseFocus;}
        bool hasKeyboardFocus(){return KeyboardFocus;}
        bool isMinimized(){return Minimized;}

        void refresh();

    protected:

    private:
        SDL_Window* window=nullptr;
        SDL_Surface* screenSurface=nullptr;
        SDL_Surface* menu=nullptr;

        SDL_Texture* texture=nullptr;
        SDL_Renderer* renderer=nullptr;

        SDL_Rect rect, rect2, menu_rect;

        std::vector<Layer> layers;

        int width, height;
        bool MouseFocus=false;
        bool KeyboardFocus=false;
        bool Minimized=false;
};

#endif // WINDOW_H
