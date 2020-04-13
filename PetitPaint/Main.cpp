#include<iostream>
#include<SDL.h>
#include"Window.h"
#include<fstream>
#include<string>
#include<regex>

using namespace std;

const int SCREEN_HEIGHT=500;
const int SCREEN_WIDTH=500;

SDL_Surface* read_PAM(string path)
{
    ifstream file(path.c_str(), ios::binary);
    if(!file)
    {
        cout<<"Failed to open file\n";
        return nullptr;
    }

    unsigned char*  pixels;
    int    width;
    int    height;
    int    depth;
    int maxval;
    string tupltype;

    Uint32 rmask, gmask, bmask, amask;

    string s;
    smatch match;
    regex rx_width("WIDTH ([0-9]*)"),
        rx_height("HEIGHT ([0-9]*)"), rx_depth("DEPTH ([0-9]*)"),
        rx_maxval("MAXVAL ([0-9]*)"), rx_tupltype("TUPLTYPE ([A-Z]*)");

    //Check format
    getline(file, s);
    if(s!="P7")
    {
        cout<<"Wrong file format or file is corrupted.\n";
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
        cout<<"File is corrupted.\n";
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
        cout<<"File is corrupted.\n";
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
        cout<<"File is corrupted.\n";
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
            cout<<"64bit images not supported (yet).\n";
            file.close();
            return nullptr;
        }
    }
    else
    {
        cout<<"File is corrupted.\n";
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
        cout<<"File is corrupted.\n";
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
        cout<<"Could not load image. Error: "<<SDL_GetError()<<"\n";
        return nullptr;
    }

    return image_surface;

}

int main(int argc, char* argv[])
{
    Window win(SCREEN_HEIGHT, SCREEN_WIDTH);
    win.set_background("samples\\PetitPaint.bmp");

    bool n_quit=true;
    SDL_Event e;

    while(n_quit)
    {
        while(SDL_PollEvent(&e)!=0)
        {
            if(e.type==SDL_QUIT)n_quit=false;
        }
    }

    return 0;
}
