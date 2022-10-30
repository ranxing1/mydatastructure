#pragma once
#include <iostream>
#include <mutex>
#include <cstring>
#include <cstdlib>

//           节              点
template<typename K,typename V>
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
Node<K, V>::Node(){}
template<typename K, typename V>
Node<K,V>::Node(K key, V value) : _key(key),_value(value)
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
void Node<K,V>::setValue(V value)
{
	_value = value;
}


//       链             表
template<typename K, typename V>
class List {
public:
	List();
	~List();
	bool push_front(K key, V value);
	bool push_back(K key, V value);
	bool insertNode(K key, V value, K preKey);
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
List<K, V>::List()
{
	head = new Node<K, V>;
	tail = new Node<K, V>;
	head->next = head->pre = tail;
	tail->next = tail->pre = head;
}

template<typename K, typename V>
List<K, V>::~List()
{
	mtx.lock();
	Node<K,V>* p = head;
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
bool List<K, V>::push_front(K key, V value)
{
	mtx.lock();
	Node<K, V>* node = createNode(key, value);
	head->next->pre = node;
	node->next = head->next;
	node->pre = head;
	head->next = node;
	mtx.unlock();
	return true;
}

template<typename K, typename V>
bool List<K, V>::push_back(K key, V value)
{
	mtx.lock();
	Node<K, V>* node = createNode(key, value);
	tail->pre->next = node;
	node->pre = tail->pre;
	node->next = tail;
	tail->pre = node;
	mtx.unlock();
	return true;
}

template<typename K, typename V>
Node<K, V>* List<K,V>::searchNode(K key)
{
	Node<K,V>* node = head->next;
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
bool List<K,V>::insertNode(K key, V value, K preKey)
{
	Node<K,V>* preNode = searchNode(preKey);
	if (preNode == NULL)
		return false;
	Node<K, V>* node = createNode(key, value);
	mtx.lock();
	preNode->next->pre = node;
	node->next = preNode->next;
	node->pre = preNode;
	preNode->next = node;
	mtx.unlock();
	return true;
}

template<typename K, typename V>
bool List<K, V>::deleteNode(K key)
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
void List<K, V>::displayList()
{
	Node<K, V>* current = head->next;
	while (current != tail)
	{
		std::cout << "key: " << current->getKey() << " value: " << current->getValue() << "    ";
		current = current->next;
	}
}

template<typename K, typename V>
Node<K,V>* List<K, V>::createNode(K key, V value)
{
	Node<K,V>* node = new Node<K, V>(key, value);
	//std::cout << "node: " << node << std::endl;
	return node;
}