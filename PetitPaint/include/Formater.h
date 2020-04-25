#ifndef FORMATER_H
#define FORMATER_H

#include<string>
#include"Pixel.h"
#include<vector>
#include"Selection.h"

class Image;
class SDL_Surface;
class CompositeOperation;

class Formater
{
    public:
        static Image* read_PAM(std::string path);
        static void export_BMP(SDL_Surface* surface, std::string path);
        static void export_PAM(SDL_Surface* surface, std::string path);
        static std::vector<Pixel> surface_to_pixels(SDL_Surface* surface);
        static SDL_Surface* pixels_to_surface(std::vector<Pixel> pixels, int w, int h);
        static void export_FUN(CompositeOperation* co, std::string path);
        static CompositeOperation* import_FUN(std::string path, std::vector<Selection>* selections);

    protected:

    private:
        static int process_FUN(CompositeOperation* co, std::ifstream& file);
        static void recursive_delete(CompositeOperation* co);
        Formater();
        virtual ~Formater();
};

#endif // FORMATER_H
