#include "iostream"
#include "btree.h"
#include "bnode.h"
#include "bnode_inner.h"
#include "bnode_leaf.h"
#include <cassert>
#include "typeinfo"
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

    Bnode *insertEntry = nullptr;
    VALUETYPE insertValue = 0;
    inner_node_insert(&root, value, &insertEntry, insertValue);
    
		if ((insertEntry != nullptr) && (insertValue != 0))
	  { 
				// root need split
				// init split leaf --> inner
				Bnode_inner* new_root = new Bnode_inner;
				new_root->insert(root,0);
				new_root->insert(insertEntry,1);
				new_root->insert(insertValue);
				root = dynamic_cast<Bnode*>(new_root);
		}

    bool result = search(value);
	  if (result) 
		{
				cout<<"insertion complete"<<endl;
		}
    return result;
}

bool Btree::remove(VALUETYPE value) {
    // TODO: Implement this
    return true;
}

vector<Data*> Btree::search_range(VALUETYPE begin, VALUETYPE end) {
    std::vector<Data*> returnValues;
    // TODO: Implement this
    assert(root);
    Bnode_leaf* leaf;
    int *idx = NULL;
    leaf = this->search_larger(begin, idx);
    Data* temp_data = leaf->getData(*idx);

    while (temp_data->value <= end)
    {
        returnValues.push_back(temp_data);
        temp_data = this->getNext(leaf,idx);
    } 
    return returnValues;
}

//
// for help to implement
//
// Bnode_leaf* find_leaf_node(Bnode, value){

//}

// void split_leaf_insert(Bnode,  value){

//}

void Btree::inner_node_insert(Bnode** nodepointer,VALUETYPE value ,Bnode** childentry,VALUETYPE& parent_val)
{
    Bnode_inner* curnode = dynamic_cast<Bnode_inner*>(*nodepointer);
    
    //if is innernode
    if(curnode){
        int idx = curnode->find_value_gt(value);
				Bnode *temp = curnode->getChild(idx);
        inner_node_insert(&temp,value, childentry, parent_val);
//        inner_node_insert(&(curnode->getChild(idx)),value, childentry, parent_val);
        if(*childentry == nullptr){return;}
        else{
            //if N has space
            if(curnode->getNumValues()<BTREE_FANOUT-1){
                int insertidx = curnode->find_idx(parent_val);
//                int insertidx = curnode->find_idx(value);
                curnode->insert(*childentry,insertidx+1);  //insert child tree into current root node 
                curnode->insert(parent_val);  //insert the value into the current root node
                *childentry = nullptr;
                return;
            }
            //if N does not have space
            else{         
                int insertval = parent_val;
                //insert child tree and value in split function. change the parent_val in split function.
                *childentry = curnode->split(parent_val,insertval,*childentry);  
            }

//            if(curnode == this->root){
//                Bnode* new_root = new Bnode_inner;
//                new_root->insert(parent_val); //insert value after split into parent node
//                new_root->insert(childentry); //insert right tree
//                new_root->insert(curnode); //insert left tree
//            }
        }
    }
    else{
        Bnode_leaf* curnodelf = dynamic_cast<Bnode_leaf*>(*nodepointer);
        //if L has space
        if(curnodelf->getNumValues() < BTREE_LEAF_SIZE){
            curnodelf->insert(value);
            *childentry = nullptr;
//            parent_val = nullptr;
            parent_val = 0;
            return;
        }
        else 
				{
						// L is full 
            Bnode_leaf *tempEntry = curnodelf->split(value);
            parent_val = tempEntry->get(0);
						*childentry = dynamic_cast<Bnode*>(tempEntry);
            return;
        }
//				else
//				{
//						// L is full and L is also a root --> change root to innernode
//					  Bnode_inner* new_root = new Bnode_inner;
//            Bnode_leaf *tempEntry = curnodelf->split(value);
//						new_root->insert(curnodelf,0);
//						new_root->insert(tempEntry,1);
//						new_root->insert(tempEntry->get(0));
//            return;
//				}
    }
}

// void inner_node_delete(Bnode_inner, value){

//}

// gets the first ancestor where value is not
// less than all of the ancestors value
// bnode_inner* get_nonfirst_ancestor(bonde_inner, value){

//}

// bnode_inner* get_common_ancestor(left, right){

//}


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

Bnode_leaf* Btree::search_larger(VALUETYPE value, int* out_idx) {
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

    for (int i = 0; i < leaf->getNumValues(); ++i) 
    {
        if (leaf->get(i) >= value)   
        {
            *out_idx = i;
            return leaf;
        }
    }

    // reached past the possible values - not here
    cout<<"Cannot find it. Bug!"<<endl;
    return nullptr;
}

Data* Btree::getNext(Bnode_leaf* leaf, int* idx)
{
    
    assert((*idx >= 0) && (*idx < leaf->getNumValues()));
    if ((*idx == (leaf->getNumValues()-1)) && (leaf->next != NULL))
    { 
        leaf = leaf->next;
        *idx = 0;
        return leaf->getData(*idx);
    }
    else if (leaf->next != NULL)
    {
        *idx = *idx + 1;
        return leaf->getData(*idx);
    }
}
