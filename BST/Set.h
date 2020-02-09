#include <algorithm>

template <typename ValueType>
class Set {
private:
    struct Node {
        const ValueType value;
        Node* left;
        Node* right;
        Node* parent;
        bool color;

        Node(const ValueType& key, bool color_)
            : value(key)
            , left(nullptr)
            , right(nullptr)
            , parent(nullptr)
            , color(color_)
        {}

        Node(const Node& other)
            : value(other.value)
            , left(other.left)
            , right(other.right)
            , parent(other.parent)
            , color(other.color)
        {}

        static bool isLeft(const Node* n) {
            return n->parent && n->parent->left == n;
        }

        static bool isRight(const Node* n) {
            return n->parent && n->parent->right == n;
        }

        static bool getColor(const Node* n) {
            return n == nullptr ? Black : n->color;
        }

        // returns parent's field with pointer to current node
        static Node*& getLink(Node* n) {
            if (n->parent->left == n) {
                return n->parent->left;
            } else {
                return n->parent->right;
            }
        }
    };

    Node* root;
    size_t sz;
    const static bool Red = 1;
    const static bool Black = 0;

    void rotateLeft(Node* node) {
        Node* secondNode = node->right;
        node->right = secondNode->left;
        if (node->right) {
            node->right->parent = node;
        }
        if (node->parent) {
            Node::getLink(node) = secondNode;
        }
        secondNode->parent = node->parent;
        node->parent = secondNode;
        secondNode->left = node;
        // maybe we have to change root
        if (root == node) {
            root = secondNode;
        }
    }

    void rotateRight(Node* node) {
        Node* secondNode = node->left;
        node->left = secondNode->right;
        if (node->left) {
            node->left->parent = node;
        }
        if (node->parent) {
            Node::getLink(node) = secondNode;
        }
        secondNode->parent = node->parent;
        node->parent = secondNode;
        secondNode->right = node;
        // maybe we have to change root
        if (node == root) {
            root = secondNode;
        }
    }

    Node* deepCopy(const Node* node) {
        if (node == nullptr) {
            return nullptr;
        }
        Node* result = new Node(*node);
        result->left = deepCopy(node->left);
        if (result->left) {
            result->left->parent = result;
        }
        result->right = deepCopy(node->right);
        if (result->right) {
            result->right->parent = result;
        }
        return result;
    }

    void clean_memory(Node* v) {
        if (v == nullptr) {
            return;
        }
        clean_memory(v->left);
        clean_memory(v->right);
        delete v;
    }

    // returns minimal Node in subtree
    static const Node* minNode(const Node* n) {
        while (n->left) {
            n = n->left;
        }
        return n;
    }

    static Node* minNode(Node* n) {
        return const_cast<Node*>(minNode(const_cast<const Node*>(n)));
    }

    static const Node* maxNode(const Node* n) {
        while (n->right) {
            n = n->right;
        }
        return n;
    }

    static Node* maxNode(Node* n) {
        return const_cast<Node*>(maxNode(const_cast<const Node*>(n)));
    }

    // we have to swap nodes without swapping elements
    void swapNodes(Node* x, Node* y) {
        if (x->parent) {
            Node::getLink(x) = y;
        }
        if (y->parent) {
            Node::getLink(y) = x;
        }
        std::swap(x->parent, y->parent);
        std::swap(x->left, y->left);
        std::swap(x->right, y->right);
        // fixing children's parents
        if (x->left) {
            x->left->parent = x;
        }
        if (x->right) {
            x->right->parent = x;
        }
        if (y->left) {
            y->left->parent = y;
        }
        if (y->right) {
            y->right->parent = y;
        }
    }

