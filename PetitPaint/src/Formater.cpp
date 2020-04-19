#include "Formater.h"
#include<SDL.h>
#include<iostream>
#include<fstream>
#include<regex>
#include"Image.h"

Formater::Formater()
{
}

Formater::~Formater()
{
}

Image* Formater::read_PAM(std::string path)
{
    std::ifstream file(path.c_str(), std::ios::binary);
    if(!file)
    {
        std::cout<<"Failed to open file\n";
        return nullptr;
    }

    unsigned char*  pixels;
    int    width;
    int    height;
    int    depth;
    int maxval;
    std::string tupltype;

    Uint32 rmask, gmask, bmask, amask;

    std::string s;
    std::smatch match;
    std::regex rx_width("WIDTH ([0-9]*)"),
        rx_height("HEIGHT ([0-9]*)"), rx_depth("DEPTH ([0-9]*)"),
        rx_maxval("MAXVAL ([0-9]*)"), rx_tupltype("TUPLTYPE ([A-Z]*[\u005F]*[A-Z]*)");

    //Check format
    getline(file, s);
    if(s!="P7")
    {
        std::cout<<"Wrong file format or file is corrupted.\n";
        file.close();
        return nullptr;
    }

    //Check width
    getline(file, s);
    if(regex_match(s, match, rx_width))
    {
        width=atoi(match[1].str().c_str());
    }
    else
    {
        std::cout<<"File is corrupted.\n";
        file.close();
        return nullptr;
    }

    //Check height
    getline(file, s);
    if(regex_match(s, match, rx_height))
    {
        height=atoi(match[1].str().c_str());
    }
    else
    {
        std::cout<<"File is corrupted.\n";
        file.close();
        return nullptr;
    }

    //Check depth
    getline(file, s);
    if(regex_match(s, match, rx_depth))
    {
        depth=atoi(match[1].str().c_str());
    }
    else
    {
        std::cout<<"File is corrupted.\n";
        file.close();
        return nullptr;
    }

    //Check maxval
    getline(file, s);
    if(regex_match(s, match, rx_maxval))
    {
        maxval=atoi(match[1].str().c_str());
        if(maxval>255)
        {
            std::cout<<"64bit images not supported (yet).\n";
            file.close();
            return nullptr;
        }
    }
    else
    {
        std::cout<<"File is corrupted.\n";
        file.close();
        return nullptr;
    }

    //Check tupltype
    getline(file, s);
    if(regex_match(s, match, rx_tupltype))
    {
        tupltype=match[1].str();
    }
    else
    {
        std::cout<<"File is corrupted.\n";
        file.close();
        return nullptr;
    }

    //Header end
    getline(file, s);

    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    int shift = (tupltype != "RGB_ALPHA") ? 8 : 0;
    rmask = 0xff000000 >> shift;
    gmask = 0x00ff0000 >> shift;
    bmask = 0x0000ff00 >> shift;
    amask = 0x000000ff >> shift;
    #else // little endian, like x86
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = (tupltype != "RGB_ALPHA") ? 0 : 0xff000000;
    #endif

    //Array of pixels
    pixels=new unsigned char[width*height*depth];

    unsigned current_pixel=0;

    while(getline(file, s))
    {
        for(unsigned char c:s)
        {
            pixels[current_pixel]=0+c;
            ++current_pixel;
        }
    }
    file.close();

    SDL_Surface* image_surface;
    image_surface=SDL_CreateRGBSurfaceFrom((void*)pixels, width, height, depth<<3, depth*width,
                                           rmask, gmask, bmask, amask);
    if(image_surface==nullptr)
    {
        std::cout<<"Could not load image. Error: "<<SDL_GetError()<<"\n";
        delete [] pixels;
        pixels=nullptr;
        return nullptr;
    }

    Image* image=new Image(image_surface, pixels, width, height, depth);

    return image;
}

void Formater::export_BMP(SDL_Surface* surface, std::string path)
{
    if(SDL_SaveBMP(surface, path.c_str())<0)
    {
        std::cout<<"Could not export bmp. Error: "<<SDL_GetError()<<"\n";
    }
}

void Formater::export_PAM(SDL_Surface* surface, std::string path)
{
    std::ofstream file(path.c_str(), std::ios::binary);

    file<<"P7\n";
    file<<"WIDTH "<<surface->w<<"\n";
    file<<"HEIGHT "<<surface->h<<"\n";
    file<<"DEPTH 4\n";
    file<<"MAXVAL 255\n";
    file<<"TUPLTYPE RGB_ALPHA\n";
    file<<"ENDHDR\n";

    char* pixels=(char*)surface->pixels;

    for(unsigned i=0; i<surface->w*surface->h*4; ++i)
    {
        file<<pixels[i];
    }
    file<<EOF;

    file.close();
}
