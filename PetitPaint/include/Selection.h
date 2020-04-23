#ifndef SELECTION_H
#define SELECTION_H

#include<SDL.h>
#include<vector>
#include<string>

class Selection
{
    public:
        Selection(SDL_Rect _rect);
        Selection();
        virtual ~Selection();
        std::vector<SDL_Rect> rects;
        bool isEnabled(){return enabled;}
        void setEnabled(bool _enabled){enabled=_enabled;}
        std::string label;
    protected:

    private:

        bool enabled=true;
};

#endif // SELECTION_H
