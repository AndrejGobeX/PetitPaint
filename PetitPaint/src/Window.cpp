#include "Window.h"
#include<iostream>
#include<fstream>
#include<string>
#include"Formater.h"

const int INIT_ERROR=1, WINDOW_ERROR=2, RENDER_ERROR=3;

Window::Window(const int& SCREEN_HEIGHT, const int& SCREEN_WIDTH): width(SCREEN_WIDTH), height(SCREEN_HEIGHT)
{
    if(SDL_Init(SDL_INIT_VIDEO)<0)
    {
        std::cout<<"SDL could not initialize. Error: "<<SDL_GetError()<<"\n";
        throw INIT_ERROR;
    }
    window = SDL_CreateWindow( "PetitPaint", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH+100, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if(window==nullptr)
    {
        std::cout<<"SDL window not created. Error: "<<SDL_GetError()<<"\n";
        throw WINDOW_ERROR;
    }

    screenSurface=SDL_GetWindowSurface(window);
    SDL_FillRect(screenSurface, nullptr, SDL_MapRGBA(screenSurface->format, 0x0, 0x0, 0x0, 0xFF));
    SDL_UpdateWindowSurface(window);
    renderer=SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer==nullptr)
    {
        std::cout<<"SDL renderer not created. Error: "<<SDL_GetError()<<"\n";
        throw RENDER_ERROR;
    }
    SDL_SetRenderDrawColor(renderer, 0x00, 0xAA, 0xAA, 0xFF);

    rect.h=height;
    rect.w=width;
    rect.x=100;
    rect.y=0;
    rect2=rect;
    menu_rect.h=height;
    menu_rect.w=100;
    menu_rect.x=0;
    menu_rect.y=0;
    menu=SDL_LoadBMP("menu\\menu.bmp");
    //SDL_BlitSurface(menu, nullptr, screenSurface, nullptr);

    texture=SDL_CreateTextureFromSurface(renderer, menu);
    SDL_RenderCopy(renderer, texture, &menu_rect, &menu_rect);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(texture);
    texture=nullptr;

    format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);

    set_background("samples\\PetitPaint.bmp");
    refresh();

    rect2.w=0;
    rect2.h=0;
    rect=rect2;
    delete_layers();
    selections.clear();
    clear();
    refresh();

    add("samples\\petitpaint.bmp");
    reload();
}

Window::~Window()
{
    SDL_DestroyWindow(window);
    if(renderer!=nullptr)
    {
        SDL_DestroyRenderer(renderer);
        renderer=nullptr;
    }
    if(menu!=nullptr)
    {
        SDL_FreeSurface(menu);
        menu=nullptr;
    }
    if(background!=nullptr)
    {
        SDL_FreeSurface(background);
        background=nullptr;
    }
    if(format!=nullptr)
    {
        SDL_FreeFormat(format);
        format=nullptr;
    }
    delete_layers();
    delete_composites();
    SDL_Quit();
}

void Window::delete_layers()
{
    std::for_each(layers.begin(), layers.end(), [](Layer* l){delete l;});
    layers.clear();
}

void Window::delete_composites()
{
    std::for_each(composites.rbegin(), composites.rend(), [](CompositeOperation* l){delete l;});
    composites.clear();
}

void Window::clear()
{
    SDL_FillRect(background, nullptr, SDL_MapRGBA(format, 0x0, 0x0, 0x0, 0x0));
}

void Window::swap_background(SDL_Surface* surface)
{
    if(background!=nullptr)
    {
        SDL_FreeSurface(background);
    }
    background=surface;
    for(int i=0; i<background->w*background->h*4; ++i)
    {
        ((char*)background->pixels)[i]=0;
    }
}

void Window::set_background(std::string path)
{
    SDL_Surface* srf=SDL_LoadBMP(path.c_str());
    if(srf==nullptr)
    {
        std::cout<<"Could not load image. Error: "<<SDL_GetError()<<"\n";
        return;
    }
    background=SDL_ConvertSurface(srf, format, 0);
    SDL_FreeSurface(srf);
}

void Window::blit_to_background(SDL_Surface* surface)
{
    SDL_BlitSurface(surface, nullptr, background, nullptr);
}

