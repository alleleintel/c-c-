#include "common_header.h"

template<typename T, typename Cmp = std::less<T>>
class Heap
{
	using type = unsigned int;
public:
	Heap() 
	{
		arr_.reserve(16);
		arr_.push_back(0xffff);
	}
// 	template<typename It>
// 	Heap(It b, It e)
// 	{
// 		arr_.assign(b,e);
// 	}

	bool insert(const T& t);
	bool remove();
	std::vector<T> arr_;
private:

	type parent(type i) { return i >> 1; };
	type left(type i) { return i << 1; };
	type right(type i) { return 1 + i << 1; };
	void pro_up(type hole);
	void pro_down(type hole, type size);
};

template<typename T, typename Cmp /*= std::less<T>*/>
void Heap<T, Cmp>::pro_down(type hole, type size)
{
	Cmp cmp;
	T tmp = arr_[hole];
	type c = left(hole);
	while (c <= size)
	{
		if (c != size && cmp(arr_[c + 1], arr_[c]))
		{
			++c;
		}
		
		if (cmp(arr_[c], tmp))
		{
			arr_[hole] = arr_[c];
		}
		else
			break;
		hole = c;
		c = left(hole);
	}
	arr_[hole] = tmp;
}

template<typename T, typename Cmp /*= std::less<T>*/>
void Heap<T, Cmp>::pro_up(type hole)
{
	type p = parent(hole);
	Cmp cmp;
	while (p && cmp(arr_[hole], arr_[p]))
	{
		std::swap(arr_[hole], arr_[p]);
		hole = p;
		p = parent(hole);
	}
}

template<typename T, typename Cmp /*= std::less<T>*/>
bool Heap<T, Cmp>::remove()
{
	arr_[1] = arr_.back();
	arr_.pop_back();
	pro_down(1, arr_.size() - 1);
	return true;
}

template<typename T, typename Cmp /*= std::less<T>*/>
bool Heap<T, Cmp>::insert(const T& t)
{
	arr_.push_back(t);
	pro_up(arr_.size() - 1);
	return true;
}
