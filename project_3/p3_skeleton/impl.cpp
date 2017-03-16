//
// FILENAME: Impl.cpp
// PURPOSE:  Define the given implementations we give you
//           Feel free to read this file if you want
//
// NOTES:
// DO NOT change this file
// NOTE: We will test your submission with OUR impl.cpp (not yours)
//       Pro tip: You CAN modify the print() functions to print more things (if you want)
//

#include "bnode_inner.h"
#include "bnode_leaf.h"
#include "btree.h"

#include <iostream>
#include <vector>
#include <queue>

using namespace std;

//
// =====================================================================
//                  PRINITNG FUNCTIONS (GIVEN)
// =====================================================================
//

void Bnode_leaf::print(std::ostream& out) const {
    // You may or may not want to print next/prev/parent pointer here
    out << "{ ";
    for (int i = 0; i < num_values; ++i)
        out << values[i]->value << " ";
    out << "} ";
}

void Bnode_inner::print(std::ostream& out) const {
    // You may or may not want to print parent pointers here
    out << " [ ";
    for (int i = 0; i < num_values; ++i)
        out << values[i] << " ";
    out << "]";
}

ostream& operator<< (ostream& os, const Btree& tree) {
    os << "@@@ Btree <" << endl;

    // Level-order printing
    queue<pair<Bnode*, int>> q;
    Bnode* current = tree.root;
    Bnode_inner* inner;
    int depth = 0;
    q.push(make_pair(current,0));
    while (!q.empty()) {
        current = q.front().first;
        depth = q.front().second;
        q.pop();
        if ((inner = dynamic_cast<Bnode_inner*> (current)) && inner->getNumChildren() != 0)
            for (int i = 0; i < inner->getNumChildren(); ++i)
                q.push(make_pair(inner->getChild(i), depth+1));

        os << "@@@ ";
        for (int i = 0; i < depth; ++i) os << '\t';
        os << *current << endl;
    }
    os << "@@@ >";
    return os;
}

//
// =====================================================================
//                  BTREE IMPLEMENTATION (GIVEN)
// =====================================================================
//
void inorder_traverse(Bnode* current, vector<VALUETYPE>& values) {
    Bnode_inner* inner = dynamic_cast<Bnode_inner*>(current);
    if (inner) {
        assert(inner->getNumChildren() != 0);
        assert(inner->getNumValues() == inner->getNumChildren()-1);
        inorder_traverse(inner->getChild(0), values);
        for (int i = 0; i < inner->getNumValues(); ++i) {
            values.push_back(inner->get(i));
            inorder_traverse(inner->getChild(i+1), values);
        }
    }
    else {
        // not a inner? must be a leaf
        Bnode_leaf* leaf = dynamic_cast<Bnode_leaf*>(current);
        assert(leaf);
        for (int i = 0; i < leaf->getNumValues(); ++i) {
            values.push_back(leaf->get(i));
        }
    }
}

bool Btree::isValid() {
    vector<VALUETYPE> values;
    inorder_traverse(root, values);
    if (values.empty()) return true;
    VALUETYPE prev_value = values[0];
    for (int i = 1; i < values.size(); ++i) {
        if (prev_value > values[i]) return false;
        prev_value = values[i];
    }
    return true;
}


//
// =====================================================================
//                  INNER NODE IMPLEMENTATION (GIVEN)
// =====================================================================
//

int Bnode_inner::find_value(VALUETYPE value) const {
    for (int i = 0; i < num_values; ++i)
        if (values[i] == value) return i;
    return -1;
}
int Bnode_inner::find_child(Bnode* value) const {
    for (int i = 0; i < num_children; ++i)
        if (children[i] == value) return i;
    return -1;
}

int Bnode_inner::find_value_gt(VALUETYPE value) const {
    for (int i = 0; i < num_values; ++i)
        if (values[i] > value) return i;
    return num_values;
}


int Bnode_inner::insert(VALUETYPE value) {
    assert(num_values < BTREE_FANOUT-1); // node is full
    int idx = find_value_gt(value);
    assert(idx <= num_values);
    for (int i = num_values; i > idx; --i)
        values[i] = values[i-1];
    values[idx] = value;
    num_values++;
    return idx;
}

int Bnode_inner::insert(Bnode* child, int idx) {
    assert(num_children < BTREE_FANOUT); // node is full
    for (int i = num_children; i > idx; --i)
        children[i] = children[i-1];
    children[idx] = child;
    num_children++;
    return idx;
}

void Bnode_inner::remove_value(int idx) {
    assert(idx >= 0);
    assert(idx < num_values);
    for (int i = idx; i < num_values-1; ++i)
        values[i] = values[i+1];
    num_values--;
}

void Bnode_inner::remove_child(int idx) {
    assert(idx >= 0);
    assert(idx < num_children);
    for (int i = idx; i < num_children-1; ++i)
        children[i] = children[i+1];
    num_children--;
}

void Bnode_inner::replace_value(VALUETYPE value, int idx) {
    assert(idx >= 0);
    assert(idx < num_values);
    values[idx] = value;
}



//
// =====================================================================
//                  LEAF NODE IMPLEMENTATION (GIVEN)
// =====================================================================
//

int Bnode_leaf::find_idx(VALUETYPE val) {
    int retVal;
    for (retVal = 0; retVal < num_values; ++retVal)
        if (values[retVal]->value > val) break;
    return retVal;
}

void Bnode_leaf::insert(VALUETYPE val) {
    assert(num_values < BTREE_LEAF_SIZE);
    // must insert in order
    int insert_idx = find_idx(val);
    for (int i = num_values; i > insert_idx; --i)
        values[i] = values[i-1];
    num_values++;
    values[insert_idx] = new Data(val);
}

void Bnode_leaf::insert(Data* val) {
    assert(num_values < BTREE_LEAF_SIZE);
    // must insert in order
    int insert_idx = find_idx(val->value);
    for (int i = num_values; i > insert_idx; --i)
        values[i] = values[i-1];
    num_values++;
    values[insert_idx] = val;
}

bool Bnode_leaf::remove(VALUETYPE value) {
    int found_idx = -1;
    for (int i = 0; i < num_values; ++i) {
        if (values[i]->value == value) {
            found_idx = i;
            break;
        }
    }
    if (found_idx == -1) return false;
    delete values[found_idx];
    num_values--;
    for (int i = found_idx; i < num_values; ++i)
        values[i] = values[i+1];
    return true;
}
