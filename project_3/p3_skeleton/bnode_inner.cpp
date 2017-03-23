#include "bnode_inner.h"
#include <vector>

using namespace std;

//===================================================
// for help
int Bnode_inner::find_idx(VALUETYPE val){
    int retVal;
    for (retVal = 0; retVal < num_values; ++retVal)
//        if (values[retVal]->value > val) break; //MODIFIED
        if (values[retVal] > val) break;
    return retVal;
}

int Bnode_inner::find_parIdx(VALUETYPE val)
{
    int retVal;
    if (values[0] > val)
    {
        return 0;
    }
    for (retVal = 0; retVal < num_values; ++retVal)
        if (values[retVal] > val) break;
    return retVal-1;
}
//===================================================
//
VALUETYPE Bnode_inner::merge(Bnode_inner* rhs, int parent_idx) {
    assert(rhs->parent == parent); // can only merge siblings
    assert(rhs->num_values > 0);

    // TODO: Implement this
    assert(num_values + rhs->getNumValues() <= BTREE_FANOUT - 1);
    VALUETYPE retVal = (rhs->parent)->get(parent_idx);

    this->insert(retVal);
    //merge the value
    for(int i = 0; i < rhs->getNumValues(); i++)
        this->insert(rhs->get(i)); 
    //merge the children 
    for(int i = 0 ; i < rhs->getNumChildren(); i++)
    {
        this->insert(rhs->getChild(i),this->num_children);
        //rhs->getChild(i)->parent = this;  
        this->children[num_children-1]->parent = this;
    }
    rhs->clear();
    return retVal;
}

VALUETYPE Bnode_inner::mergeLeft(Bnode_inner* lhs, int parent_idx) {
    assert(lhs->parent == parent); // can only merge siblings
    assert(lhs->num_values > 0);

    // TODO: Implement this
    assert(num_values + lhs->getNumValues() <= BTREE_FANOUT - 1);
    VALUETYPE retVal = (this->parent)->get(parent_idx);

    lhs->insert(retVal);
    //merge the value
    for(int i = 0; i < this->getNumValues(); i++)
        lhs->insert(this->get(i)); 
    //merge the children 
    for(int i = 0 ; i < this->getNumChildren(); i++)
    {
        lhs->insert(this->getChild(i),lhs->num_children);
        //(this->getChild(i))->parent = lhs;  
        (lhs->children[lhs->num_children-1])->parent = lhs;
    }
    this->clear();
    return retVal;
}


VALUETYPE Bnode_inner::redistribute(Bnode_inner* rhs, int parent_idx) {
    assert(rhs->parent == parent); 
    assert(parent_idx >= 0);
    assert(parent_idx < parent->getNumValues());

    // TODO: Implement this
    int total_num = num_values + rhs->num_values;
    int redis_num = total_num/2 - this->num_values;

    VALUETYPE parent_val = parent->get(parent_idx);
    this->insert(parent_val);
    this->insert(rhs->getChild(0),this->num_children);
    this->children[num_children-1]->parent = this;
    rhs->remove_child(0);

    for(int i = 0 ; i< redis_num-1 ; i++)
    {
        //change the value of current node and right node
//        VALUETYPE borrowVal = rhs->get(i);
        VALUETYPE borrowVal = rhs->get(0);
        this->insert(borrowVal);
        rhs->remove_value(borrowVal);

        //change the children of current node and right node
//        this->insert(rhs->getChild(i),num_children);
        this->insert(rhs->getChild(0),num_children);
        //(rhs->getChild(i))->parent = this;    
        this->children[num_children-1]->parent = this; 
        rhs->remove_child(0);
//        rhs->remove_child(i);
    }

    VALUETYPE retVal = rhs->get(0);
    rhs->remove_value(0);
//    rhs->remove_child(0);
//    rhs->remove_value(rhs->find_value(retVal));
    return retVal;
}

VALUETYPE Bnode_inner::redistributeLeft(Bnode_inner* lhs, int parent_idx) 
{
    assert(lhs->parent == this->parent); 
    assert(parent_idx >= 0);
    assert(parent_idx < (this->parent)->getNumValues());

    // TODO: Implement this
    int total_num = num_values + lhs->num_values;
    int redis_num = total_num/2 - this->num_values + 1;  // right more than left

    VALUETYPE parent_val = parent->get(parent_idx);
    this->insert(parent_val);
//    this->insert(lhs->getChild(lhs->getNumChildren()-1),0); //?
//    (this->children[0])->parent = this;
//    lhs->remove_child(lhs->getNumChildren()-1);

    for(int i = 0 ; i < redis_num - 1 ; i++)
    {
        //change the value of current node and right node
        VALUETYPE borrowVal = lhs->get(lhs->getNumValues()-1);
        this->insert(borrowVal);
        lhs->remove_value(borrowVal);

        //change the children of current node and right node
        this->insert(lhs->getChild(lhs->getNumChildren()-1),0);
        // (this->getChild(i))->parent = lhs;    
        (this->children[0])->parent = this; 
        lhs->remove_child(lhs->getNumChildren()-1);
    }
    VALUETYPE retVal = this->get(0);
    this->remove_value(0);
//    this->remove_child(0);
    return retVal;
}

//    for(int i = 0; i < redis_num; ++i)
//    {
//        VALUETYPE parent_val = parent->get(parent_idx);
//        int parIdx = this->insert(parent_val);
//        this->insert(rhs->getChild(0),parIdx+1);
//        parent->remove_value(parent_val);
//        parent_val = rhs->get(0);
//        int parNewIdx = parent->insert(parent_val);
//        assert(parNewIdx == parent_idx);
//        // rhs rotate
//        rhs->remove_value(0);
//        rhs->remove_child(0);
//    }
//        
//    VALUETYPE retVal = parent_val;
//    VALUETYPE retVal = rhs->get(0);
//    rhs->remove_value(retVal);

    // change the count of the children num
    //num_children = num_children + (redis_num-1);
    //rhs->num_children = rhs->num_children - (redis_num-1);



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
