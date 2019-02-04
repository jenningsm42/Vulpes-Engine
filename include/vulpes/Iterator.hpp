#ifndef _VUL_ITERATOR_HPP
#define _VUL_ITERATOR_HPP
#include <vector>

template <class T>
class Iterator
{
public:
	Iterator();
	~Iterator();

	T next();

private:
	std::vector<T*> ptrs;
};

#endif // _VUL_ITERATOR_HPP