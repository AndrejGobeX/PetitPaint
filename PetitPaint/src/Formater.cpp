#include "Formater.h"
#include<SDL.h>
#include<iostream>
#include<fstream>
#include<regex>
#include"Image.h"
#include"CompositeOperation.h"
#include<string>

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
    if(!file)
    {
        std::cout<<"Error exporting image.\n";
        return;
    }

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

std::vector<Pixel> Formater::surface_to_pixels(SDL_Surface* surface)
{
    std::vector<Pixel> vp;
    Pixel p;
    Uint32* temp;
    for(int i=0; i<surface->w*surface->h; ++i)
    {
        temp=((Uint32*)(surface->pixels))+i;
        p.R=((unsigned char*)temp)[0];
        p.G=((unsigned char*)temp)[1];
        p.B=((unsigned char*)temp)[2];
        p.A=((unsigned char*)temp)[3];
        p.indx=i;
        vp.push_back(p);
    }
    return vp;
}

SDL_Surface* Formater::pixels_to_surface(std::vector<Pixel> pixels, int w, int h)
{
    SDL_Surface* surface=SDL_CreateRGBSurfaceWithFormat(0, w, h, 32,
                                                SDL_PIXELFORMAT_RGBA32);
    unsigned char temp[4];
    for(int i=0; i<surface->w*surface->h; ++i)
    {
        pixels[i].ceil_floor();
        temp[0]=pixels[i].R;
        temp[1]=pixels[i].G;
        temp[2]=pixels[i].B;
        temp[3]=pixels[i].A;
        ((Uint32*)(surface->pixels))[i]=*((Uint32*)temp);
    }
    return surface;
}

void Formater::export_FUN(CompositeOperation* co, std::string path)
{
    std::ofstream file(path.c_str());
    if(!file)
    {
        std::cout<<"Error exporting composite operation.\n";
        return;
    }

    file<<(co->get_all_ops());

    file.close();
}

CompositeOperation* Formater::import_FUN(std::string path, std::vector<Selection>* selections)
{
    if(path.substr(path.size()-4, 4)!=".fun")
    {
        std::cout<<"Wrong file format (expected .fun).\n";
        return nullptr;
    }
    std::ifstream file(path.c_str());
    if(!file)
    {
        std::cout<<"Error opening file.\n";
        return nullptr;
    }

    std::string s, c;
    file>>s>>c;
    if(c!="-c")
    {
        std::cout<<"File is corrupted.\n";
        file.close();
        return nullptr;
    }
    if(s=="add" ||
       s=="sub" ||
       s=="isb" ||
       s=="mul" ||
       s=="div" ||
       s=="idv" ||
       s=="pow" ||
       s=="log" ||
       s=="abs" ||
       s=="max" ||
       s=="min" ||
       s=="inv" ||
       s=="grs" ||
       s=="baw" ||
       s=="med")
    {
        std::cout<<"Composite operation cannot be named "<<s<<".\n";
        file.close();
        return nullptr;
    }
    CompositeOperation* co=new CompositeOperation(s, selections);
    if(process_FUN(co, file)==-1)
    {
        Formater::recursive_delete(co);
        delete co;
        co=nullptr;
    }

    file.close();
    return co;
}

int Formater::process_FUN(CompositeOperation* co, std::ifstream& file)
{
    std::string s, c;
    file>>s;
    while(s!="-e")
    {
        file>>c;
        if(s=="add")
        {
            Operation* o=new Operation(s, co->selections, _add);
            (*o)(atoi(c.c_str()));
            co->push_back(o);
        }
        else if(s=="sub")
        {
            Operation* o=new Operation(s, co->selections, _sub);
            (*o)(atoi(c.c_str()));
            co->push_back(o);
        }
        else if(s=="isb")
        {
            Operation* o=new Operation(s, co->selections, _isb);
            (*o)(atoi(c.c_str()));
            co->push_back(o);
        }
        else if(s=="mul")
        {
            Operation* o=new Operation(s, co->selections, _mul);
            (*o)(atoi(c.c_str()));
            co->push_back(o);
        }
        else if(s=="div")
        {
            Operation* o=new Operation(s, co->selections, _div);
            (*o)(atoi(c.c_str()));
            co->push_back(o);
        }
        else if(s=="idv")
        {
            Operation* o=new Operation(s, co->selections, _idv);
            (*o)(atoi(c.c_str()));
            co->push_back(o);
        }
        else if(s=="pow")
        {
            Operation* o=new Operation(s, co->selections, _pow);
            (*o)(atoi(c.c_str()));
            co->push_back(o);
        }
        else if(s=="log")
        {
            Operation* o=new Operation(s, co->selections, _log);
            (*o)(atoi(c.c_str()));
            co->push_back(o);
        }
        else if(s=="abs")
        {
            Operation* o=new Operation(s, co->selections, _abs);
            (*o)(atoi(c.c_str()));
            co->push_back(o);
        }
        else if(s=="max")
        {
            Operation* o=new Operation(s, co->selections, _max);
            (*o)(atoi(c.c_str()));
            co->push_back(o);
        }
        else if(s=="min")
        {
            Operation* o=new Operation(s, co->selections, _min);
            (*o)(atoi(c.c_str()));
            co->push_back(o);
        }
        else if(s=="inv")
        {
            Operation* o=new Operation(s, co->selections, _inv);
            (*o)(atoi(c.c_str()));
            co->push_back(o);
        }
        else if(s=="grs")
        {
            Operation* o=new Operation(s, co->selections, _grs);
            (*o)(atoi(c.c_str()));
            co->push_back(o);
        }
        else if(s=="baw")
        {
            Operation* o=new Operation(s, co->selections, _baw);
            (*o)(atoi(c.c_str()));
            co->push_back(o);
        }
        else if(s=="med")
        {
            Operation* o=new Operation(s, co->selections, nullptr);
            (*o)(atoi(c.c_str()));
            co->push_back(o);
        }
        else
        {
            if(c=="-c")
            {
                CompositeOperation* coc=new CompositeOperation(s, co->selections);
                if(process_FUN(coc, file)==-1)
                {
                    delete coc;
                    return -1;
                }
                else
                {
                    co->push_back(coc);
                }
            }
        }
        file>>s;
    }
    return 0;
}

void Formater::recursive_delete(CompositeOperation* co)
{
    for(Operation* o:co->operations)
    {
        if(o->get_class()==0)delete o;
        else
        {
            recursive_delete((CompositeOperation*)o);
            delete o;
        }
    }
    co->operations.clear();
}
