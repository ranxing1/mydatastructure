#pragma once
#include <iostream> 
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <mutex>

template<typename K,typename V>
class Node {
public:
	Node();
	Node(K key, V value, int);
	~Node();
	K getKey() const;
	V getValue() const;
	void setValue(V value);

	int nodeLevel;
	Node<K, V>** forward;
private:
	K _key;
	V _value;
};

template<typename K,typename V>
Node<K,V>::Node()
{}

template<typename K, typename V>
Node<K, V>::Node(K key,V value,int level) : _key(key),_value(value),nodeLevel(level)
{
	this->forward = new Node<K, V>*[level + 1];
	memset(this->forward, 0, sizeof(Node<K, V>*) * (level + 1));
}

template<typename K, typename V>
Node<K,V>::~Node()
{
	delete[] forward;
}

template<typename K, typename V>
K Node<K, V>::getKey() const
{
	return _key;
}

template<typename K, typename V>
V Node<K, V>::getValue() const
{
	return _value;
}

template<typename K, typename V>
void Node<K, V>::setValue(V value)
{
	this->_value = value;
}





template<typename K, typename V>
class SkipList{
public:
	SkipList(int);
	~SkipList();
	int getRandomLevel();
	Node<K, V>* createNode(K key, V value, int);
	void displayList();
	int size();
	int insertNode(K key, V value);
	bool deleteNode(K key);
	bool searchNode(K key);

private:
	//the Maximum level of skipList
	int _maxLevel;

	//the current level of skipList
	int _skipListLevel;

	//the head of skipList
	Node<K, V>* head;

	//the node count of skipList
	int nodeCnt;

	std::mutex mtx;
};

template<typename K, typename V>
Node<K, V>* SkipList<K,V>::createNode(K key, V value,int level)
{
	Node<K, V>* node = new Node<K, V>(key, value, level);
	return node;
}

// Insert given key and value in skip list 
// return 1 means element exists  
// return 0 means insert successfully
/*
						   +------------+
						   |  insert 50 |
						   +------------+
level 4     +-->1+                                                      100
				 |
				 |                      insert +----+
level 3         1+-------->10+---------------> | 50 |          70       100
											   |    |
											   |    |
level 2         1          10         30       | 50 |          70       100
											   |    |
											   |    |
level 1         1    4     10         30       | 50 |          70       100
											   |    |
											   |    |
level 0         1    4   9 10         30   40  | 50 |  60      70       100
											   +----+

*/
template<typename K, typename V>
int SkipList<K, V>::insertNode(K key, V value)
{
	mtx.lock();
	Node<K, V>* current = head;

	// create update array and initialize it 
	// update is array which put node that the node->forward[i] should be operated later
	Node<K, V>** update = new Node<K, V>* [this->_maxLevel + 1];
	memset(update, 0, sizeof(Node<K, V>*) * (this->_maxLevel + 1));

	for (int i = _skipListLevel; i >= 0; --i)
	{
		while (current->forward[i] != NULL && current->forward[i]->getKey() < key)
		{
			current = current->forward[i];
		}
		update[i] = current;
	}

	current = current->forward[0];
	if (current != NULL && current->getKey() == key)
	{
		std::cout << "key: " << key << " ,is exist" << std::endl;
		mtx.unlock();
		delete[] update;
		return 1;
	}

	if (current == NULL || current->getKey() != key)
	{
		int randomLevel = getRandomLevel();
		if (randomLevel > _skipListLevel) 
		{
			for (int i = _skipListLevel + 1; i < randomLevel; ++i)
			{
				update[i] = head;
			}
			_skipListLevel = randomLevel;
		}
		Node<K, V>* insertNode = new Node<K, V>(key, value, randomLevel);

		//insert node
		for (int i = 0; i <= randomLevel; ++i)
		{
			insertNode->forward[i] = update[i]->forward[i];
			update[i]->forward[i] = insertNode->forward[i];
		}
		std::cout << "Successfully inserted key: " << key << ", value:" << value << std::endl;
		nodeCnt++;
	}

	mtx.unlock();
	delete[] update;
	return 0;
}



// Search for element in skip list 
/*
						   +------------+
						   |  select 60 |
						   +------------+
level 4     +-->1+                                                      100
				 |
				 |
level 3         1+-------->10+------------------>50+           70       100
												   |
												   |
level 2         1          10         30         50|           70       100
												   |
												   |
level 1         1    4     10         30         50|           70       100
												   |
												   |
level 0         1    4   9 10         30   40    50+-->60      70       100
*/
template<typename K, typename V>
bool SkipList<K, V>::searchNode(K key)
{
	std::cout << "search  node-----------------" << std::endl;
	Node<K, V>* current = head;
	for (int i = _skipListLevel; i >= 0; i++)
	{
		while (current != NULL && current->forward[i]->getKey() < key)
		{
			current = current->forward[i];
		}
	}

	current = current->forward[0];
	if (current != NULL && current->getKey() == key)
	{
		std::cout << "Found key: " << key << ", value: " << current->get_value() << std::endl;
		return true;
	}

	std::cout << "Not Found Key:" << key << std::endl;
	return false;
}



template<typename K, typename V>
void SkipList<K, V>::displayList()
{
	std::cout << "\n*****Skip List*****" << "\n";
	for (int i = _skipListLevel; i >= 0; --i)
	{
		Node<K, V>* node = this->head->forward[i];
		std::cout << "Level " << i << ": ";
		while (node != NULL)
		{
			std::cout << node->getKey() << ":" << node->getValue() << ";";
			node = node->forward[i];
		}
		std::cout << std::endl;
	}
}


template<typename K, typename V>
int SkipList<K, V>::size()
{
	return nodeCnt;
}

template<typename K, typename V>
SkipList<K, V>::SkipList(int maxLevel) : _maxLevel(maxLevel)
{
	this->_skipListLevel = 0;
	this->nodeCnt = 0;

	this->head = new Node<K, V>(0,0, maxLevel);
}

template<typename K, typename V>
SkipList<K, V>::~SkipList()
{
	delete head;
}


template<typename K, typename V>
int SkipList<K, V>::getRandomLevel()
{
	int k = 1;
	while (rand() % 2) {
		k++;
	}
	k = (k < _maxLevel) ? k : _maxLevel;
	return k;
}