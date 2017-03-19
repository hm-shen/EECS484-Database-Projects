#include "bnode_leaf.h"

using namespace std;

Bnode_leaf::~Bnode_leaf() {
    // Remember to deallocate memory!!
    //bnode class has Bnode_inner* parent. do we need to deallocate that?
    // delete next;
    // delete prev;
    // delete [] values;
		next = NULL;
		prev = NULL;
		*values = NULL;
}

VALUETYPE Bnode_leaf::merge(Bnode_leaf* rhs) {
    assert(num_values + rhs->getNumValues() < BTREE_LEAF_SIZE);
    assert(rhs->num_values > 0);
    VALUETYPE retVal = rhs->get(0);

    Bnode_leaf* save = next;
    next = next->next;
    if (next) next->prev = this;

    for (int i = 0; i < rhs->getNumValues(); ++i)
        insert(rhs->getData(i));

    rhs->clear();
    return retVal;
}

VALUETYPE Bnode_leaf::redistribute(Bnode_leaf* rhs) {
    // TODO: Implement this
    assert( num_values < BTREE_LEAF_SIZE/2);
    assert( rhs->num_values > 0);
    assert( rhs->num_values >= BTREE_LEAF_SIZE/2);  //satisfy the condition of redistribution
    
    int total_num = num_values + rhs->num_values;
    int redis_num = total_num/2 - num_values;
    for(int i = 0 ; i< redis_num ; i++)
    {
        VALUETYPE borrowVal = rhs->get(i);
        insert(borrowVal);
        rhs->remove(borrowVal);
    }
    VALUETYPE retVal = rhs->get(0);
    return retVal;
}

Bnode_leaf* Bnode_leaf::split(VALUETYPE insert_value) {
    assert(num_values == BTREE_LEAF_SIZE);
    // TODO: Implement this

    Bnode_leaf* save = next;
    //new another node
    Bnode_leaf* split_node = new Bnode_leaf;

    //change pointers
    next = split_node;
    split_node->next = save;
    split_node->prev = this;
    if(save) save->prev = split_node;

    //split the node evenly
//     for(int i = 0; i <= (num_values/2-1); i++)
//         split_node->insert(get(i));
//     for(int i = num_values/2; i < num_values; i++)
//         remove(get(i));

     for(int i = 0; i <= (num_values/2-1); i++)
         split_node->insert(this->get(i+num_values/2));
     for(int i = num_values/2; i < num_values; i++)
         remove(get(i));

    //insert the new value
    if(insert_value > this->get((BTREE_LEAF_SIZE/2 - 1)))
    {split_node->insert(insert_value);}
    else
    {insert(insert_value);}
    return split_node;
}


