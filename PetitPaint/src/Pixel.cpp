#include "Pixel.h"
#include<iostream>

Pixel::Pixel()
{
    //ctor
}

Pixel::~Pixel()
{
    //dtor
}

void Pixel::ceil_floor()
{
    R=std::max(R, 0);
    R=std::min(R, 255);
    G=std::max(G, 0);
    G=std::min(G, 255);
    B=std::max(B, 0);
    B=std::min(B, 255);
    A=std::max(A, 0);
    A=std::min(A, 255);
}
