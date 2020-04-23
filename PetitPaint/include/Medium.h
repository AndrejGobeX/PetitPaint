#ifndef MEDIUM_H
#define MEDIUM_H

#include "Operation.h"


class Medium : public Operation
{
    public:
        Medium(vpointer _fun, std::string _label);
        virtual ~Medium();

    protected:

    private:
};

extern Medium o_med;

#endif // MEDIUM_H
