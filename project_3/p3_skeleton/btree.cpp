#include "btree.h"
#include "bnode.h"
#include "bnode_inner.h"
#include "bnode_leaf.h"

#include <cassert>

using namespace std;

const int LEAF_ORDER = BTREE_LEAF_SIZE/2;
const int INNER_ORDER = (BTREE_FANOUT-1)/2;

Btree::Btree() : root(new Bnode_leaf), size(0) {
    // Fill in here if needed
}

Btree::~Btree() {
    // Don't forget to deallocate memory
}

bool Btree::insert(VALUETYPE value) {
    // TODO: Implement this
    return true;
}

bool Btree::remove(VALUETYPE value) {
    // TODO: Implement this
    return true;
}

vector<Data*> Btree::search_range(VALUETYPE begin, VALUETYPE end) {
    std::vector<Data*> returnValues;
    // TODO: Implement this

    return returnValues;
}

//
// for help to implement
//
Bnode_leaf* find_leaf_node(Bnode, value){

}

void split_leaf_insert(Bnode,  value){

}

void inner_node_insert(Bnode* nodepointer,VALUETYPE value ,Bnode* childentry,VALUETYPE& parent_val){
    Bnode_inner* curnode = dynamic_cast<Bnode_inner*>(nodepointer);
    
    //if is innernode
    if(curnode){
        int idx = curnode->find_value_gt(value);
        inner_node_insert(&(curnode->getChild(idx)),value,&childentry,parent_val);
        if(childentry == nullptr){return null;}
        else{
            //if N has space
            if(curnode->getNumValues()<BTREE_FANOUT-1){
                int insertidx = curnode->find_idx(value);
                curnode->insert(childentry,insertidx);  //insert child tree into current root node 
                curnode->insert(parent_val);  //insert the value into the current root node
                childentry = nullptr;
                return null;
            }
            //if N does not have space
            else{         
                int insertval = parent_val;
                //insert child tree and value in split function. change the parent_val in split function.
                childentry = curnode->split(parent_val,insertval,childentry);  
            }

            if(curnode==root){
                Bnode* new_root = new Bnode_inner;
                new_root->insert(parent_val); //insert value after split into parent node
                new_root->insert(childentry); //insert right tree
                new_root->insert(curnode); //insert left tree
            }
        }
    }
    else{
        Bnode_leaf* curnodelf = dynamic_cast<Bnode_leaf*>(nodepointer);
        //if L has space
        if(curnodelf->getNumValues()<BTREE_LEAF_SIZE){
            curnodelf->insert(value);
            childentry = nullptr;
            parent_val = nullptr;
            return null;
        }
        else{
            childentry = curnodelf->split();
            parent_val = childentry->get(0);
            return null;
        }
    }
}

void inner_node_delete(Bnode_inner, value){

}

// gets the first ancestor where value is not
// less than all of the ancestors value
bnode_inner* get_nonfirst_ancestor(bonde_inner, value){

}

bnode_inner* get_common_ancestor(left, right){

}


//
// Given code
//
Data* Btree::search(VALUETYPE value) {
    assert(root);
    Bnode* current = root;

    // Have not reached a leaf node yet
    Bnode_inner* inner = dynamic_cast<Bnode_inner*>(current);
    // A dynamic cast <T> will return a nullptr if the given input is polymorphically a T
    //                    will return a upcasted pointer to a T* if given input is polymorphically a T
    while (inner) {
        int find_index = inner->find_value_gt(value);
        current = inner->getChild(find_index);
        inner = dynamic_cast<Bnode_inner*>(current);
    }

    // Found a leaf node
    Bnode_leaf* leaf = dynamic_cast<Bnode_leaf*>(current);
    assert(leaf);
    for (int i = 0; i < leaf->getNumValues(); ++i) {
        if (leaf->get(i) > value)    return nullptr; // passed the possible location
        if (leaf->get(i) == value)   return leaf->getData(i);
    }

    // reached past the possible values - not here
    return nullptr;
}

