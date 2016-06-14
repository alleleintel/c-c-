#include <iostream>
using std::cout;

template<typename T>
class BSTree
{
public:
	BSTree();
	~BSTree();

	void Insert(const T& t);
	void Remove(const T& t);
	bool Max(T& t);
	bool Min(T& t);
	bool Contains(const T& t);
private:
	class Node
	{
	public:
		Node(const T& n, Node* l, Node* r);
		Node();
		~Node() { std::cout << "destor " << data << '\n'; }
		T data;
		Node* left;
		Node* right;
		Node* & getLeft() { return left; };
		Node* & getRight() { return right; };
	};

	Node* root;
	void insert(const T& t, Node** root);
	void make_empty();
	void make_empty(Node*& root);
	Node* find_max(Node* r);
	Node* find_min(Node* r);
	void remove(const T& t, Node*& r);
};

template<typename T>
BSTree<T>::Node::Node() :left(nullptr), right(nullptr)
{}

template<typename T>
BSTree<T>::Node::Node(const T& n, Node* l, Node* r)
	: data(n), left(l), right(r)
{}

template<typename T>
BSTree<T>::BSTree()
	: root(nullptr)
{}

template<typename T>
BSTree<T>::~BSTree()
{
	make_empty();
}

template<typename T>
void BSTree<T>::remove(const T& t, Node*& r)
{
	if (r)
	{
		if (t < r->data)
		{
			remove(t, r->left);
		}
		else if (t > r->data)
		{
			remove(t, r->right);
		}
		else
		{
			if (r->left && r->right)
			{
				r->data = find_min(r->right)->data;
				remove(r->data, r->right);
			}
			else
			{
				Node* tmp = r;
				r = r->left ? r->left : r->right;
				delete tmp;
				tmp = nullptr;
			}
		}
	}
}

template<typename T>
void BSTree<T>::Remove(const T& t)
{
	remove(t, root);
}

template<typename T>
bool BSTree<T>::Contains(const T& t)
{
	Node* p = root;
	while (p)
	{
		if (t < p->data)
		{
			p = p->left;
		}
		else if (t > p->data)
		{
			p = p->right;
		}
		else
		{
			return true;
		}
	}
	return false;
}

template<typename T>
typename BSTree<T>::Node* BSTree<T>::find_min(typename BSTree<T>::Node* r)
{
	if (r)
	{
		while (r->left)
		{
			r = r->left;
		}
	}
	return r;
}

template<typename T>
typename BSTree<T>::Node* BSTree<T>::find_max(typename BSTree<T>::Node* r)
{
	if (r)
	{
		while (r->right)
		{
			r = r->right;
		}
	}
	return r;
}


template<typename T>
bool BSTree<T>::Max(T& t)
{
	Node* p = find_max(root);
	if (p)
	{
		t = p->data;
		return true;
	}
	return false;
}

template<typename T>
bool BSTree<T>::Min(T& t)
{
	Node* p = find_min(root);
	if (p)
	{
		t = p->data;
		return true;
	}
	return false;
}

template<typename T>
void BSTree<T>::make_empty(Node*& r)
{
	if (r)
	{
		make_empty(r->left);
		make_empty(r->right);
		delete r;
	}
	r = nullptr;
}

template<typename T>
void BSTree<T>::make_empty()
{
	make_empty(root);
}

template<typename T>
void BSTree<T>::insert(const T& t, Node** root)
{
	while (*root)
	{
		if (t < (*root)->data)
		{
			root = &(*root)->left;
		}
		else if (t >(*root)->data)
		{
			root = &(*root)->right;
		}
		else
		{
			return;
		}
	}
	*root = new Node(t, nullptr, nullptr);
}

template<typename T>
void BSTree<T>::Insert(const T& t)
{
	insert(t, &root);
}
