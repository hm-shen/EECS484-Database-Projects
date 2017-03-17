#include "bnode_inner.h"
#include <vector>

using namespace std;

VALUETYPE Bnode_inner::merge(Bnode_inner* rhs, int parent_idx) {
    assert(rhs->parent == parent); // can only merge siblings
    assert(rhs->num_values > 0);

    // TODO: Implement this
    assert(num_values + rhs->getNumValues() <= BTREE_FANOUT - 1);
    VALUETYPE retVal = parent->get(parent_idx);

    insert(retVal);
    //merge the value
    for(int i = 0; i < rhs->getNumValues(); i++)
        insert(rhs->get(i)); 
    //merge the children 
    for(int i = 0 ; i < rhs->getNumChildren(); i++)
    {
        insert(rhs->getChild(i),num_children);
        rhs->getChild(i)->parent = this;  //还是用下面的？
        // children[num_children-1]->parent = this;
    }
    rhs->clear();
    return retVal;
}

VALUETYPE Bnode_inner::redistribute(Bnode_inner* rhs, int parent_idx) {
    assert(rhs->parent == parent); // inner node redistribution should only happen with siblings
    assert(parent_idx >= 0);
    assert(parent_idx < parent->getNumValues());

    // TODO: Implement this
    int total_num = num_values + rhs->num_values;
    int redis_num = total_num/2 - num_values;

    VALUETYPE parent_val = parent->get(parent_idx);
    insert(parent_val);

    for(int i = 0 ; i< redis_num-1 ; i++)
    {
        //change the value of current node and right node
        VALUETYPE borrowVal = rhs->get(i);
        insert(borrowVal);
        rhs->remove_value(borrowVal);

        //change the children of current node and right node
        insert(rhs->getChild(i),num_children);
        //rhs->getChild(i)->parent = this;    
        children[num_children-1]->parent = this; //能访问到么? 和上面那个用哪个?
        rhs->remove_child(i);
    }

    VALUETYPE retVal = rhs->get(0);
    rhs->remove_value(retVal);

    // change the count of the children num
    //num_children = num_children + (redis_num-1);
    //rhs->num_children = rhs->num_children - (redis_num-1);

    return retVal;
}


Bnode_inner* Bnode_inner::split(VALUETYPE& output_val, VALUETYPE insert_value, Bnode* insert_node) {
    assert(num_values == BTREE_FANOUT-1); // only split when it's full!

    // Populate an intermediate array with all the values/children before splitting - makes this simpler
    vector<VALUETYPE> all_values(values, values + num_values);
    vector<Bnode*> all_children(children, children + num_children);

    // Insert the value that created the split
    int ins_idx = find_value_gt(insert_value);
    all_values.insert(all_values.begin()+ins_idx, insert_value);
    all_children.insert(all_children.begin()+ins_idx+1, insert_node);

    // Do the actual split into another node
    Bnode_inner* split_node = new Bnode_inner;

    assert(all_values.size() == BTREE_FANOUT);
    assert(all_children.size() == BTREE_FANOUT+1);

    // Give the first BTREE_FANOUT/2 values to this bnode
    clear();
    for (int i = 0; i < BTREE_FANOUT/2; ++i)
        insert(all_values[i]);
    for (int i = 0, idx = 0; i < (BTREE_FANOUT/2) + 1; ++i, ++idx) {
        insert(all_children[i], idx);
        all_children[i] -> parent = this;
    }

    // Middle value should be pushed to parent
    output_val = all_values[BTREE_FANOUT/2];

    // Give the last BTREE/2 values to the new bnode
    for (int i = (BTREE_FANOUT/2) + 1; i < all_values.size(); ++i)
        split_node->insert(all_values[i]);
    for (int i = (BTREE_FANOUT/2) + 1, idx = 0; i < all_children.size(); ++i, ++idx) {
        split_node->insert(all_children[i], idx);
        all_children[i] -> parent = split_node;
    }

    // I like to do the asserts :)
    assert(num_values == BTREE_FANOUT/2);
    assert(num_children == num_values+1);
    assert(split_node->getNumValues() == BTREE_FANOUT/2);
    assert(split_node->getNumChildren() == num_values + 1);

    split_node->parent = parent; // they are siblings

    return split_node;
}