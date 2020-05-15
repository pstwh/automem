#ifndef VIEW_H
#define VIEW_H

#include <LinkedList.h>

#include "symbols.h"
#include "math.h"

class View {
    public:
        virtual ~View() {}

        virtual void setup();
        virtual void draw();
        virtual void update();
};

#endif