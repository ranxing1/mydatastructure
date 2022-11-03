#include "mylist.h"
#include <mutex>

template<typename V>
class Queue {
public:
	Queue();
	~Queue();
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
Queue<V>::Queue() : size(0)
{
	list = new List<V, V>();
}

template<typename V>
Queue<V>::~Queue()
{
	delete list;
}

template<typename V>
V Queue<V>::top() const
{
	return list->head->next->getValue();
}

template<typename V>
bool Queue<V>::pop()
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
bool Queue<V>::push(V value)
{
	bool flag = list->push_back(value, value);
	if (flag)
	{
		mtx.lock();
		size++;
		mtx.unlock();
	}
	return flag;
}

template<typename V>
int Queue<V>::getSize() const
{
	return size;
}
