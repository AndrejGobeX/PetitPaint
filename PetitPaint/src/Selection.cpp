#include "Selection.h"

Selection::Selection(SDL_Rect _rect)
{
    rects.push_back(_rect);
}

Selection::Selection()
{

}

Selection::~Selection()
{
    //dtor
}
