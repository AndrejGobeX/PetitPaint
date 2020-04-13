#ifndef FORMATER_H
#define FORMATER_H

#include<string>

class Image;
class SDL_Surface;

class Formater
{
    public:
        static Image* read_PAM(std::string path);

    protected:

    private:
        Formater();
        virtual ~Formater();
};

#endif // FORMATER_H
