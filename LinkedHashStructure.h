#pragma once

#include <cassert>

#ifndef __WINDOWS__
namespace stdext
{
	template<>
	struct hash<string>
	{
		size_t operator() (const string& s) const
		{
			return __stl_hash_string (s.c_str());
		}
	};
}
#endif

template <class value_type>
struct LinkedHashEntry
{
public:
	LinkedHashEntry (void)
		: _myValue ()
		, _before (0)
		, _after (0)
	{
	}
	LinkedHashEntry (const value_type& value, LinkedHashEntry* b = 0)
		: _myValue (value)
		, _before (b)
		, _after (0)
	{
	}
	value_type _myValue;
	LinkedHashEntry* _before;
	LinkedHashEntry* _after;
}; // end class LinkedHashEntry

template <class _Ty, class _ListType>
struct LinkedHashConstIter
	: public std::iterator <std::bidirectional_iterator_tag, _Ty>
{
public:
	typedef _Ty value_type;
	typedef value_type& reference;
	typedef value_type* pointer;
	typedef const value_type& const_reference;
	typedef const value_type* const_pointer;

	LinkedHashConstIter (void)
		: _ptr (0)
		, _list (0)
	{
	}

	explicit LinkedHashConstIter (pointer ptr, const _ListType * const list)
		: _ptr (ptr)
		, _list (const_cast<_ListType*> (list))
	{
	}

	const_reference operator* () const
	{	// return designated value
		return static_cast<const_reference> (*_ptr);
	}

	const_pointer operator-> () const
	{	// return pointer to class object
		return (&**this);
	}

	LinkedHashConstIter& operator++ ()
	{	// preincrement
		assert (_ptr != 0);
		_list->_Inc (_ptr);
		return (*this);
	}

	LinkedHashConstIter operator++ (int)
	{	// postincrement
		LinkedHashConstIter tmp = *this;
		++*this;
		return (tmp);
	}

	LinkedHashConstIter& operator-- ()
	{	// predecrement
		_list->_Dec (_ptr);
		return (*this);
	}

	LinkedHashConstIter operator-- (int)
	{	// postdecrement
		LinkedHashConstIter tmp = *this;
		--*this;
		return (tmp);
	}

	bool operator== (const LinkedHashConstIter& rhs) const
	{	// test for iterator equality
		return (_ptr == rhs._ptr);
	}

	bool operator!= (const LinkedHashConstIter& rhs) const
	{	// test for iterator inequality
		return (!(*this == rhs));
	}

protected:
	pointer _ptr;	// pointer to node
	_ListType* _list;
}; // end class LinkedHashConstIter

template <class _Ty, class _ListType>
class LinkedHashIter
	: public LinkedHashConstIter<_Ty, _ListType>
{
public:
	typedef _Ty value_type;
	typedef value_type& reference;
	typedef value_type* pointer;

	LinkedHashIter (void)
	{	// construct with null node pointer
	}

	explicit LinkedHashIter (pointer ptr, _ListType* list)
		: LinkedHashConstIter<value_type, _ListType> (ptr, list)
	{	// construct with node pointer _Pnode
	}

	reference operator* () const
	{	// return designated value
		return ((reference)**(LinkedHashConstIter<value_type, _ListType> *)this);
	}

	pointer operator-> () const
	{	// return pointer to class object
		return (&**this);
	}

	LinkedHashIter& operator++ ()
	{	// preincrement
		++(*(LinkedHashConstIter<value_type, _ListType> *)this);
		return (*this);
	}

	LinkedHashIter operator++ (int)
	{	// postincrement
		LinkedHashIter tmp = *this;
		++*this;
		return (tmp);
	}

	LinkedHashIter& operator-- ()
	{	// predecrement
		--(*(LinkedHashConstIter<value_type, _ListType> *)this);
		return (*this);
	}

	LinkedHashIter operator-- (int)
	{	// postdecrement
		LinkedHashIter tmp = *this;
		--*this;
		return (tmp);
	}
};
