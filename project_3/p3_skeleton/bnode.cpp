#include "bnode.h"
#include <iostream>

Bnode::~Bnode() {}

std::ostream& operator<< (std::ostream& out, const Bnode& node) {
    node.print(out);
    return out;
}

