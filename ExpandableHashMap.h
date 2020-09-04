// ExpandableHashMap.h

// Skeleton for the ExpandableHashMap class template.  You must implement the first six
// member functions.
#include <vector>
#include <string>
using namespace std;
template<typename KeyType, typename ValueType>
class ExpandableHashMap
{
public:
	ExpandableHashMap(double maximumLoadFactor = 0.5);
	~ExpandableHashMap();
	void reset();
	int size() const;

	//gets the index of the bucket to enter to recieve
	unsigned int getBucketNumber(const KeyType& key) const;

	//inserts or updates a element in the key, vlaue pair
	void associate(const KeyType& key, const ValueType& value);

	// for a map that can't be modified, return a pointer to const ValueType
	const ValueType* find(const KeyType& key) const {
		int bucket = getBucketNumber(key) % m_buckets;
		Node* temp = m_hashMap[bucket];
		while (temp != nullptr) {
			if (temp->key == key)
				return &(temp->val);
			temp = temp->next;
		}
		return nullptr;
	}

	// for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
	}

	// C++11 syntax for preventing copying and assignment
	ExpandableHashMap(const ExpandableHashMap&) = delete;
	ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;

private:
	//every thing in the hashtable will be a node that has a key, value, and a next pointer
	struct Node {
		KeyType key;
		ValueType val;
		Node* next = nullptr;
	};
	int m_size; //number of elements
	int m_buckets = 8; //number of buckets
	double m_maxLoadFactor;
	vector<Node*> m_hashMap;

	//sets all nodes in the vector to be nullptr
	void ptrSet(vector<Node*> v, int size);
	//gets the loadfactor
	double loadFactor();
	//deletes everything in the hashTable
	void ptrDelete();
};

//Constructor for Expandable HashTable
template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::ExpandableHashMap(double maximumLoadFactor) {
	//set 8 spots in vector to be nullptr
	ptrSet(m_hashMap, 8);
	m_size = 0; //set size to be originally zero
	//if passed in load factor is less than 0, then make it 0.5 otherwise, make it 0.5
	if (maximumLoadFactor <= 0)
		m_maxLoadFactor = 0.5;
	else
		m_maxLoadFactor = maximumLoadFactor;
}

//Destructor will delete all of the dynamically allocated data in the hashtable
template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::~ExpandableHashMap() {
	ptrDelete();
}

//This function goes through the entire Expandable HashTable and deletes every node
template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::ptrDelete() {
	//go through every element in the vector
	for (int i = 0; i < m_buckets; i++) {
		if (m_hashMap[i] != nullptr) {
			Node* temp = m_hashMap[i];
			//delete the entire linked list in each spot
			while (temp != nullptr) {
				Node* deleteNode = temp;
				temp = deleteNode->next;
				delete deleteNode;
			}
		}
	}
}

//Calculates and returns the loadFactor
template<typename KeyType, typename ValueType>
double ExpandableHashMap<KeyType, ValueType>::loadFactor() {
	double dSize = double(m_size);
	double dBuckets = double(m_buckets);
	return dSize/dBuckets;
}

//Takes in a size and pushed nullptr into the vector n times of nullptr
template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::ptrSet(vector<Node*> v, int size) {
	for (int i = 0; i < size; i++) {
		m_hashMap.push_back(nullptr);
	}
}

//Insert and update values in the hashTable
template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value) {
	//get the index hwere to look at 
	int bucketNumber = getBucketNumber(key) % m_buckets;
	Node* temp = m_hashMap[bucketNumber];
	//check if the value exists in the index of the hashTable
	bool found = false;
	while (temp != nullptr) {
		if (temp->key == key) {
			found = true;
			break;
		}
		temp = temp->next;
	}
	if (found) { //if it already exist, change it's value
		temp->key = key;
		temp->val = value;
	}
	else { //otherwise create and new node and insert
		Node* a = new Node;
		a->val = value;
		a->key = key;
		a->next = nullptr;
		temp = m_hashMap[bucketNumber];
		if (temp == nullptr) //if spot is empty, simply just insert there
		{
			m_hashMap[bucketNumber] = a;
		}
		else { //otherwise link it to last spot in the index
			while (temp->next != nullptr) {
				temp = temp->next;
			}
			temp->next = a;
		}
		m_size++;
	}

	//If you need reorganize the vector
	if (loadFactor() > m_maxLoadFactor) {
		//create a vector with double the space
		vector<Node*> newVector(m_buckets * 2);
		Node* curr = nullptr;
		//initialize is to all nullptr
		ptrSet(newVector, m_buckets * 2);
		for (int i = 0; i < m_buckets; i++) {
			curr = m_hashMap[i];
			//for every element of hashTable, insert it into new hashTable
			while (curr != nullptr) {
				Node* newNode = new Node;
				newNode->val = curr->val;
				newNode->key = curr->key;
				newNode->next = nullptr;
				int bucket = getBucketNumber(newNode->key) % (2 * m_buckets);
				Node* t = newVector[bucket];
				if (t == nullptr) { //insert is if the spot is empty
					newVector[bucket] = newNode;
				}
				else { //go until you can insert it 
					while (t->next != nullptr)
						t = t->next;
					t->next = newNode;
				}
				curr = curr->next;
			}
		}
		ptrDelete(); //delete all the data in the old hashtable
		m_buckets *= 2; //increase the amount of buckets by 2 times
		m_hashMap = newVector; //set old vector to new vector
	}
}

template<typename KeyType, typename ValueType>
unsigned int  ExpandableHashMap<KeyType, ValueType>::getBucketNumber(const KeyType& key) const
{
	//call hasher and return the unsigned int bucket number
	unsigned int hasher(const KeyType & k);
	unsigned int h = hasher(key);
	return h;
}

//return the number of elements 
template<typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::size() const {
	return m_size;
}

//Deletes all pointers in hashTable and reduces it size back to 8
template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::reset() {
	ptrDelete();
	vector<Node*> newVector;
	ptrSet(newVector, 8);
	m_hashMap = newVector;
}