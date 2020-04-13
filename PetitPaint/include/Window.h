#ifndef WINDOW_H
#define WINDOW_H

#include<string>

class SDL_Window;
class SDL_Surface;

class Window
{
    public:
        Window(const int& SCREEN_HEIGHT, const int& SCREEN_WIDTH);
        virtual ~Window();
        SDL_Window* get_window(){return window;};
        SDL_Surface* get_surface(){return screenSurface;};

        void set_background(std::string path);
        void set_background(SDL_Surface* surface);

    protected:

    private:
        SDL_Window* window=nullptr;
        SDL_Surface* screenSurface=nullptr;
        SDL_Surface* background=nullptr;
};

#endif // WINDOW_H
