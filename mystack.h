#include "mylist.h"
#include <iostream>
#include<mutex>

template<typename V>
class Stack {
public:
	Stack();
	~Stack();
	V top() const;
	bool pop();
	bool push(V value);
	int getSize() const;
private:
	List<V, V>* list;
	int size;
	std::mutex mtx;
};

template<typename V>
Stack<V>::Stack() : size(0)
{
	list = new List<V,V>();
}

template<typename V>	
Stack<V>::~Stack()
{
	delete list;
}

template<typename V>
V Stack<V>::top() const
{
	return list->head->next->getValue();
}

template<typename V>
bool Stack<V>::pop()
{
	bool flag = list->deleteNode(list->head->next->getKey());
	if (flag)
	{
		mtx.lock();
		size--;
		mtx.unlock();
	}
	return flag;
}

template<typename V>
bool Stack<V>::push(V value)
{
	bool flag = list->push_front(value, value);
	if (flag)
	{
		mtx.lock();
		size++;
		mtx.unlock();
	}
	return flag;
}

template<typename V>
int Stack<V>::getSize() const
{
	return size;
}