    void fixDeletion(Node* current) { // Deletion cases 2-6
        // while we have to fix smth
        while (current != root && current->color == Black) {
            if (Node::isLeft(current)) {
                Node *brother = current->parent->right; // Can not be NULL
                // Case 2
                if (brother->color == Red) {
                    current->parent->color = Red;
                    brother->color = Black;
                    rotateLeft(current->parent);
                    continue;
                }
                // Now brother is BLACK
                if (Node::getColor(brother->left) == Black && Node::getColor(brother->right) == Black) {
                    // Case 4
                    if (Node::getColor(current->parent) == Red) {
                        brother->color = Red;
                        current->parent->color = Black;
                        current = root;
                    } else { // Case 3
                        brother->color = Red;
                        current->parent->color = Black;
                        current = current->parent;
                    }
                    continue;
                }
                // Case 5
                if (Node::getColor(brother->left) == Red && Node::getColor(brother->right) == Black) {
                    brother->left->color = Black;
                    brother->color = Red;
                    rotateRight(brother);
                    brother = brother->parent;
                    continue;
                }
                // Case 6
                if (Node::getColor(brother->right) == Red) {
                    brother->right->color = Black;
                    brother->color = current->parent->color;
                    current->parent->color = Black;
                    rotateLeft(current->parent);
                    current = root;
                }
            } else {
                Node *brother = current->parent->left; // Can not be NULL
                // Case 2
                if (brother->color == Red) {
                    current->parent->color = Red;
                    brother->color = Black;
                    rotateRight(current->parent);
                    continue;
                }
                if (Node::getColor(brother->right) == Black && Node::getColor(brother->left) == Black) {
                    // Case 4
                    if (Node::getColor(current->parent) == Red) {
                        brother->color = Red;
                        current->parent->color = Black;
                        current = root;
                    } else { // Case 3
                        brother->color = Red;
                        current->parent->color = Black;
                        current = current->parent;
                    }
                    continue;
                }
                // Case 5
                if (Node::getColor(brother->right) == Red && Node::getColor(brother->left) == Black) {
                    brother->right->color = Black;
                    brother->color = Red;
                    rotateLeft(brother);
                    brother = brother->parent;
                    continue;
                }
                // Case 6
                if (Node::getColor(brother->left) == Red) {
                    brother->left->color = Black;
                    brother->color = current->parent->color;
                    current->parent->color = Black;
                    rotateRight(current->parent);
                    current = root;
                }
            }
        }
        root->color = Black;
        current->color = Black;
    }

    Node* basic_find(const ValueType& val) const {
        Node* res = root;
        while (res && (res->value < val || val < res->value)) {
            if (val < res->value) {
                res = res->left;
            } else {
                res = res->right;
            }
        }
        return res;
    }

public:
    Set()
        : root(nullptr)
        , sz(0)
    {}

    template <typename inputIt>
    Set(inputIt begin, inputIt end)
        : Set()
    {
        while (begin != end) {
            insert(*begin);
            ++begin;
        }
    }

    Set(const std::initializer_list<ValueType>& elements)
        : Set(elements.begin(), elements.end())
    {}

    Set(const Set& other)
        : Set()
    {
        sz = other.sz;
        root = deepCopy(other.root);
    }

    Set& operator = (const Set& other) {
        if (this == &other) {
            return *this;
        }
        clean_memory(root);
        sz = other.sz;
        root = deepCopy(other.root);
        return *this;
    }

    ~Set() {
        clean_memory(root);
    }

    void erase(const ValueType& value) {
        Node* current = basic_find(value);
        if (current == nullptr) {
            return;
        }
        --sz;
        // Bad case, we have two sons. Swapping with minimal vertex in right subtree
        if (current->left && current->right) {
            Node* next = minNode(current->right);
            if (current == root) {
                root = next;
            }
            swapNodes(current, next);
            std::swap(current->color, next->color);
        }
        // Easy case, no sons
        if (current->left == nullptr && current->right == nullptr) {
            if (current == root) {
                root = nullptr;
            } else {
                // Property 5 is broken. Fixing
                if (current->color == Black) {
                    fixDeletion(current);
                }
                Node::getLink(current) = nullptr;
            }
            delete current;
            return;
        }
        Node* child = (current->left == nullptr ? current->right : current->left);
        if (current == root) {
            root = child;
            child->parent = nullptr;
        } else {
            Node::getLink(current) = child;
            child->parent = current->parent;
        }
        // Property 5 is broken. Fixing
        if (current->color == Black) {
            fixDeletion(child);
        }
        delete current;
    }

    size_t size() const {
        return sz;
    }

    bool empty() const {
        return sz == 0;
    }

