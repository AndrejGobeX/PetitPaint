#ifndef IMAGE_H
#define IMAGE_H

class SDL_Surface;

class Image
{
    public:
        Image(SDL_Surface* _surface, unsigned char* _pixels, unsigned _width,
              unsigned _height, unsigned _depth);
        Image(SDL_Surface* _surface, unsigned _width,
              unsigned _height, unsigned _depth);
        virtual ~Image();

        SDL_Surface* get_surface(){return surface;}
        unsigned get_width(){return width;}
        unsigned get_height(){return height;}
        unsigned get_depth(){return depth;}

    protected:

    private:
        SDL_Surface* surface=nullptr;
        unsigned width, height, depth;
};

#endif // IMAGE_H
