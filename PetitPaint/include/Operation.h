#ifndef OPERATION_H
#define OPERATION_H

#include<SDL.h>

typedef Uint32 (*vpointer)(Uint32 pixel, int param);

class Operation
{
    public:
        Operation(vpointer _fun);
        virtual ~Operation();

        Uint32 operator()(Uint32 pixel, int param);

    protected:

    private:
        vpointer fun=nullptr;
}add([](Uint32 pixel, int param)->Uint32{
    return pixel;
});

#endif // OPERATION_H
