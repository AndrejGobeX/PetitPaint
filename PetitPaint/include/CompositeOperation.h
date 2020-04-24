#ifndef COMPOSITEOPERATION_H
#define COMPOSITEOPERATION_H

#include "Operation.h"
#include<list>


class CompositeOperation : public Operation
{
    public:
        CompositeOperation(std::string _label, std::vector<Selection>* _selections);
        virtual ~CompositeOperation();

        virtual void operator()(std::vector<Pixel>& pixels, int w)override;

        std::string get_all_ops();
        void push_back(Operation* o){operations.push_back(o);};
        void erase(std::string _label);

    protected:

    private:
        std::list<Operation*> operations;
};

#endif // COMPOSITEOPERATION_H