void Window::refresh()
{
    texture=SDL_CreateTextureFromSurface(renderer, screenSurface);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);

    SDL_DestroyTexture(texture);
    texture=nullptr;

    texture=SDL_CreateTextureFromSurface(renderer, menu);
    SDL_RenderCopy(renderer, texture, &menu_rect, &menu_rect);

    SDL_DestroyTexture(texture);
    texture=nullptr;

    texture=SDL_CreateTextureFromSurface(renderer, background);
    SDL_RenderCopy(renderer, texture, nullptr, &rect);

    SDL_DestroyTexture(texture);
    texture=nullptr;

    SDL_RenderDrawLine(renderer, rect.w+1+100, 0, rect.w+1+100, rect.h+1);
    SDL_RenderDrawLine(renderer, 100, rect.h+1, rect.w+1+100, rect.h+1);
    SDL_RenderDrawLine(renderer, 99, 0, 99, rect.h+1);

    SDL_SetRenderDrawColor(renderer, 0x00, 0xAA, 0x11, 0x22);

    if(preview_sel)
    {
        //Selections
        for(Selection sel:selections)
        {
            if(sel.isEnabled())
            {
                for_each(sel.rects.begin(), sel.rects.end(),
                [this](SDL_Rect r)
                {
                    SDL_RenderDrawRect(renderer, &r);
                });
            }
        }
    }

    SDL_SetRenderDrawColor(renderer, 0x00, 0xAA, 0xAA, 0xFF);

    SDL_RenderPresent(renderer);
}

void Window::reload()
{
    clear();
    for(Layer* l:layers)
    {
        //Resize layer
        if(l->get_image()->get_surface()->w<rect.w || l->get_image()->get_surface()->h<rect.h)
        {
            SDL_Surface* temp=l->get_image()->get_surface();
            l->get_image()->set_surface( SDL_CreateRGBSurfaceWithFormat(0, rect.w, rect.h, 32, format->format) );
            SDL_BlitSurface(temp, nullptr, l->get_image()->get_surface(), nullptr);
            SDL_FreeSurface(temp);
            l->get_image()->refresh();
        }
        if(l->isVisible())
        {
            if(l->get_opacity()==100)
                SDL_BlitSurface(l->get_image()->get_surface(), nullptr, background, nullptr);
            else
            {
                SDL_Surface* surface_copy=SDL_CreateRGBSurfaceWithFormat(0, rect.w, rect.h, 32, format->format);
                SDL_BlitSurface(l->get_image()->get_surface(), nullptr, surface_copy, nullptr);
                long long temp;
                //Change A for every pixel
                for(int i=3; i<rect.w*rect.h*4; i+=4)
                {
                    temp=((unsigned char*)(l->get_image()->get_surface()->pixels))[i];
                    temp*=l->get_opacity();
                    temp/=100;
                    ((unsigned char*)(surface_copy->pixels))[i]=(unsigned char)temp;
                }
                SDL_BlitSurface(surface_copy, nullptr, background, nullptr);
                SDL_FreeSurface(surface_copy);
            }
        }
    }
    refresh();
}

void Window::out(std::string path)
{
    std::regex bmp("(.*)\u002Ebmp");
    std::regex pam("(.*)\u002Epam");

    std::smatch match;

    if(regex_match(path, match, bmp))
    {
        Formater::export_BMP(background, path);
    }
    else if(regex_match(path, match, pam))
    {
        Formater::export_PAM(background, path);
    }
    else
    {
        std::cout<<"File type not supported (yet).\n";
        return;
    }
}

