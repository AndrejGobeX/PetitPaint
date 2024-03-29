#ifndef LAYER_H
#define LAYER_H

#include"Image.h"

class Layer
{
    public:
        Layer();
        Layer(Image* _image, bool _delete_after);
        virtual ~Layer();

        Image* get_image(){return image;}
        void set_delete_after(bool _delete_after){delete_after=_delete_after;}

        bool isVisible(){return visible;}
        void setVisible(bool _visible){visible=_visible;}

        void set_opacity(int _opacity){opacity=_opacity;}
        int get_opacity(){return opacity;}

    protected:

    private:
        Image* image=nullptr;
        bool visible=true;
        bool delete_after=false;
        int opacity=100;
};

#endif // LAYER_H
