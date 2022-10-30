#pragma once
#include <iostream>
#include <mutex>
#include <cstring>
#include <cstdlib>

//           节              点
template<typename K, typename V>
class Node {
public:
	Node();
	Node(K key, V value);
	K getKey() const;
	V getValue() const;
	void setValue(V value);

	Node* next;
	Node* pre;
private:
	K _key;
	V _value;
};
template<typename K, typename V>
Node<K, V>::Node() {}
template<typename K, typename V>
Node<K, V>::Node(K key, V value) : _key(key), _value(value)
{
	pre = next = NULL;
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
	_value = value;
}




//    升    序    链      表
template<typename K, typename V>
class AscList {
public:
	AscList();
	~AscList();
	bool insertNode(K key, V value);
	bool deleteNode(K key);
	void displayList();
	Node<K, V>* searchNode(K key);

	Node<K, V>* head;
	Node<K, V>* tail;
private:
	Node<K, V>* createNode(K key, V value);
	std::mutex mtx;
};

template<typename K, typename V>
AscList<K, V>::AscList()
{
	head = new Node<K, V>;
	tail = new Node<K, V>;
	head->next = head->pre = tail;
	tail->next = tail->pre = head;
}

template<typename K, typename V>
AscList<K, V>::~AscList()
{
	mtx.lock();
	Node<K, V>* p = head;
	while (p != tail)
	{
		/*std::cout << head << std::endl;*/
		p = p->next;
		delete head;
		head = p;
		//std::cout << "释放" << std::endl;
	}
	delete tail;
	mtx.unlock();
}

template<typename K, typename V>
Node<K, V>* AscList<K, V>::searchNode(K key)
{
	Node<K, V>* node = head->next;
	mtx.lock();
	while (node != tail)
	{
		if (node->getKey() == key)
		{
			mtx.unlock();
			return node;
		}
		node = node->next;
	}
	mtx.unlock();
	return NULL;
}

template<typename K, typename V>
bool AscList<K, V>::insertNode(K key, V value)
{
	Node<K, V>* node = createNode(key, value);
	Node<K, V>* p = head->next;
	while (1)
	{
		if (p == tail)
		{
			tail->pre->next = node;
			node->pre = head;
			node->next = tail;
			tail->pre = node;
			break;
		}
		else if (node->getKey() < p->getKey())
		{
			p->pre->next = node;
			node->next = p;
			node->pre = p->pre;
			p->pre = node;
			break;
		}	
		p = p->next;
	}
	return true;
}


template<typename K, typename V>
bool AscList<K, V>::deleteNode(K key)
{
	Node<K, V>* node = searchNode(key);
	if (node == NULL)
		return false;
	mtx.lock();
	node->pre->next = node->next;
	node->next->pre = node->pre;
	delete node;
	mtx.unlock();
	return true;
}

template<typename K, typename V>
void AscList<K, V>::displayList()
{
	Node<K, V>* current = head->next;
	while (current != tail)
	{
		std::cout << "key: " << current->getKey() << " value: " << current->getValue() << "    ";
		current = current->next;
	}
}

template<typename K, typename V>
Node<K, V>* AscList<K, V>::createNode(K key, V value)
{
	Node<K, V>* node = new Node<K, V>(key, value);
	//std::cout << "node: " << node << std::endl;
	return node;
}
