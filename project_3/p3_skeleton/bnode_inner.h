//
// FILENAME: bnode_inner.h
// PURPOSE:  Define structures for an inner node
// TODO:     Implement the merge() and redistribute() function
//           split() is given
//
// NOTES:
// Feel free to add more functions as you see fit
// DO NOT change the implementations we give you
// We WILL be assuming our GIVEN implementations wil stay the same when grading
//      -> DO NOT CHANGE (though you can add more)
// You CAN change the prototype of merge()/redistirubte()/split() if you think it is needed
// based on how you're implementing Btree::insert()/remove()
//
//


#ifndef BNODE_INNER_H
#define BNODE_INNER_H

#include "constants.h"
#include "bnode.h"

#include <cassert>
#include <iosfwd>

//
// Inner Btree node
//
class Bnode_inner : public Bnode {
public:
    Bnode_inner() : Bnode(), num_values(0), num_children(0) {}


    //
    // ======================= To implement ===============================
    //                (feel free to change if you want)
    //

    // Merges this object with rhs
    // Input:   The other node that this object should be merged with
    //          The index in the parent node that contains the value that this merge operation should
    //          happen through
    // Output: The value that was (or should be) removed to the parent node
    VALUETYPE merge(Bnode_inner* rhs, int parent_idx); //TODO: Write this

    // Redistributes this object with rhs
    // Inputs:  The other node that this object should be redistributed with
    //          The index in the parent node that contains the value that this redistribution should
    //          happen through
    // Output:  The value that was/should be written to the parent node
    VALUETYPE redistribute(Bnode_inner* rhs, int parent_idx); // TODO: Write this


    // Splits this object with rhs
    // Input:   The input node/value that caused this split
    // Output:  A heap-allocated Bnode_inner* that was created due to the split
    //          output_val should be the value that needs to be inserted in the parent node
    Bnode_inner* split(VALUETYPE& output_val, VALUETYPE insert_value, Bnode* insert_node);

    ///====================================================================
    /// for help 
    ///
    // returns the first index such that values[index] > value 
    int find_idx(VALUETYPE val);

    //=====================================================================

    //
    // ====================== Given implementations ========================
    //                           (do not change)
    //

    // returns -1 if value is not found in this node
    int find_value(VALUETYPE value) const;
    // returns -1 if child is not found in this node
    int find_child(Bnode* child) const;
    // Returns the first index where values[output] > value
    // Returns num_values if value is greater than everything in this node
    int find_value_gt(VALUETYPE value) const;

    // Inserts value into the this node
    // Returns the index that it was inserted at
    int insert(VALUETYPE value);
    // Inserts child into the internal array at a given index
    // Returns the index it was inserted at (idx)
    int insert(Bnode* child, int idx);

    // Removes a value at a given index
    void remove_value(int idx);
    // Removes a child at a given index
    void remove_child(int idx);

    // Replaces a value in the inner array at a given index
    void replace_value(VALUETYPE value, int idx);

    // Retrieves internal child at a given index
    Bnode* getChild(int idx) const { assert(idx >= 0); assert(idx < num_children); return children[idx]; }
    // Retrives a value at a given index
    VALUETYPE get(int idx) const { assert(idx >= 0); assert(idx < num_values); return values[idx]; }
    
    // Returns the number of values in this node
    int getNumValues() const { return num_values; }
    // Returns the number of children in this node
    int getNumChildren() const { return num_children; }

    // Returns a pointer to all internal values
    //      -- Use this with caution
    VALUETYPE* getAllValues() { return values; }
    // Returns a pointer to all internal children (ownership is kept)
    //      -- Use this with caution
    Bnode** getAllChildren() { return children; }
    // Clear all internal value and chlidren.
    // Memroy is not deallocated
    //      -- Use this with caution
    void clear() { num_values = 0; num_children = 0; }


    void print(std::ostream& out) const;

private:
    VALUETYPE   values[BTREE_FANOUT-1]; // Array of search keys
    int         num_values;             // Number of valid entires in the above array
                                        // -- ENSURE only the first num_values cells are valid
    Bnode*      children[BTREE_FANOUT]; // Array of pointers to other nodes
    int         num_children;           // Number of valid children in the above array
                                        // -- ENSURE only the first num_values cells are valid

};


#endif
