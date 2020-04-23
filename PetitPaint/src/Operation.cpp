#include "Operation.h"

Operation::Operation(std::string _label, std::vector<Selection>* _selections, Fun _f):
            label(_label), selections(_selections), f(_f)
{
    //ctor
}

Operation::~Operation()
{
    //dtor
}

void Operation::operator()(std::vector<Pixel>& pixels, int w)
{
    int x, y;
    bool yes, mega_yes=true;
    if(selections->size()==0)mega_yes=true;
    else
    {
        for(Selection& s:(*selections))
        {
            if(s.isEnabled())
            {
                mega_yes=false;
                break;
            }
        }
    }
    for(Pixel& p:pixels)
    {
        x=p.indx%w;
        y=p.indx/w;
        yes=false;
        if(mega_yes)yes=true;
        else for(Selection& s:(*selections))
        {
            if(s.isEnabled())
            for(SDL_Rect& r:s.rects)
            {
                if(x>=r.x-100 && x<=r.x-100+r.w &&
                   y>=r.y && y<=r.y+r.h)
                {
                    yes=true;
                    break;
                }
            }
            if(yes)break;
        }
        if(!yes)continue;

        (*f)(p, par);
    }
}


void _add(Pixel& pixel, int p)
{
    pixel.R+=p;
    pixel.G+=p;
    pixel.B+=p;
}

void _sub(Pixel& pixel, int p)
{
    pixel.R-=p;
    pixel.G-=p;
    pixel.B-=p;
}

void _isb(Pixel& pixel, int p)
{
    pixel.R=p-pixel.R;
    pixel.G=p-pixel.G;
    pixel.B=p-pixel.B;
}

void _mul(Pixel& pixel, int p)
{
    pixel.R*=p;
    pixel.G*=p;
    pixel.B*=p;
}

void _div(Pixel& pixel, int p)
{
    pixel.R/=p;
    pixel.G/=p;
    pixel.B/=p;
}

void _idv(Pixel& pixel, int p)
{
    pixel.R=p/pixel.R;
    pixel.G=p/pixel.G;
    pixel.B=p/pixel.B;
}

void _pow(Pixel& pixel, int p)
{
    pixel.R=pow(pixel.R, p);
    pixel.G=pow(pixel.G, p);
    pixel.B=pow(pixel.B, p);
}

void _log(Pixel& pixel, int p)
{
    pixel.R=log(pixel.R);
    pixel.G=log(pixel.G);
    pixel.B=log(pixel.B);
}

void _abs(Pixel& pixel, int p)
{
    pixel.R=abs(pixel.R);
    pixel.G=abs(pixel.G);
    pixel.B=abs(pixel.B);
}

void _max(Pixel& pixel, int p)
{
    pixel.R=std::max(pixel.R, p);
    pixel.G=std::max(pixel.G, p);
    pixel.B=std::max(pixel.B, p);
}

void _min(Pixel& pixel, int p)
{
    pixel.R=std::min(pixel.R, p);
    pixel.G=std::min(pixel.G, p);
    pixel.B=std::min(pixel.B, p);
}

void _inv(Pixel& pixel, int p)
{
    _isb(pixel, 255);
}

void _grs(Pixel& pixel, int p)
{
    int avg=pixel.R+pixel.G+pixel.B;
    avg/=3;
    pixel.R=avg;
    pixel.G=avg;
    pixel.B=avg;
}

void _baw(Pixel& pixel, int p)
{
    int avg=pixel.R+pixel.G+pixel.B;
    avg/=3;
    pixel.R=avg<127?0:255;
    pixel.G=avg<127?0:255;
    pixel.B=avg<127?0:255;
}

