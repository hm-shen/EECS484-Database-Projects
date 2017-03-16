//
// FILENAME: bnode_leaf.h
// PURPOSE:  Defines class for an leaf node
// TODO:     Implement the split() and redistribute() function
//           merge() is given
//
// NOTES:
// Feel free to add more functions as you see fit
// DO NOT change the implementations we give you
// We WILL be assuming our GIVEN implementations wil stay the same when grading
//      -> DO NOT CHANGE (though you can add more)
// You CAN change the prototype of merge()/redistirubte()/split() if you think it is needed
// based on how you're implementing Btree::insert()/remove()
//

#ifndef BNODE_LEAF
#define BNODE_LEAF

#include "bnode.h"
#include "constants.h"

//
// Leaf Btree node
//
class Bnode_leaf : public Bnode {
public:
    Bnode_leaf() : Bnode(), num_values(0), next(nullptr), prev(nullptr) {}
    ~Bnode_leaf(); // **PRO TIP** you might want to re-connect the next/prev pointers in the destructor
                   // so you don't have to worry about them (except for assigning them the first time)

    //
    // ======================= To implement ===============================
    //                (feel free to change if you want)
    //

    // Merges this object with rhs
    // Inputs:  The other leaf node this node should be merged with
    // Output:  The value that should be removed from the parent node
    VALUETYPE merge(Bnode_leaf* rhs);

    // Redistribute this object with rhs
    // Inputs:  The other leaf node this node should be redistirubted with
    // Output:  The value that was (or should be) written to the parent node
    VALUETYPE redistribute(Bnode_leaf* rhs); // TODO: Implement this

    // Split this object and applcate another bnode_leaf
    // Inputs:  The value that should be inserted into this node that caused this split
    // Output:  A new heap-allocated leaf node that was created due to this split (ownership should be transferred)
    Bnode_leaf* split(VALUETYPE insert_value); // TODO: Implement this


    //
    // ====================== Given implementations ========================
    //                           (do not change)
    //

    // Inserts a value into the internal array
    // A new Data* will be heap allocated (that will be managed by this class)
    void insert(VALUETYPE value);
    // Inserts a Data* into the internal array (ownership will be transferred to this class)
    void insert(Data* value);
    // Removes the value from the internal array
    // returns true if value was removed, false if value was not in this node
    bool remove(VALUETYPE value);

    // Retrieves the value at a given index
    VALUETYPE get(int index) { assert(index < num_values && index >= 0); return values[index]->value; }
    // Retrives a Data* at the given index (ownership is not transferred)
    Data* getData(int index) { assert(index < num_values && index >= 0); return values[index]; }
    // Gets the number of values in the internal array
    int getNumValues() const { return num_values; }

    // Retrievies all the internal data (ownership is kept)
    //      -- Use this with caution
    Data** getAll() { return values; }
    // Removes all the internal data
    // Memory is not deallocated
    //      -- Use this with caution
    void clear() { num_values = 0; }


    Bnode_leaf*     next;       // pointer to next leaf node
    Bnode_leaf*     prev;       // pointer to previous leaf node

    void print(std::ostream& out) const;

private:
    // returns the first index such that values[index] > value
    int find_idx(VALUETYPE val);

    Data*       values[BTREE_LEAF_SIZE];    // array of "value" or data
    int         num_values;                 // number of valid values in the above array
                                            // -- ENSURE: Only the first num_values cells are valid

};



#endif
