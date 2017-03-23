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
    if ((deleteEntry == this->root) && (size == BTREE_LEAF_SIZE) && (test != nullptr))
    {
				Bnode_inner *root_copy = dynamic_cast<Bnode_inner*>(root);
//        root_copy->remove_child(root_copy->find_value(deleteVal)+1);
        root_copy->remove_value(root_copy->find_value(deleteVal));
        Bnode_leaf *temp = dynamic_cast<Bnode_leaf*>(root_copy->getChild(0));
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
//        root_copy->remove_child(root_copy->find_value(deleteVal)+1);
        root_copy->remove_value(root_copy->find_value(deleteVal));
        if (root_copy->getNumValues() == 0)
        {
            root = root_copy->getChild(0);
            root_copy->remove_child(0);
            root->parent = nullptr;
        }
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
    Bnode_leaf *leaf;
    int *idx = 0;
    leaf = this->search_larger(begin, idx);
    Data* temp_data = leaf->getData(*idx);

    while ((temp_data->value <= end) && (temp_data != nullptr))
    {
        returnValues.push_back(temp_data);
        temp_data = this->getNext(&leaf,idx);
    } 
//    this->search(begin);
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
        }
    }
    else{
        Bnode_leaf* curnodelf = dynamic_cast<Bnode_leaf*>(*nodepointer);
        //if L has space
        if(curnodelf->getNumValues() < BTREE_LEAF_SIZE){
            curnodelf->insert(value);
            *childentry = nullptr;
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
        else 
        {
            // child node are deleted
            curnode = dynamic_cast<Bnode_inner*>(*parentpointer);
//            int deleteidx = curnode->find_idx(oldchildentry);
            int deleteidx = curnode->find_value(oldchildentry);
//            curnode->remove_child(deleteidx+1);  // delete corresponding child from tree
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

                Bnode_inner* rhs;
                Bnode_inner* lhs;
               
                if (curnodeId == curParent->getNumChildren()-1)
                {
                    // no sibiling on rhs --- choose left
                    rhs = nullptr;
                    lhs = dynamic_cast<Bnode_inner*>(curParent->getChild(curnodeId-1));
                }
                else if (curnodeId == 0)
                {
                    // no lhs --- 
                    rhs = dynamic_cast<Bnode_inner*>(curParent->getChild(curnodeId+1));
                    lhs = nullptr;
                }
                else 
                {
                    // both rhs and lhs exist
                    rhs = dynamic_cast<Bnode_inner*>(curParent->getChild(curnodeId+1));
                    lhs = dynamic_cast<Bnode_inner*>(curParent->getChild(curnodeId-1));
                }
                
                if ((rhs != nullptr) && (rhs->getNumValues() > (BTREE_FANOUT - 1)/2))
                {   
                    // redis with right
                    VALUETYPE parVal = curnode->redistribute(rhs, curnodeId);
                    curParent->replace_value(parVal,curnodeId);
                    *parentpointer = nullptr;
                    oldchildentry = 0;
                    return;
                }
                else if ((lhs != nullptr) && (lhs->getNumValues() > (BTREE_FANOUT - 1)/2)) 
                {
                    // redis with left
                    VALUETYPE parVal = curnode->redistributeLeft(lhs, curnodeId-1);
                    curParent->replace_value(parVal, curnodeId-1);
                    *parentpointer = nullptr;
                    oldchildentry = 0;
                    return;
                }
                else if (rhs != nullptr)
                {
                    // merge with right
                    Bnode_inner* parent = rhs->parent;
                    VALUETYPE parRemove = curnode->merge(rhs, curnodeId);
                    int rhsId = parent->find_child(rhs);
                    parRemove = parent->get(rhsId-1);
						        parent->remove_child(parent->find_child(rhs));
                    *parentpointer = parent;
                    oldchildentry = parRemove;
                    return;
                }
                else if (lhs != nullptr)
                {
                    // merge with left
                    Bnode_inner* parent = curnode->parent;
                    VALUETYPE parRemove = curnode->mergeLeft(lhs, curnodeId-1);
                    int lhsId = parent->find_child(curnode);
                    parRemove = parent->get(lhsId-1);
						        parent->remove_child(parent->find_child(curnode));
                    *parentpointer = parent;
                    oldchildentry = parRemove;
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
            VALUETYPE curMax = curnodelf->get(curnodelf->getNumValues()-1);
            VALUETYPE curMin = curnodelf->get(0);
            curnodelf->remove(value);  // delete corresponding value from tree
            if ((curnodelf->next != nullptr) && ((curnodelf->next)->getNumValues() >= BTREE_LEAF_SIZE/2 + 1) && ((curnodelf->next)->getNumValues() > 0))
            {
                // redis on right
                Bnode_leaf* rhs = curnodelf->next;
                // common ances?
                if (rhs->parent != curnodelf->parent)
                {
                    // non-common ancestor
                    Bnode_inner *comAn = get_common_ancestor(curnodelf->parent, rhs->parent);
                    int ancId = common_ancestor_id(comAn, curMax, rhs->get(0)); 
                    VALUETYPE parIdx = curnodelf->redistribute(rhs);
                    comAn->replace_value(rhs->get(0), ancId);
                    *parentpointer = nullptr;
                    oldchildentry = 0;
                    return;
                }
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
                // common ances?
                if (lhs->parent != curnodelf->parent)
                {
                    // non-common ancestor
                    Bnode_inner *comAn = get_common_ancestor(lhs->parent,curnodelf->parent);
                    int ancId = common_ancestor_id(comAn, lhs->get(lhs->getNumValues()-1),curMin); 
                    VALUETYPE parIdx = curnodelf->redistributeLeft(lhs);
                    comAn->replace_value(curnodelf->get(0), ancId);
                    *parentpointer = nullptr;
                    oldchildentry = 0;
                    return;
                }
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
                // common ances?
                if (rhs->parent != curnodelf->parent)
                {
                    // non-common ancestor
                    Bnode_inner *comAn = get_common_ancestor(curnodelf->parent, rhs->parent);
                    int ancId = common_ancestor_id(comAn, curMax, rhs->get(0)); 
                    comAn->replace_value((rhs->next)->get(0), ancId);
                }
                VALUETYPE parRem = curnodelf->merge(rhs);
                Bnode_inner* parent = rhs->parent;
                parRem = parent->get((parent->find_parIdx(parRem)));  /////BUGBUGWARNING
						    parent->remove_child(parent->find_child(rhs));
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
						    parent->remove_child(parent->find_child(curnodelf));
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
//    cout<<"Cannot find it. Bug!"<<endl;
    return nullptr;
}

Data* Btree::getNext(Bnode_leaf** leaf, int* idx)
{
    
    assert((*idx >= 0) && (*idx < (*leaf)->getNumValues()));
    if ((*idx == ((*leaf)->getNumValues()-1)) && ((*leaf)->next != NULL))
    { 
        *leaf = (*leaf)->next;
        *idx = 0;
        return (*leaf)->getData(*idx);
    }
    else if (*idx < ((*leaf)->getNumValues()-1))  //if //(leaf->next != NULL)
    {
        *idx = *idx + 1;
        return (*leaf)->getData(*idx);
    }
    return nullptr;
}


Bnode_inner* Btree::get_common_ancestor(Bnode_inner* left, Bnode_inner* right)
{
		assert(left != right);
		Bnode_inner* leftp = left;
		Bnode_inner* rightp = right;
		while(leftp != rightp)
		{		
				leftp = leftp->parent;
				rightp = rightp->parent;
		}
		return leftp;
}

int Btree::common_ancestor_id(Bnode_inner* comAn, VALUETYPE leftVal, VALUETYPE rightVal)
{
		int common_ancestor_id = comAn->find_idx(leftVal);
		assert(comAn->get(common_ancestor_id) <= rightVal);
		return common_ancestor_id;	
}


