#ifndef LAYER_H
#define LAYER_H

#include"Image.h"

class Layer
{
    public:
        Layer();
        Layer(Image* _image, bool _delete_after);
        virtual ~Layer();

    protected:

    private:
        Image* image=nullptr;
        bool visible=true;
        bool delete_after=false;
};

#endif // LAYER_H