void Window::add(std::string path)
{
    SDL_Surface* surface=nullptr;
    Image* img=nullptr;

    std::regex bmp("(.*)\u002Ebmp");
    std::regex pam("(.*)\u002Epam");
    std::regex no("empty");
    std::regex nowh("empty -w ([0-9]+) -h ([0-9]+)");

    std::smatch match;

    if(regex_match(path, match, bmp))
    {
        surface=SDL_LoadBMP(path.c_str());
        if(surface==nullptr)
        {
            std::cout<<"Cannot open file or it doesn't exits. Error: "<<SDL_GetError()<<"\n";
            return;
        }
        img=new Image(SDL_ConvertSurface(surface, format, 0), surface->w, surface->h, 4);
        SDL_FreeSurface(surface);
    }
    else if(regex_match(path, match, pam))
    {
        Image* temp=nullptr;
        temp=Formater::read_PAM(path);
        if(temp!=nullptr){
        img=new Image(SDL_ConvertSurface(temp->get_surface(), format, 0), temp->get_width(), temp->get_height(), 4);
        delete temp;
        }
    }
    else if(regex_match(path, match, nowh))
    {
        surface=SDL_CreateRGBSurfaceWithFormat(0, atoi(match[1].str().c_str()),
                        atoi(match[2].str().c_str()), 32, format->format);
        if(surface==nullptr)
        {
            std::cout<<"Cannot create layer. Error: "<<SDL_GetError()<<"\n";
            return;
        }
        img=new Image(SDL_ConvertSurface(surface, format, 0), surface->w, surface->h, 4);
        SDL_FreeSurface(surface);
    }
    else if(regex_match(path, match, no))
    {
        if(layers.size()==0)
        {
            std::cout<<"First layer dimensions must be specified, use: empty -w width -h height\n";
            return;
        }
        surface=SDL_CreateRGBSurfaceWithFormat(0, rect.w, rect.h, 32, format->format);
        if(surface==nullptr)
        {
            std::cout<<"Cannot create layer. Error: "<<SDL_GetError()<<"\n";
            return;
        }
        img=new Image(SDL_ConvertSurface(surface, format, 0), surface->w, surface->h, 4);
        SDL_FreeSurface(surface);
    }
    else
    {
        std::cout<<"File type not supported (yet).\n";
        return;
    }
    if(img==nullptr || img->get_surface()==nullptr)
    {
        std::cout<<"Error processing image.\n";
        if(img)delete img;
        return;
    }
    layers.push_back((new Layer(img, true)));

    rect.h=std::max((unsigned)rect.h, img->get_height());//Resizing background
    rect.w=std::max((unsigned)rect.w, img->get_width()); //if necessary
    rect2=rect;

    //Swap new dimensions
    swap_background(SDL_CreateRGBSurfaceWithFormat(0, rect.w, rect.h, 32, format->format));
}

void Window::apply(Operation* operation)
{
    for(Layer* layer:layers)
    {
        std::vector<Pixel> pixels=
            Formater::surface_to_pixels(layer->get_image()->get_surface());

        (*operation)(pixels, layer->get_image()->get_surface()->w);

        SDL_FreeSurface(layer->get_image()->get_surface());
        layer->get_image()->set_surface(Formater::pixels_to_surface(pixels,
                    layer->get_image()->get_width(), layer->get_image()->get_height()));
    }
}

