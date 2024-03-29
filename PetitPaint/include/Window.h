#ifndef WINDOW_H
#define WINDOW_H

#include<string>
#include<SDL.h>
#include"Layer.h"
#include<windows.h>
#include<winuser.h>
#include<vector>
#include<regex>
#include"Selection.h"
#include"Operation.h"
#include"CompositeOperation.h"

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
        void handle_command(std::string s);

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
        void delete_composites();

        void apply(Operation* operation);
        void recursive_add_composite(CompositeOperation* c_operation);

        std::string get_composite_label(){return (*composites.begin())->get_label();}

    protected:

    private:
        SDL_Window* window=nullptr;
        SDL_Surface* screenSurface=nullptr;
        SDL_Surface* menu=nullptr;
        SDL_Surface* background=nullptr;

        SDL_PixelFormat* format=nullptr;

        SDL_Texture* texture=nullptr;
        SDL_Renderer* renderer=nullptr;

        SDL_Rect rect, rect2, menu_rect, surface_rect, selection_rect;

        std::vector<Layer*> layers;
        std::vector<Selection> selections;
        std::list<CompositeOperation*> composites;

        bool preview_sel=true;

        int width, height;
        bool MouseFocus=false;
        bool KeyboardFocus=false;
        bool Minimized=false;

        bool saved=true;

        bool selection=false;
        int sx1, sx2;
};

#endif // WINDOW_H
