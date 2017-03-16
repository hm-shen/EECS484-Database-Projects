//
// FILENAME: bnode.h
// PURPOSE:  Define abstract base class for Btree nodes
//

#ifndef BNODE_H
#define BNODE_H

#include <iosfwd>
#include <cassert>

class Bnode_inner;

//
// Base class - Bnode (pure abstract)
//
class Bnode {
public:
    Bnode() : parent(nullptr) {}
    virtual ~Bnode() =0;
    virtual void print(std::ostream& out) const =0;

    Bnode_inner* parent;
};

std::ostream& operator<< (std::ostream& out, const Bnode& node);

#endif
