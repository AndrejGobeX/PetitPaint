#include "Layer.h"

Layer::Layer()
{
}

Layer::Layer(Image* _image, bool _delete_after):image(_image), delete_after(_delete_after)
{
}

Layer::~Layer()
{
    if(delete_after && image)
    {
        delete image;
    }
    image=nullptr;
}
