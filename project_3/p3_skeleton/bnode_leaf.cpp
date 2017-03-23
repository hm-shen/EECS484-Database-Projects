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
//    Bnode_inner* rhsParent = rhs->parent;
//    int rhsId = rhsParent->find_child(rhs)
//    VALUETYPE retVal = rhsParent->get(rhsId-1);
    VALUETYPE retVal = rhs->get(0);
    
    Bnode_leaf* save = next;
    next = next->next;
    if (next) next->prev = this;

    for (int i = 0; i < rhs->getNumValues(); ++i)
        this->insert(rhs->getData(i));

    rhs->clear();
    return retVal;
}

VALUETYPE Bnode_leaf::mergeLeft(Bnode_leaf* lhs) {
    assert(num_values + lhs->getNumValues() < BTREE_LEAF_SIZE);
    assert(lhs->num_values > 0);
    VALUETYPE retVal = 0;
//    VALUETYPE retVal = this->get(0);
    
    Bnode_leaf* save = next;
    lhs->next = save;
    if (save) save->prev = lhs;
//    next = next->next;
//    if (next) next->prev = this;

    for (int i = 0; i < this->getNumValues(); ++i)
        lhs->insert(this->getData(i));

    this->clear();
    return retVal;
}

VALUETYPE Bnode_leaf::redistribute(Bnode_leaf* rhs) {
    // TODO: Implement this
    assert( num_values < BTREE_LEAF_SIZE/2);
    assert( rhs->num_values > 0);
    assert( rhs->num_values > BTREE_LEAF_SIZE/2);  //satisfy the condition of redistribution
    
    int total_num = num_values + rhs->num_values;
    int redis_num = total_num/2 - this->num_values;
    int i = 0;
    while (i != redis_num)
    {
        VALUETYPE borrowVal = rhs->get(0);
        this->insert(borrowVal);
        rhs->remove(borrowVal);
        i++;
    }
    VALUETYPE retVal = rhs->get(0);
    return retVal;
}

VALUETYPE Bnode_leaf::redistributeLeft(Bnode_leaf* lhs) {
    // TODO: Implement this
    assert( num_values < BTREE_LEAF_SIZE/2);
    assert( lhs->num_values > 0);
    assert( lhs->num_values > BTREE_LEAF_SIZE/2);  //satisfy the condition of redistribution
    
    int total_num = num_values + lhs->num_values;
    int redis_num = total_num/2 - this->num_values;
    for(int i = 0 ; i< redis_num ; i++)
    {
        VALUETYPE borrowVal = lhs->get(lhs->num_values-1);   //-i?
        this->insert(borrowVal);
        lhs->remove(borrowVal);
    }
    VALUETYPE retVal = this->get(0);
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
     for(int i = 0; i <= (BTREE_LEAF_SIZE/2-1); i++)
         split_node->insert(this->get(i+BTREE_LEAF_SIZE/2));
     for(int i = BTREE_LEAF_SIZE/2; i < BTREE_LEAF_SIZE; i++)
         this->remove(this->get(num_values-1));

    //insert the new value
      if(insert_value < split_node->get(0))
      {
          this->insert(insert_value);
      }
      else
      {
          split_node->insert(insert_value);
      }

    return split_node;
}


