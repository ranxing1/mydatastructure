#include <mutex>
#include <iostream>
template<typename K,typename V>
class AvlNode {
public:
	AvlNode(K key, V value);
	~AvlNode();
	K getKey() const;
	V getValue() const;
	void setKey(K key);
	void setValue(V value);

	int bf;
	AvlNode* lchild, * rchild;
private:
	K _key;
	V _value;
};

template<typename K, typename V>
AvlNode<K, V>::AvlNode(K key, V value) : _key(key), _value(value), lchild(nullptr), rchild(nullptr), bf(0)
{}

template<typename K, typename V>
AvlNode<K, V>::~AvlNode()
{}

template<typename K, typename V>
K AvlNode<K, V>::getKey() const
{
	return _key;
}

template<typename K, typename V>
V AvlNode<K, V>::getValue() const
{
	return _value;
}

template<typename K, typename V>
void AvlNode<K, V>::setKey(K key)
{
	_key = key;
}

template<typename K, typename V>
void AvlNode<K, V>::setValue(V value)
{
	_value = value;
}

template<typename K,typename V>
class AvlTree {
public:
	AvlTree(K key,V value);
	~AvlTree();
	int insertNode(K key, V value);
	//bool deleteNode(K key);
	void display();
	int getSize() const;
private:
	void inorder(AvlNode<K,V>* current);
	void deleteAll(AvlNode<K, V>* current);
	AvlNode<K, V>* createNode(K key, V value);
	AvlNode<K, V>* LL_Rotate(AvlNode<K, V>* noavl);
	AvlNode<K, V>* RR_Rotate(AvlNode<K, V>* noavl);
	AvlNode<K, V>* LR_Rotate(AvlNode<K, V>* noavl);
	AvlNode<K, V>* RL_Rotate(AvlNode<K, V>* noavl);
private:
	AvlNode<K,V>* root;
	std::mutex mtx;
	int size;
};

template<typename K, typename V>
AvlTree<K,V>::AvlTree(K key, V value)
{
	root = createNode(key, value);
}

template<typename K, typename V>
AvlTree<K, V>::~AvlTree()
{
	deleteAll(root);
}

template<typename K, typename V>
int AvlTree<K, V>::insertNode(K key, V value)
{
	mtx.lock();
	if (root == nullptr)
	{
		root = createNode(key, value);
		mtx.unlock();
		return 0;
	}
	AvlNode<K, V>* current = root;
	AvlNode<K, V>* noavl = root;
	AvlNode<K, V>* p = nullptr, * x = nullptr, * f = nullptr;
	//寻找可以插入的地方
	while (current != nullptr)
	{
		if (current->getKey() == key)
		{
			mtx.unlock();
			return 1;
		}
		if (current->bf != 0)
		{
			noavl = current;
			f = p;

		}
		p = current;
		if (key < current->getKey())
		{
			current = current->lchild;
		}
		else
		{
			current = current->rchild;
		}
	}
	//插入节点
	AvlNode<K, V>* node = createNode(key, value);
	if (key < p->getKey())
	{
		p->lchild = node;
	}
	else
	{
		p->rchild = node;
	}
	size++;

	//修改相关节点平衡因子
	current = noavl;
	while (current != node)
	{
		if (key < current->getKey())
		{
			current->bf++;
			current = current->lchild;
		}
		else
		{
			current->bf--;
			current = current->rchild;
		}
	}
	//不影响平衡性 返回
	if (noavl->bf > -2 && noavl->bf < 2)
	{
		mtx.unlock();
		return 0;
	}
	//左子树不平衡
	if (noavl->bf == 2)
	{
		x = noavl->lchild;
		//节点插入在*noavl左孩子的左子树中
		if (x->bf == 1)
		{
			current = LL_Rotate(noavl);
		}
		//节点插入在*noavl左孩子的右子树中
		else
		{
			current = LR_Rotate(noavl);
		}
	}
	//右子树不平衡
	else
	{
		x = noavl->rchild;
		//节点插入在*noavl右孩子的左子树中
		if (x->bf == 1)
		{
			current = RL_Rotate(noavl);
		}
		//节点插入在*noavl右孩子的右子树中
		else
		{
			current = RR_Rotate(noavl);
		}
	}
	if (f == nullptr) root = current;//原*noavl是AVL树的根
	else if (f->lchild == noavl) f->lchild = current;//将新子树连到原结点*noavl的双亲结点上
	else f->rchild = current;
	mtx.unlock();
	return 0;
}


template<typename K, typename V>
AvlNode<K, V>* AvlTree<K, V>::LL_Rotate(AvlNode<K, V>* noavl)
{
	AvlNode<K, V>* node = noavl->lchild;
	noavl->lchild = node->rchild;
	node->rchild = noavl;
	node->bf = noavl->bf = 0;
	return node;
}

template<typename K, typename V>
AvlNode<K, V>* AvlTree<K, V>::LR_Rotate(AvlNode<K, V>* noavl)
{
	AvlNode<K, V>* a, * current;
	a = noavl->lchild;
	current = a->rchild;
	noavl->lchild = current->rchild;
	a->rchild = current->lchild;
	current->lchild = a;
	current->rchild = noavl;
	if (current->bf == 1)
	{
		noavl->bf = -1;
		a->bf = 0;
	}
	else if(current->bf == -1)
	{
		noavl->bf = 0;
		a->bf = 1;
	}
	else
	{
		noavl->bf = a->bf = 0;
	}
	current->bf = 0;
	return current;
}

template<typename K, typename V>
AvlNode<K, V>* AvlTree<K, V>::RR_Rotate(AvlNode<K, V>* noavl)
{
	AvlNode<K, V>* node = noavl->rchild;
	noavl->rchild = node->lchild;
	node->lchild = noavl;
	node->bf = noavl->bf = 0;
	return node;
}

template<typename K, typename V>
AvlNode<K, V>* AvlTree<K, V>::RL_Rotate(AvlNode<K, V>* noavl)
{
	AvlNode<K, V>* a, * current;
	a = noavl->rchild;
	current = a->lchild;
	noavl->rchild = current->lchild;
	a->lchild = current->rchild;
	current->rchild = a;
	current->lchild = noavl;
	if (current->bf == 1)
	{
		noavl->bf = 0;
		a->bf = -1;
	}
	else if (current->bf == -1)
	{
		noavl->bf = 1;
		a->bf = 0;
	}
	else
	{
		noavl->bf = a->bf = 0;
	}
	current->bf = 0;
	return current;
}

template<typename K, typename V>
void AvlTree<K, V>::display()
{
	AvlNode<K, V>* current = root;
	inorder(current);
}

template<typename K, typename V>
void AvlTree<K, V>::inorder(AvlNode<K,V>* current)
{
	if (current)
	{
		inorder(current->lchild);
		std::cout << " key: " << current->getKey() << " ,value: " << current->getValue() << std::endl;
		inorder(current->rchild);
	}
}

template<typename K, typename V>
void AvlTree<K, V>::deleteAll(AvlNode<K, V>* current)
{
	if (current)
	{
		AvlNode<K, V>* l = current->lchild;
		AvlNode<K, V>* r = current->lchild;
		delete current;
		if (l)
		{
			deleteAll(l);
		}
		if (r)
		{
			deleteAll(r);
		}
	}
}

template<typename K, typename V>
int AvlTree<K, V>::getSize() const
{
	return size;
}

template<typename K, typename V>
AvlNode<K, V>* AvlTree<K, V>::createNode(K key, V value)
{
	AvlNode<K, V>* node = new AvlNode<K, V>(key, value);
	return node;
}