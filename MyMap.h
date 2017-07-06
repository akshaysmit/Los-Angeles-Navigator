// MyMap.h

// Skeleton for the MyMap class template.  You must implement the first six
// member functions.

template<typename KeyType, typename ValueType>
class MyMap
{
public:
	MyMap();
	~MyMap();
	void clear();
	int size() const;
	void associate(const KeyType& key, const ValueType& value);

	  // for a map that can't be modified, return a pointer to const ValueType
	const ValueType* find(const KeyType& key) const;

	  // for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const MyMap*>(this)->find(key));
	}

	  // C++11 syntax for preventing copying and assignment
	MyMap(const MyMap&) = delete;
	MyMap& operator=(const MyMap&) = delete;

private:

    struct Node {
        
        Node(KeyType key, ValueType val, Node* left, Node* right)
         : m_key(key), m_val(val), m_left(left), m_right(right)
        {}
        
        KeyType m_key;
        ValueType m_val;
        Node* m_left;
        Node* m_right;
    };
    
    int m_size;
    Node* m_root;
    
    /* private member functions */
    void clearAux(Node* &root);
    void associateAux(const KeyType& key, const ValueType& value, Node* &root);
    ValueType* findAux(const KeyType& key, Node* root) const;
};

template<typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>::MyMap() : m_size(0), m_root(nullptr) {}

template<typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>::~MyMap() {
    clear();
}

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::clear() {
    clearAux(m_root);
    m_size=0;
}

template<typename KeyType, typename ValueType>
int MyMap<KeyType, ValueType>::size() const { return m_size; }

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::associate(const KeyType &key, const ValueType &value) {
    associateAux(key, value, m_root);
}

template<typename KeyType, typename ValueType>
const ValueType* MyMap<KeyType, ValueType>::find(const KeyType &key) const {
    return findAux(key, m_root);
}

/* private member functions */

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::clearAux(MyMap::Node* &root) {
    
    if(root == nullptr)
        return;
    
    if (root->m_left != nullptr)
        clearAux(root->m_left);
    
    if (root->m_right != nullptr)
        clearAux(root->m_right);
    
    delete root;
    root = nullptr;
}

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::associateAux(const KeyType &key, const ValueType &value, Node* &root) {
    
    if (root == nullptr) {
        root = new Node(key, value, nullptr, nullptr);
        m_size++;
        return;
    }
    
    if (root->m_key == key) {
        root->m_val = value;
        return;
    }
    
    if (root->m_key < key)
        associateAux(key, value, root->m_right);
    else
        associateAux(key, value, root->m_left);
}

template<typename KeyType, typename ValueType>
ValueType* MyMap<KeyType, ValueType>::findAux(const KeyType &key, Node *root) const {
    
    if (root == nullptr)
        return nullptr;
    
    if (root->m_key == key)
        return &(root->m_val);
    
    if (root->m_key < key)
        return findAux(key, root->m_right);
    else
        return findAux(key, root->m_left);
}

/*
 void tempPrintTree(Node* root, int depth) {
 if (root == nullptr)
 return;
 std::cerr << std::string(2*depth, ' ') << root->m_key << ": " << root->m_val << std::endl;
 tempPrintTree(root->m_left, depth+1);
 tempPrintTree(root->m_right, depth+1);
 }
 
 void tempPrintTree() {
 tempPrintTree(m_root, 0);
 }
*/