void Window::handle_command(std::string s)
{
    std::regex togglel("toggle -l ([0-9]+)");
    std::regex toggles("toggle -s ([0-9]+)");
    std::regex toggless("toggle -s -l ([a-zA-Z0-9]+)");
    std::regex mksel("mksel ([a-zA-Z0-9_]+) -n ([0-9]+)");
    std::regex rm("rm ([0-9]+)");
    std::regex pwsl("pwsl");
    std::regex op("op ([a-z]+)");
    std::regex opp("op ([a-z]+) ([a-zA-Z0-9]+)");
    std::regex opc("op -c ([a-z]+) -n ([0-9]+)([\u0020-k]*)");
    std::regex ope("op -e ([a-z]+) -f (.*)");
    std::regex opi("op -i (.+)");
    std::regex cls("cls");
    std::regex sw("sw -l ([0-9]+) ([0-9]+)");
    std::regex setop("setop ([0-9]+) -l ([0-9]+)");

    std::smatch match;
    if(regex_match(s, match, cls))
    {
        system("cls");
    }
    else if(regex_match(s, match, setop))
    {
        int percentage=atoi(match[1].str().c_str());
        unsigned layer=atoi(match[2].str().c_str());
        layer=layers.size()-layer;
        if(layer>=layers.size() || layer<0)
        {
            std::cout<<"Index out of bounds. Current layer count: "<<layers.size()<<"\n";
            return;
        }
        if(percentage<0 || percentage>100)
        {
            std::cout<<"Opacity out of range (0-100).\n";
            return;
        }
        layers[layer]->set_opacity(percentage);
    }
    else if(regex_match(s, match, pwsl))
    {
        preview_sel=!preview_sel;
    }
    else if(regex_match(s, match, togglel))
    {
        unsigned layer=atoi(match[1].str().c_str());
        layer=layers.size()-layer;
        if(layer>=layers.size() || layer<0)
        {
            std::cout<<"Index out of bounds. Current layer count: "<<layers.size()<<"\n";
            return;
        }
        layers[layer]->setVisible(!layers[layer]->isVisible());
    }
    else if(regex_match(s, match, toggless))
    {
        std::string sel=match[1].str();
        for(Selection& sele:selections)
        {
            if(sele.label==sel)
                sele.setEnabled(!sele.isEnabled());
        }
    }
    else if(regex_match(s, match, toggles))
    {
        unsigned sel=atoi(match[1].str().c_str());
        sel=selections.size()-sel;
        if(sel>=selections.size() || sel<0)
        {
            std::cout<<"Index out of bounds. Current selection count: "<<selections.size()<<"\n";
            return;
        }
        selections[sel].setEnabled(!selections[sel].isEnabled());
    }
    else if(regex_match(s, match, mksel))
    {
        Selection sel;
        sel.label=match[1].str();
        for(int i=0; i<atoi(match[2].str().c_str()); ++i)
        {
            std::cout<<"Rect "<<i+1<<" x y w h: ";
            int x, y, w, h;
            std::cin>>x>>y>>w>>h;
            SDL_Rect r;

            r.x=x+100;
            r.y=y;
            r.w=w;
            r.h=h;

            sel.rects.push_back(r);
        }
        selections.push_back(sel);
        std::getchar();
    }
    else if(regex_match(s, match, rm))
    {
        unsigned layer=atoi(match[1].str().c_str());
        layer=layers.size()-layer;
        if(layer>=layers.size() || layer<0)
        {
            std::cout<<"Index out of bounds. Current layer count: "<<layers.size()<<"\n";
            return;
        }
        delete layers[layer];
        layers.erase(layers.begin()+layer);
    }
    else if(regex_match(s, match, opc))
    {
        std::string lbl=match[1].str();
        int n=atoi(match[2].str().c_str());
        std::string keep=match[3].str();
        if(n<1)
        {
            std::cout<<"You're kidding, right?\n";
            return;
        }

        CompositeOperation* co=new CompositeOperation(lbl, &selections);
        for(int i=0; i<n; ++i)
        {
            std::cout<<"Operation "<<i+1<<":\n\t";
            std::getline(std::cin, s);

            if(regex_match(s, match, opp))
            {
                int par=atoi(match[2].str().c_str());
                s=match[1].str();
                if(s=="add")
                {
                    Operation* o=new Operation("add", &selections, _add);
                    (*o)(par);
                    co->push_back(o);
                }
                else if(s=="sub")
                {
                    Operation* o=new Operation("sub", &selections, _sub);
                    (*o)(par);
                    co->push_back(o);
                }
                else if(s=="isb")
                {
                    Operation* o=new Operation("isb", &selections, _isb);
                    (*o)(par);
                    co->push_back(o);
                }
                else if(s=="mul")
                {
                    Operation* o=new Operation("mul", &selections, _mul);
                    (*o)(par);
                    co->push_back(o);
                }
                else if(s=="idv")
                {
                    Operation* o=new Operation("idv", &selections, _idv);
                    (*o)(par);
                    co->push_back(o);
                }
                else if(s=="div")
                {
                    Operation* o=new Operation("div", &selections, _div);
                    (*o)(par);
                    co->push_back(o);
                }
                else if(s=="pow")
                {
                    Operation* o=new Operation("pow", &selections, _pow);
                    (*o)(par);
                    co->push_back(o);
                }
                else if(s=="max")
                {
                    Operation* o=new Operation("max", &selections, _max);
                    (*o)(par);
                    co->push_back(o);
                }
                else if(s=="min")
                {
                    Operation* o=new Operation("min", &selections, _min);
                    (*o)(par);
                    co->push_back(o);
                }
                else
                {
                    std::cout<<"Unrecognized operation\n";
                    --i;
                }
            }
            else if(regex_match(s, match, op))
            {
                s=match[1].str();
                if(s=="log")
                {
                    Operation* o=new Operation("log", &selections, _log);
                    co->push_back(o);
                }
                else if(s=="abs")
                {
                    Operation* o=new Operation("abs", &selections, _abs);
                    co->push_back(o);
                }
                else if(s=="inv")
                {
                    Operation* o=new Operation("inv", &selections, _inv);
                    co->push_back(o);
                }
                else if(s=="grs")
                {
                    Operation* o=new Operation("grs", &selections, _grs);
                    co->push_back(o);
                }
                else if(s=="baw")
                {
                    Operation* o=new Operation("baw", &selections, _baw);
                    co->push_back(o);
                }
                else if(s=="med")
                {
                    Operation* o=new Operation("med", &selections, nullptr);
                    co->push_back(o);
                }
                else
                {
                    bool yes=true;
                    for(CompositeOperation* co1:composites)
                    {
                        if(co1->get_label()==s)
                        {
                            co->push_back(co1);
                            yes=false;
                            break;
                        }
                    }
                    if(yes)std::cout<<"Unrecognized operation\n", --i;
                }
            }
            else
            {
                std::cout<<"Unrecognized operation (forgot op?).\n";
                --i;
            }
        }
        apply(co);
        if(keep==" -k")composites.push_back(co);
        else delete co;
    }
    else if(regex_match(s, match, opp))
    {
        int par=atoi(match[2].str().c_str());
        int hx=strtol(match[2].str().c_str(), nullptr, 16);
        s=match[1].str();
        if(s=="add")
        {
            Operation o("add", &selections, _add);
            o(par);
            apply(&o);
        }
        else if(s=="sub")
        {
            Operation o("sub", &selections, _sub);
            o(par);
            apply(&o);
        }
        else if(s=="isb")
        {
            Operation o("isb", &selections, _isb);
            o(par);
            apply(&o);
        }
        else if(s=="mul")
        {
            Operation o("mul", &selections, _mul);
            o(par);
            apply(&o);
        }
        else if(s=="idv")
        {
            Operation o("idv", &selections, _idv);
            o(par);
            apply(&o);
        }
        else if(s=="div")
        {
            Operation o("div", &selections, _div);
            o(par);
            apply(&o);
        }
        else if(s=="pow")
        {
            Operation o("pow", &selections, _pow);
            o(par);
            apply(&o);
        }
        else if(s=="max")
        {
            Operation o("max", &selections, _max);
            o(par);
            apply(&o);
        }
        else if(s=="min")
        {
            Operation o("min", &selections, _min);
            o(par);
            apply(&o);
        }
        else if(s=="col")
        {
            Operation o("col", &selections, _col);
            o(hx);
            apply(&o);
        }
        else
        {
            std::cout<<"Unrecognized operation\n";
        }
    }
    else if(regex_match(s, match, op))
    {
        s=match[1].str();
        if(s=="log")
        {
            Operation o("log", &selections, _log);
            apply(&o);
        }
        else if(s=="abs")
        {
            Operation o("abs", &selections, _abs);
            apply(&o);
        }
        else if(s=="inv")
        {
            Operation o("inv", &selections, _inv);
            apply(&o);
        }
        else if(s=="grs")
        {
            Operation o("grs", &selections, _grs);
            apply(&o);
        }
        else if(s=="baw")
        {
            Operation o("baw", &selections, _baw);
            apply(&o);
        }
        else if(s=="med")
        {
            Operation o("med", &selections, nullptr);
            apply(&o);
        }
        else
        {
            bool yes=true;
            for(CompositeOperation* co:composites)
            {
                if(co->get_label()==s)
                {
                    apply(co);
                    yes=false;
                    break;
                }
            }
            if(yes)std::cout<<"Unrecognized operation\n";
        }
    }
    else if(regex_match(s, match, ope))
    {
        s=match[1].str();
        bool yes=true;
        for(CompositeOperation* co:composites)
        {
            if(co->get_label()==s)
            {
                Formater::export_FUN(co, match[2].str());
                yes=false;
                break;
            }
        }
        if(yes)std::cout<<"Unrecognized operation\n";
    }
    else if(regex_match(s, match, opi))
    {
        CompositeOperation* co=Formater::import_FUN(match[1].str(), &selections, &composites);
        if(co)
        {
            composites.push_back(co);
        }
    }
    else if(regex_match(s, match, sw))
    {
        int first=atoi(match[1].str().c_str());
        int second=atoi(match[2].str().c_str());
        first=layers.size()-first;
        second=layers.size()-second;
        if(first<0 || first>=layers.size() ||
           second<0 || second>=layers.size())
        {
            std::cout<<"Index out of bounds. Current layer count: "<<layers.size()<<"\n";
            return;
        }
        std::swap(layers[first], layers[second]);
    }
    reload();
}

