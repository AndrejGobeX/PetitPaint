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
    int x, y; int h=pixels.size()/w;
    std::vector<Pixel> c_pixels;

    bool yes, mega_yes=true, med=false;
    if(label=="med")med=true;
    if(med)c_pixels=pixels;

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

        if(!med)(*f)(p, par);
        else
        {
            int r=p.R, g=p.G, b=p.B, c=1;
            if(x>0)
            {
                c++;
                r+=c_pixels[p.indx-1].R;
                g+=c_pixels[p.indx-1].G;
                b+=c_pixels[p.indx-1].B;
            }
            if(x<w-1)
            {
                c++;
                r+=c_pixels[p.indx+1].R;
                g+=c_pixels[p.indx+1].G;
                b+=c_pixels[p.indx+1].B;
            }
            if(y>0)
            {
                c++;
                r+=c_pixels[p.indx-w].R;
                g+=c_pixels[p.indx-w].G;
                b+=c_pixels[p.indx-w].B;
            }
            if(y<h-1)
            {
                c++;
                r+=c_pixels[p.indx+w].R;
                g+=c_pixels[p.indx+w].G;
                b+=c_pixels[p.indx+w].B;
            }
            p.R=r/c;
            p.G=g/c;
            p.B=b/c;
        }
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