    void insert(const ValueType& x) {
        if (root == nullptr) {
            Node* newNode = new Node(x, Red);
            root = newNode;
            newNode->color = Black;
            ++sz;
            return;
        }
        Node* currentNode = root;
        Node* newNode = nullptr;
        // We have to find Node, whose son we will be
        while (newNode == nullptr) {
            // Element already exists. Nothing to do
            if (!(x < currentNode->value || currentNode->value < x)) {
                return;
            }
            if (x < currentNode->value) {
                // Found
                if (currentNode->left == nullptr) {
                    newNode = new Node(x, Red);
                    currentNode->left = newNode;
                    newNode->parent = currentNode;
                } else {
                    currentNode = currentNode->left;
                }
            } else {
                // Found
                if (currentNode->right == nullptr) {
                    newNode = new Node(x, Red);
                    currentNode->right = newNode;
                    newNode->parent = currentNode;
                } else {
                    currentNode = currentNode->right;
                }
            }
        }
        currentNode = newNode;
        // Fixing insertion (cases 3-5)
        while (currentNode->parent && currentNode->parent->color == Red) { // => parent != root
            if (Node::isLeft(currentNode->parent)) {
                Node* uncle = currentNode->parent->parent->right;
                // Case 3
                if (Node::getColor(uncle) == Red) {
                    currentNode->parent->color = Black;
                    uncle->color = Black;
                    currentNode->parent->parent->color = Red;
                    currentNode = currentNode->parent->parent;
                } else {
                    // Case 4
                    if (Node::isRight(currentNode)) {
                        currentNode = currentNode->parent;
                        rotateLeft(currentNode); // now we are left son of previous currentNode
                    }
                    // Case 5
                    currentNode->parent->color = Black;
                    currentNode->parent->parent->color = Red;
                    rotateRight(currentNode->parent->parent);
                }
            } else {
                Node* uncle = currentNode->parent->parent->left;
                // Case 3
                if (uncle && uncle->color == Red) {
                    currentNode->parent->color = Black;
                    uncle->color = Black;
                    currentNode->parent->parent->color = Red;
                    currentNode = currentNode->parent->parent;
                } else {
                    // Case 4
                    if (Node::isLeft(currentNode)) {
                        currentNode = currentNode->parent;
                        rotateRight(currentNode);
                    }
                    // Case 5
                    currentNode->parent->color = Black;
                    currentNode->parent->parent->color = Red;
                    rotateLeft(currentNode->parent->parent);
                }
            }
        }
        root->color = Black;
        ++sz;
    }

    class iterator {
    public:
        iterator()
            : ptr(nullptr)
            , mySet(nullptr)
        {}

        iterator(const iterator& other)
            : ptr(other.ptr)
            , mySet(other.mySet)
        {}

        iterator(const Node* ptr_, const Set* set)
            : ptr(ptr_)
            , mySet(set)
        {}

        const ValueType& operator * () const {
            return ptr->value;
        }

        const ValueType* operator -> () const {
            return &ptr->value;
        }

        bool operator == (iterator other) const {
            return ptr == other.ptr;
        }

        bool operator != (iterator other) const {
            return ptr != other.ptr;
        }

        iterator& operator ++ () {
            if (ptr->right) {
                ptr = minNode(ptr->right);
                return *this;
            }
            while (Node::isRight(ptr)) {
                ptr = ptr->parent;
            }
            if (ptr->parent && Node::isLeft(ptr)) {
                ptr = ptr->parent;
            } else {
                ptr = nullptr;
            }
            return *this;
        }

        iterator& operator -- () {
            if (ptr == nullptr) {
                ptr = maxNode(mySet->root);
                return *this;
            }
            if (ptr->left) {
                ptr = maxNode(ptr->left);
                return *this;
            }
            while (Node::isLeft(ptr)) {
                ptr = ptr->parent;
            }
            if (Node::isRight(ptr)) {
                ptr = ptr->parent;
            } else {
                ptr = nullptr;
            }
            return *this;
        }

        iterator operator ++ (int) {
            iterator tmp = *this;
            ++*this;
            return tmp;
        }

        iterator operator -- (int) {
            iterator tmp = *this;
            --*this;
            return tmp;
        }

    private:
        const Node* ptr;
        const Set* mySet;
    };

    iterator begin() const {
        return iterator(root ? minNode(root) : root, this);
    }

    iterator end() const {
        return iterator(nullptr, this);
    }

    iterator find(const ValueType& value) const {
        return iterator(basic_find(value), this);
    }

    void clear() {
        *this = Set();
    }

    iterator lower_bound(const ValueType& value) const {
        if (empty()) {
            return end();
        }
        Node* curr = root;
        while (curr->value < value || value < curr->value) {
            if (value < curr->value) {
                if (curr->left) {
                    curr = curr->left;
                } else {
                    break;
                }
            } else {
                if (curr->right) {
                    curr = curr->right;
                } else {
                    break;
                }
            }
        }
        iterator res = iterator(curr, this);
        if (*res < value) {
            ++res;
        }
        return res;
    }
};
