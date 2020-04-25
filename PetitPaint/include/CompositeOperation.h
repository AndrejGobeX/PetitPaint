#ifndef COMPOSITEOPERATION_H
#define COMPOSITEOPERATION_H

#include "Operation.h"
#include<list>


class CompositeOperation : public Operation
{
    friend class Formater;
    friend class Window;
    public:
        CompositeOperation(std::string _label, std::vector<Selection>* _selections);
        virtual ~CompositeOperation();

        virtual void operator()(std::vector<Pixel>& pixels, int w)override;

        virtual std::string get_all_ops()override;
        void push_back(Operation* o){operations.push_back(o);};
        void erase(std::string _label);

        virtual int get_class()override{return 1;}

    protected:

    private:
        std::list<Operation*> operations;
};

#endif // COMPOSITEOPERATION_H
