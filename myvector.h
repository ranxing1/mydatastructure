#include <iostream>
#include <mutex>

template<typename V>
class Vector {
public:
	Vector();
	~Vector();
	Vector( Vector& vector);
	bool push_back(V value);
	int count(V value) const;
	int getSize() const;
	int getCapacity() const;
	V erase(V value);
	void display() const;
	V& operator[](int index);

private:
	bool restart();
	void toFront(int num);

private:
	V* v;
	int size;
	int capacity;
	std::mutex mtx;
};

template<typename V>
Vector<V>::Vector() : size(0), capacity(5)
{
	v = new V[capacity];
}

template<typename V>
Vector<V>::~Vector()
{
	delete[] v;
}

template<typename V>
Vector<V>::Vector( Vector& vector)
{
	delete[] v;
	size = vector.getSize();
	capacity = vector.getCapacity();
	v = new V[capacity];
	for (int i = 0; i < size; i++)
	{
		*(v + i) = vector[i];
	}
}

template<typename V>
V& Vector<V>::operator[](int index)
{
	return *(v + index);
}

template<typename V>
bool Vector<V>::push_back(V value)
{
	bool flag = false;
	if (size >= capacity)
	{
		flag = restart();
		if (!flag)
			return flag;
	}
	mtx.lock();
	*(v + size) = value;
	size++;
	mtx.unlock();
	return flag;
}


template<typename V>
int Vector<V>::count(V value) const
{
	int cnt = 0;
	for (int i = 0; i < size; i++)
	{
		if (*(v + i) == value)
			cnt++;
	}
	return cnt;
}

template<typename V>
V Vector<V>::erase(V value)
{
	for (int i = 0; i < size; i++)
	{
		mtx.lock();
		if (*(v + i) == value)
		{
			V x = *(v + i);
			toFront(i);
			size--;
			mtx.unlock();
			return x;
		}
		mtx.unlock();
	}
}

template<typename V> 
void Vector<V>::display() const
{
	for (int i = 0; i < size; i++)
	{
		std::cout << *(v + i) << " ";
	}
	std::cout << std::endl;
}

template<typename V>
void Vector<V>::toFront(int num)
{
	for (int i = num; i < size - 1; i++) 
	{
		*(v + i) = *(v + i + 1);
	}
}

template<typename V>
bool Vector<V>::restart()
{
	mtx.lock();
	capacity *= 2;
	V* newv1 = new V[capacity];
	V* oldv = v;
	for (int i = 0; i < size; i++)
	{
		*(newv1 + i) = *(v + i);
	}
	v = newv1;
	delete[] oldv;
	mtx.unlock();
	return true;
}


template<typename V>
int Vector<V>::getSize() const
{
	return size;
}

template<typename V>
int Vector<V>::getCapacity() const
{
	return capacity;
}

