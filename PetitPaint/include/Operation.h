#ifndef OPERATION_H
#define OPERATION_H

#include"Pixel.h"
#include<vector>
#include<string>
#include"Selection.h"
#include<cmath>
#include<algorithm>

typedef void (*Fun)(Pixel& pixel, int p);

class Operation
{
    friend class Formater;
    friend class Window;
    public:
        Operation(std::string _label, std::vector<Selection>* _selections, Fun _f);
        virtual ~Operation();

        void operator()(int _param){par=_param;}
        virtual void operator()(std::vector<Pixel>& pixels, int w);

        virtual std::string get_label(){return label;}
        virtual std::string get_all_ops(){return label+" "+std::to_string(par);}

        virtual int get_class(){return 0;}

    protected:

    private:
        std::string label;
        std::vector<Selection>* selections=nullptr;
        Fun f=nullptr;
        int par=0;
};

void _add(Pixel& pixel, int p);
void _sub(Pixel& pixel, int p);
void _isb(Pixel& pixel, int p);
void _mul(Pixel& pixel, int p);
void _div(Pixel& pixel, int p);
void _idv(Pixel& pixel, int p);
void _pow(Pixel& pixel, int p);
void _log(Pixel& pixel, int p);
void _abs(Pixel& pixel, int p);
void _max(Pixel& pixel, int p);
void _min(Pixel& pixel, int p);
void _inv(Pixel& pixel, int p);
void _grs(Pixel& pixel, int p);
void _baw(Pixel& pixel, int p);
void _col(Pixel& pixel, int p);

#endif // OPERATION_H
