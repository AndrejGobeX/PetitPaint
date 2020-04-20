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

    int    width;
    int    height;
    int    depth;
    int    maxval;
    std::string tupltype;

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

    SDL_Surface* image_surface=SDL_CreateRGBSurfaceWithFormat(0, width,
                    height, 32, SDL_PIXELFORMAT_RGBA32);
    if(image_surface==nullptr)
    {
        std::cout<<"Could not create surface. Error: "<<SDL_GetError()<<"\n";
        return nullptr;
    }

    unsigned char temp[4];

    std::cout<<std::hex;
    for(long long i=0; i<width*height; ++i)
    {
        file.read((char*)&temp, depth);
        if(depth==3)temp[3]=0xff;
        ((Uint32*)(image_surface->pixels))[i]=*((Uint32*)temp);
    }

    file.close();

    Image* img=new Image(image_surface, width, height, depth);

    return img;
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
    int i=0;
    for(i=0; i<surface->w*surface->h*4; ++i)
    {
        file<<pixels[i];
    }
    file<<EOF;

    file.close();
}
