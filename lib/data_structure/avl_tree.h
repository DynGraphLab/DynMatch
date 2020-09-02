/******************************************************************************
 * avl_tree.h
 *
 *
 *****************************************************************************/

#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <bitset>
#include <cassert>
#include <iostream>

#include "../definitions.h"

template <class T>
class avl_tree {
        private:
                struct Node {
                        T id;
                        Node* left;
                        Node* right;
                        size_t height;

                        Node (T id);
                        Node (const Node&);

                        ~Node ();
                };

        public:
                avl_tree ();
                ~avl_tree ();

                bool insert (T id);
                bool remove (T id);

                T * get (T id);
                T * getMin ();
                T * getMax ();

                bool isIn (T id);

                size_t size();
                void print ();

                std::vector<T> allElements ();

        private:
                // members
                Node * root;
                size_t count;

                // private member functions
                size_t height (Node *);
                size_t max (size_t, size_t);
                Node * leftRotate (Node *);
                Node * rightRotate (Node *);
                int getBalance(Node *);

                Node * insert (T, Node *);
                Node * remove (T, Node *);
                Node * get (T, Node *);

                Node * getMin (Node *);
                Node * getMax (Node *);

                std::vector<T>& fill (Node *, std::vector<T>&);

                void print (Node *, size_t = 0);
};

template <class T>
avl_tree<T>::avl_tree () {
        root = nullptr;
        count = 0;
}

template <class T>
avl_tree<T>::~avl_tree () {
        if (root) {
                delete root;
        }
}

template <class T>
bool avl_tree<T>::insert (T id) {
        size_t tmp = count;
        root = insert(id, root);
        return (tmp+1 == count);
}

template <class T>
bool avl_tree<T>::remove (T id) {
        size_t tmp = count;
        root = remove(id, root);
        return (tmp-1 == count);
}

template <class T>
T * avl_tree<T>::get (T id) {
        return &(get(id, root)->id);
}

template <class T>
T * avl_tree<T>::getMin () {
        return &(getMin (root)->id);
}

template <class T>
T * avl_tree<T>::getMax () {
        return &(getMax (root)->id);
}

template <class T>
bool avl_tree<T>::isIn (T id) {
        T * result = get(id);
        return result;
}

template <class T>
size_t avl_tree<T>::size() {
        return count;
}

template <class T>
void avl_tree<T>::print () { // left=-1, both=0, right=1
        print (root);
}

template <class T>
std::vector<T> avl_tree<T>::allElements () {
        std::vector<T> content;
        content = fill(root, content);
        return content;
}

template <class T>
avl_tree<T>::Node::Node (T id) {
        this->id = id;
        left = nullptr;
        right = nullptr;
        height = 1;
}

template <class T>
avl_tree<T>::Node::Node (const Node& node) {
        id = node.id;
        left = node.left;
        right = node.right;
        height = node.height;
}

template <class T>
avl_tree<T>::Node::~Node () {
        if (left) {
                delete left;
        }

        if (right) {
                delete right;
        }
}

template <class T>
size_t avl_tree<T>::height (avl_tree<T>::Node * node) {
        if (!node) {
                return 0;
        }

        return node->height;
}

template <class T>
size_t avl_tree<T>::max (size_t a, size_t b) {
        return (a > b) ? a : b;
}

template <class T>
typename avl_tree<T>::Node * avl_tree<T>::leftRotate (Node * x) {
        Node * y = x->right;
        Node * a = y->left;

        x->right = a;
        y->left = x;

        x->height = max(height(x->left), height(x->right)) + 1;
        y->height = max(height(y->left), height(y->right)) + 1;

        return y;
}

template <class T>
typename avl_tree<T>::Node * avl_tree<T>::rightRotate (Node * x) {
        Node * y = x->left;
        Node * a = y->right;

        x->left = a;
        y->right = x;

        x->height = max(height(x->left), height(x->right)) + 1;
        y->height = max(height(y->left), height(y->right)) + 1;

        return y;
}

template <class T>
int avl_tree<T>::getBalance(Node * node) {
        return (height(node->left) - height(node->right));
}