void Window::handle_event(SDL_Event &e)
{
    if( e.type == SDL_WINDOWEVENT )
    {
        switch( e.window.event )
        {
        //Get new dimensions and repaint on window size change
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            width = e.window.data1;
            height = e.window.data2;
            refresh();
            break;

        //Repaint on exposure
        case SDL_WINDOWEVENT_EXPOSED:
            refresh();
            break;

        //Mouse entered window
        case SDL_WINDOWEVENT_ENTER:
            MouseFocus = true;
            break;

        //Mouse left window
        case SDL_WINDOWEVENT_LEAVE:
            MouseFocus = false;
            break;

        //Window has keyboard focus
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            KeyboardFocus = true;
            break;

        //Window lost keyboard focus
        case SDL_WINDOWEVENT_FOCUS_LOST:
            KeyboardFocus = false;
            break;

        //Window minimized
        case SDL_WINDOWEVENT_MINIMIZED:
            Minimized = true;
            break;

        //Window maximized
        case SDL_WINDOWEVENT_MAXIMIZED:
            Minimized = false;
            break;

        //Window restored
        case SDL_WINDOWEVENT_RESTORED:
            Minimized = false;
            break;
        }
    }
    else if ( e.type == SDL_MOUSEBUTTONUP)
    {
        int x, y;
        SDL_GetMouseState(&x, &y);

        if(selection && x>100 && x<rect.w+100 && y>0 && y<rect.h)
        {
            selection_rect.w=abs(selection_rect.x-x);
            selection_rect.h=abs(selection_rect.y-y);
            selection_rect.x=std::min(selection_rect.x, x);
            selection_rect.y=std::min(selection_rect.y, y);
            selection=false;

            selections.push_back(Selection(selection_rect));

            std::cout<<"Selection label: ";
            std::string s;
            std::getline(std::cin, s);
            selections[selections.size()-1].label=s;

            std::cout<<"Selection made.\n";

            refresh();
            saved=false;
        }
        else if(selection)
        {
            std::cout<<"Selection cancelled.\n";
            selection=!selection;
        }

        //Menu
        if(!selection && x>0 && x<100)
        {
            //New
            if(y>0 && y<30)
            {
                if(!saved)
                {
                    std::cout<<"Are you sure? All unsaved changes will be lost! [Y/n]: ";
                    char Y;
                    std::cin>>Y;
                    if(Y=='n')return;
                }
                saved=false;

                rect2.w=0;
                rect2.h=0;
                rect=rect2;
                delete_layers();
                selections.clear();
                clear();
                refresh();
            }
            //Open
            if(y>30 && y<60)
            {
                if(!saved)
                {
                    std::cout<<"Are you sure? All unsaved changes will be lost! [Y/n]: ";
                    char Y;
                    std::cin>>Y;
                    if(Y=='n')return;
                }
                saved=false;

                std::string s;
                std::cout<<"Path to file: ";
                std::getline(std::cin, s);
                rect2.w=0;
                rect2.h=0;
                rect=rect2;
                delete_layers();
                selections.clear();
                clear();
                Formater::import_XML(this, s, &layers, &selections, &composites);
                reload();
                refresh();
            }
            //Save
            if(y>60 && y<90)
            {
                std::string s;
                std::cout<<"Path to file: ";
                std::getline(std::cin, s);
                Formater::export_XML(this, s, &layers, &selections, &composites);
                saved=true;
            }
            //Import
            if(y>120 && y<150)
            {
                std::string s;
                std::cout<<"Path to file: ";
                std::getline(std::cin, s);
                add(s);
                reload();
                saved=false;
            }
            //Export
            if(y>150 && y<180)
            {
                std::string s;
                std::cout<<"Path to file: ";
                std::getline(std::cin, s);
                out(s);
            }
            //Quit
            if(y>210 && y<240)
            {
                if(!saved)
                {
                    std::cout<<"Are you sure? All unsaved changes will be lost! [Y/n]: ";
                    char Y;
                    std::cin>>Y;
                    if(Y=='n')return;
                }
                saved=false;

                n_quit=false;
            }
            //Command line input
            if(y>270 && y<300)
            {
                std::string s;
                std::cout<<"Input: ";
                std::getline(std::cin, s);
                handle_command(s);
                saved=false;
            }
        }

    }
    else if( e.type == SDL_MOUSEBUTTONDOWN)
    {
        int x, y;
        SDL_GetMouseState(&x, &y);

        //Selection
        if(!selection && x>100 && x<rect.w+100 && y>0 && y<rect.h)
        {
            selection=true;
            selection_rect.x=x;
            selection_rect.y=y;

            std::cout<<"Selection begin.\n";
        }
    }
}
