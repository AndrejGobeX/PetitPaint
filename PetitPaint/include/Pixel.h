#ifndef PIXEL_H
#define PIXEL_H


class Pixel
{
    public:
        Pixel();
        virtual ~Pixel();

        int R=0, G=0, B=0, A=0;

        int indx=-1;

        void ceil_floor();

    protected:

    private:
};

#endif // PIXEL_H