template <class T>
typename avl_tree<T>::Node * avl_tree<T>::insert (T id, Node * node) {
        if (!node) {
                Node * new_node = new Node (id);
                count++;
                return new_node;
        }

        if (id < node->id) { // left case
                node->left = insert(id, node->left);
        } else if (id > node->id) { // right case
                node->right = insert(id, node->right);
        } else { // same id, not allowed
                return node;
        }

        node->height = max(height(node->left), height(node->right)) + 1;

        int balance = getBalance(node);


        if (balance > 1 && id < node->left->id) { // Left Left
                return rightRotate(node);
        }

        if (balance > 1 && id > node->left->id) { // Left Right
                node->left = leftRotate(node->left);
                return rightRotate(node);
        }

        if (balance < -1 && id < node->right->id) { // Right Left
                node->right = rightRotate(node->right);
                return leftRotate(node);
        }

        if (balance < -1 && id > node->right->id) { // Right Right
                return leftRotate(node);
        }

        return node;
}

template <class T>
typename avl_tree<T>::Node * avl_tree<T>::remove (T id, Node * node) {
        if (!node) {
                return node;
        }

        if (id < node->id) { // left case
                node->left = remove(id, node->left);
        } else if (id > node->id) { // right case
                node->right = remove(id, node->right);
        } else { // same id means this is the node, we want to delete
                if (!node->left && !node->right) { // no children, simply remove the node
                        delete node;
                        node = nullptr;
                        count--;
                        return nullptr;
                } else if (!node->left || !node->right) { // one child: make the child a child of its
                        // grandparent, remove the parent (=node)
                        Node * tmp = node->left ? node->left : node->right;

                        node->left = nullptr;
                        node->right = nullptr;

                        delete node;
                        node = nullptr;
                        count--;
                        return tmp;
                } else {
                        //            Node * tmp = new Node(*(getMax(node->left)));
                        Node * tmp = getMax(node->left);

                        ASSERT_TRUE(!tmp->right);

                        node->id = tmp->id; // overwrite the id of node with the id of tmp
                        node->left = remove (tmp->id, node->left);

                }
        }

        node->height = max(height(node->left), height(node->right)) + 1;

        int balance = getBalance(node);

        if (balance > 1 && getBalance(node->left) >= 0) { // Left Left
                return rightRotate(node);
        }

        if (balance > 1 && getBalance(node->left) < 0) { // Left Right
                node->left = leftRotate(node->left);
                return rightRotate(node);
        }

        if (balance < -1 && getBalance(node->right) > 0) { // Right Left
                node->right = rightRotate(node->right);
                return leftRotate(node);
        }

        if (balance < -1 && getBalance(node->right) <= 0) { // Right Right
                return leftRotate(node);
        }

        return node;
}

template <class T>
typename avl_tree<T>::Node * avl_tree<T>::get(T id, Node * node) {
        if (!node) {
                return nullptr;
        }

        if (id < node->id) {
                return get(id, node->left);
        } else if (id > node->id) {
                return get(id, node->right);
        } else { // else id == node->id
                return node;
        }
}

template <class T>
typename avl_tree<T>::Node * avl_tree<T>::getMin (Node * node) {
        if (!node->left) {
                return node;
        } else {
                return getMin (node->left);
        }
}

template <class T>
typename avl_tree<T>::Node * avl_tree<T>::getMax (Node * node) {
        if (!node->right) {
                return node;
        } else {
                return getMax (node->right);
        }
}

template <class T>
std::vector<T>& avl_tree<T>::fill (Node * node, std::vector<T>& content) {
        if (!node) {
                return content;
        }

        content = fill(node->left, content);

        content.push_back(node->id);

        content = fill(node->right, content);

        return content;
}


template <class T>
void avl_tree<T>::print (Node * node, size_t depth) {
        if (!node) return;

        print(node->right, depth+1);

        for (size_t i = 0; i < depth; ++i) {
                std::cout << "|  ";
        }
        std::cout << node->id << std::endl;

        print(node->left, depth+1);
}

#endif // AVL_TREE_H
