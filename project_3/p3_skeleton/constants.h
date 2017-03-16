//
// FILENAME: constants.h
// PURPOSE:  Define global definitions that are needed in all modules
//
// NOTES:
// DO NOT change this file
// We will test your submission with OUR constants.h (not yours)
//

#ifndef CONSTANTS_H
#define CONSTANTS_H

// Typedefs
typedef int VALUETYPE;  // to reflect the type of data we want in our datastructure
                        // For the sake of the project, we're going to use this typedef rather than a template (for simplicity)

// For the project we will not be doing anything with records themselves
// This is just to show where records should actually exist in this Btree
struct Record {};

// Data object that are contained by leaf nodes
struct Data {
    Data(VALUETYPE value_) : value(value_), record(nullptr) {}

    // Overloaded comparison
    bool operator<(const Data& rhs) { return value < rhs.value; }
    bool operator>(const Data& rhs) { return value > rhs.value; }
    bool operator==(const Data& rhs) { return value == rhs.value; }

    // Internal values
    // This should make it obvious that we are using alternative 2
    VALUETYPE value;// The value of this piece of data
    Record* record; // Pointer to the actual piece of memory that holds this record
                    // Although this is unsued in this project, you should know that this exists here for alternate 2
                    // Think about what else might be here if we used the other alternates?

};

// ARRAY CONSTANTS
const int BTREE_FANOUT      = 3;    // Fanout of each node in our Btree
                                    // Guarentees: This will never be < 3
                                    //             Will always be odd
const int BTREE_LEAF_SIZE   = 2;    // Size of leaf nodes (how many data entries each leaf can hold)
                                    // Guarentees: This will never be < 2
                                    //             Will always be even

#endif
