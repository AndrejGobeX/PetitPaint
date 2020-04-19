#ifndef FORMATER_H
#define FORMATER_H

#include<string>

class Image;
class SDL_Surface;

class Formater
{
    public:
        static Image* read_PAM(std::string path);
        static void export_BMP(SDL_Surface* surface, std::string path);
        static void export_PAM(SDL_Surface* surface, std::string path);

    protected:

    private:
        Formater();
        virtual ~Formater();
};

#endif // FORMATER_H
