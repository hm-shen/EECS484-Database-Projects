#include "iostream"
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
    if (this->search(value) != nullptr) {return false;}

    Bnode *insertEntry = nullptr;
    VALUETYPE insertValue = 0;
    node_insert(&root, value, &insertEntry, insertValue);
    
		if ((insertEntry != nullptr) && (insertValue != 0))
	  { 
				// root need split
				// init split leaf --> inner
				Bnode_inner* new_root = new Bnode_inner;
				new_root->insert(root,0);
        root->parent = new_root;
				new_root->insert(insertValue);
				new_root->insert(insertEntry,1);
        insertEntry->parent = new_root;
				root = dynamic_cast<Bnode*>(new_root);
		}

    bool result = search(value);
	  if (result) 
		{
        this->size ++;
		}
    return result;
}

bool Btree::remove(VALUETYPE value) 
{
    // TODO: Implement this
    if (this->search(value) == nullptr) {return false;}
    Bnode *deleteEntry = nullptr;
    VALUETYPE deleteVal= 0;
    node_delete(&root, value, &deleteEntry, deleteVal);
    Bnode_inner* test = dynamic_cast<Bnode_inner*>(root);
    if ((deleteEntry == this->root) && (size = BTREE_LEAF_SIZE) && (test != nullptr))
    {
				Bnode_inner *root_copy = dynamic_cast<Bnode_inner*>(root);
        root_copy->remove_child(root_copy->find_value(deleteVal)+1);
        root_copy->remove_value(root_copy->find_value(deleteVal));
        Bnode_leaf *temp = dynamic_cast<Bnode_leaf*>(root_copy->getChild(0));
        cout<<temp->get(0)<<endl;
        root = temp;
        root_copy->clear();
        deleteEntry = nullptr;
        deleteVal = 0;

        bool result = search(value);
	      if (result == false) 
		    {
            this->size --;
		    }
        return result;
    }
    else if ((deleteEntry == this->root) && (size > BTREE_LEAF_SIZE))
    {
				Bnode_inner *root_copy = dynamic_cast<Bnode_inner*>(root);
        root_copy->remove_value(deleteVal);
        root_copy->remove_child(root_copy->find_value(deleteVal)+1);
        deleteEntry = nullptr;
        deleteVal = 0;

        bool result = search(value);
	      if (result == false) 
		    {
            this->size --;
		    }
        return result;
    }
    else if ((deleteEntry == this->root) && (size <= BTREE_LEAF_SIZE))
    {
				Bnode_leaf *root_copy = dynamic_cast<Bnode_leaf*>(root);
        root_copy->remove(deleteVal);
        bool result = search(value);
	      if (result == false) 
		    {
            this->size --;
		    }
        return result;
    }

    bool result = search(value);
	  if (result == false) 
		{
        this->size --;
		}
    return result;
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

void Btree::node_insert(Bnode** nodepointer,VALUETYPE value ,Bnode** childentry,VALUETYPE& parent_val)
{
    Bnode_inner* curnode = dynamic_cast<Bnode_inner*>(*nodepointer);
    
    //if is innernode
    if(curnode){
        int idx = curnode->find_value_gt(value);
				Bnode *temp = curnode->getChild(idx);
        node_insert(&temp,value, childentry, parent_val);
//        inner_node_insert(&(curnode->getChild(idx)),value, childentry, parent_val);
        if(*childentry == nullptr){return;}
        else{
            //if N has space
            if(curnode->getNumValues()<BTREE_FANOUT-1){
                int insertidx = curnode->find_idx(parent_val);
//                int insertidx = curnode->find_idx(value);
                (*childentry)->parent = curnode;
                curnode->insert(*childentry,insertidx+1);  //insert child tree into current root node 
                curnode->insert(parent_val);  //insert the value into the current root node
                parent_val = 0;
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
    }
}

void Btree::node_delete(Bnode** nodepointer, VALUETYPE value, Bnode** parentpointer, VALUETYPE& oldchildentry)
{
    Bnode_inner* curnode = dynamic_cast<Bnode_inner*>(*nodepointer);
    if (curnode)
    {
        // inner-node
        int idx = curnode->find_value_gt(value);
				Bnode *temp = curnode->getChild(idx);
        this->node_delete(&temp, value, parentpointer, oldchildentry);
        if ((*parentpointer == nullptr) && (oldchildentry == 0))
        {   
            // child not being deleted
            return; 
        } 
        else if (*parentpointer == this->root)
        {
            // root does not merge or split.
            return;
        }
        else if (*parentpointer != *nodepointer)
        {
            // its sibiling should be modified.
                        
        }
        else 
        {
            // child node are deleted
            int deleteidx = curnode->find_idx(oldchildentry);
            curnode->remove_child(deleteidx+1);  // delete corresponding child from tree
            curnode->remove_value(deleteidx);  // delete corresponding value from tree
            if (curnode->getNumValues() >= (BTREE_FANOUT - 1)/2)
            {
                // has entries to spare
                oldchildentry = 0;
                *parentpointer = nullptr;
                return;
            }
            else
            {
                // need redist or merge
                // get sibiling
                Bnode_inner* curParent = curnode->parent;
                int curnodeId = curParent->find_child(curnode);
                
//                if (curnodeId == curParent->getNumChildren()-1)
//                {
//                    // no sibiling on rhs --- choose left
//                    Bnode_inner* rhs = nullptr;
//                    Bnode_inner* lhs = dynamic_cast<Bnode_inner*>(curParent->getChild(curnodeId-1));
//                }
//                else if (curnodeId == 0)
//                {
//                    // no lhs --- 
//                    Bnode_inner* rhs = dynamic_cast<Bnode_inner*>(curParent->getChild(curnodeId+1));
//                    Bnode_inner* lhs = nullptr;
//                }
//                else 
//                {
//                    // both rhs and lhs exist
//                    Bnode_inner* rhs = dynamic_cast<Bnode_inner*>(curParent->getChild(curnodeId+1));
//                    Bnode_inner* lhs = dynamic_cast<Bnode_inner*>(curParent->getChild(curnodeId-1));
//                }
                

                Bnode_inner* rhs = dynamic_cast<Bnode_inner*>(curParent->getChild(curnodeId+1));
                Bnode_inner* lhs = dynamic_cast<Bnode_inner*>(curParent->getChild(curnodeId-1));
                if ((rhs != nullptr) && (rhs->getNumValues() > (BTREE_FANOUT - 1)/2))
                {   
                    // redis with right
                    VALUETYPE parVal = curnode->redistribute(rhs, curnodeId);
                    curParent->replace_value(parVal,curnodeId);
                    return;
                }
                else if ((lhs != nullptr) && (lhs->getNumValues() > (BTREE_FANOUT - 1)/2)) 
                {
                    // redis with left
                    VALUETYPE parVal = curnode->redistribute(rhs, curnodeId-1);
                    curParent->replace_value(parVal, curnodeId-1);
                    return;
                }
                else if (rhs != nullptr)
                {
                    // merge with right
                    VALUETYPE parRemove = curnode->merge(rhs, curnodeId);
//                    curParent->remove_value(parRemove);
//                    curParent->remove_child(curnodeId+1);
                    *parentpointer = curParent->getChild(curnodeId+1);
                    oldchildentry = curParent->get(parRemove);
                    return;
                }
                else if (lhs != nullptr)
                {
                    // merge with left
                    VALUETYPE parRemove = curnode->merge(lhs, curnodeId-1);
//                    curParent->remove_value(parRemove);
//                    curParent->remove_child(curnodeId);
                    *parentpointer = curParent->getChild(curnodeId);
                    oldchildentry = curParent->get(parRemove);
                    return;
                }
            } 
        }
    }
    else
    {
        // leaf-node
        Bnode_leaf* curnodelf = dynamic_cast<Bnode_leaf*>(*nodepointer);
        if (curnodelf->getNumValues() > BTREE_LEAF_SIZE/2)
        {
            // has entries to spare
            curnodelf->remove(value);  // delete corresponding value from tree
            oldchildentry = 0;
            *parentpointer = nullptr;
            return; 
        }
        else if (*nodepointer == root)
        {
            *parentpointer = root;
            oldchildentry = value;
            return;
        }
        else
        {
            // redis or merge
            Bnode_inner* curParent = curnodelf->parent;
            int curnodelfId = curParent->find_child(curnodelf);
            curnodelf->remove(value);  // delete corresponding value from tree
            if ((curnodelf->next != nullptr) && ((curnodelf->next)->getNumValues() >= BTREE_LEAF_SIZE/2 + 1) && ((curnodelf->next)->getNumValues() > 0))
            {
                // redis on right
                Bnode_leaf* rhs = curnodelf->next;
                VALUETYPE parIdx = curnodelf->redistribute(rhs);
                curParent->replace_value(parIdx, curnodelfId);
                *parentpointer = nullptr;
                oldchildentry = 0;
                return;
            }
            else if ((curnodelf->prev != nullptr) && ((curnodelf->prev)->getNumValues() >= BTREE_LEAF_SIZE/2 + 1))
            {
                // redis on left
                Bnode_leaf* lhs = curnodelf->prev;
                VALUETYPE parIdx = curnodelf->redistributeLeft(lhs);
                curParent->replace_value(parIdx, curnodelfId-1);
                *parentpointer = nullptr;
                oldchildentry = 0;
                return;
            }
            else if (curnodelf->next != nullptr)
            {
                // merge on right
                Bnode_leaf* rhs = curnodelf->next;
                VALUETYPE parRem = curnodelf->merge(rhs);
                Bnode_inner* parent = rhs->parent;
//                parRem = parent->get((parent->find_idx(parRem))-1);
                parRem = parent->get((parent->find_parIdx(parRem)));
                *parentpointer = parent;
                oldchildentry = parRem;
                return;
            }
            else if (curnodelf->prev != nullptr)
            {
                // merge on left
                Bnode_leaf* lhs = curnodelf->prev;
//                VALUETYPE parIdx = curnodelf->merge(lhs,curnodelfId-1);
                VALUETYPE parRem = curnodelf->mergeLeft(lhs);
                Bnode_inner *parent = curnodelf->parent;
                int lhsId = parent->find_child(curnodelf);
                parRem = parent->get(lhsId-1);
                *parentpointer = parent;
                oldchildentry = parRem;
//                parent->remove_value(parRem);
//                parent->remove_child(parent->find_child(curnodelf));
                return;
            }
        }
    }
}

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
