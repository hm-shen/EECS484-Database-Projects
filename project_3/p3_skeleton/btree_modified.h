//
// FILENAME: btree.h
// PURPOSE:  A B+ tree implementation
// TODO:     Write the insert(), remove() and search_range() function as described in the specs
//           The search() function has been given to you to show you how to use the skeleton code
//
// NOTES:
// Do not change the function prototypes we have given you. We will assume that these exact
// prototypes when we grade your submission
// Feel free to add more functions if you need
//

#ifndef BTREE_H
#define BTREE_H

#include "constants.h"

#include <vector>
#include <iosfwd>

class Record;
class Bnode_inner;
class Bnode_leaf;
class Bnode;

class Btree {
public:
    Btree();
    ~Btree();

    //
    // ======================= To implement ===============================
    //                      (Do NOT change these)
    //

    // Inserts a value into the existing Btree
    // Input:   Value to be inserted
    // Output:  true if value was inserted successfully
    //          false if the value already exists
    bool insert(VALUETYPE value);   // To implement

    // Removes a value from the existing Btree
    // Input:   Value to be inserted
    // Output:  true if value was succesfully removed
    //          false if value was not found
    bool remove(VALUETYPE value);   // To implement

    // Does a range-based search for data such that begin <= data <= end
    // Input:   begin and end values for range-based search
    // Output:  a vector containing Data* that satisfy begin <= values <= end
    //          return an empty vector if none satfisfy this condition
    //          Ownership is not transferred
    std::vector<Data*> search_range(VALUETYPE begin, VALUETYPE end);  // To implement

    //
    // ====================== Given implementations ========================
    //                        (Do NOT change these)
    //

    // Finds the Data* that has the given value
    // Input:   The value that we are searching for
    // Output:  The Data pointer that is associated with this value
    //          Returns nullptr if value is not found
    //          Ownership is not transferred
    Data* search(VALUETYPE value);  // implemented for you

    // Returns the size of the current Btree
    // Output:  The current size of this Btree
    int getSize() { return size; }

    // Checks if this Btree is valid
    // This DOES NOT check if your btree is correct. It only checks that you've maintained ordering
    // Output:  true if values are in order
    //          false if values are not
    //          This may assert();
    bool isValid();


    friend std::ostream& operator<< (std::ostream& os, const Btree& tree);

private:
    Bnode* root;
    int size;
    Bnode_leaf* search_larger(VALUETYPE value);
    Data* getNext(Bnode_leaf* leaf, int* idx);

    /*
    Some suggestions of member functions you may or may not might want to create

    Bnode_leaf find_leaf_node(Bnode, value)
    void split_leaf_insert(Bnode,  value)
    void inner_node_insert(Bnode_inner, value, child)
    void inner_node_delete(Bnode_inner, value)
    bnode_inner get_nonfirst_ancestor(bonde_inner, value) // gets the first ancestor where value is not
                                                          // less than all of the ancestors value
    bnode_inner get_common_ancestor(left, right)

    */
};


// Overloaded << operator for better debugging. You're welcome :)
std::ostream& operator<< (std::ostream& os, const Btree& tree);


#endif
