#pragma once

#include <cassert>
#include <string>
#include <iterator>
#include <hash_set>
using namespace std;

template <class value_type>
class LinkedHashEntry
{
public:
	LinkedHashEntry ()
		: val ()
		, prev (0)
		, next (0)
	{
	}
	LinkedHashEntry (LinkedHashEntry* p, LinkedHashEntry* n)
		: val ()
		, prev (p)
		, next (n)
	{
	}
	LinkedHashEntry (const value_type& v, LinkedHashEntry* p, LinkedHashEntry* n)
		: val (v)
		, prev (p)
		, next (n)
	{
	}
	value_type val;
	LinkedHashEntry* prev;
	LinkedHashEntry* next;
}; // end class LinkedHashEntry

#ifndef _WIN32
#define stdext __gnu_cxx
namespace stdext
{
	template<>
	struct hash<string>
	{
		inline size_t operator() (const string& s) const
		{
			return __stl_hash_string (s.c_str());
		}
	};
}
#endif

template <class _Ty>
struct LinkedHashConstIter
	: public std::iterator <std::bidirectional_iterator_tag, _Ty>
{
	typedef _Ty value_type;
	typedef value_type& reference;
	typedef value_type* pointer;
	typedef const value_type& const_reference;
	typedef const value_type* const_pointer;

	explicit LinkedHashConstIter (LinkedHashEntry<value_type>* ptr)
		: _ptr (ptr)
	{
	}

	const_reference operator* () const
	{	// return designated value
		return static_cast<const_reference> (_ptr->val);
	}

	const_pointer operator-> () const
	{	// return pointer to class object
		return (&**this);
	}

	LinkedHashConstIter& operator++ ()
	{	// preincrement
		_ptr = _ptr->next;
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
		_ptr = _ptr->prev;
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
	LinkedHashEntry<value_type>* _ptr;	// pointer to node
}; // end class LinkedHashConstIter

template <class _Ty>
struct LinkedHashIter
	: public LinkedHashConstIter<_Ty>
{
	typedef _Ty value_type;
	typedef value_type& reference;
	typedef value_type* pointer;

	explicit LinkedHashIter (LinkedHashEntry<value_type>*const ptr)
		: LinkedHashConstIter<value_type> (ptr)
	{	// construct with node pointer
	}

	reference operator* () const
	{	// return designated value
		return ((reference)**(LinkedHashConstIter<value_type> *)this);
	}

	pointer operator-> () const
	{	// return pointer to class object
		return (&**this);
	}

	LinkedHashIter& operator++ ()
	{	// preincrement
		++(*(LinkedHashConstIter<value_type> *)this);
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
		--(*(LinkedHashConstIter<value_type> *)this);
		return (*this);
	}

	LinkedHashIter operator-- (int)
	{	// postdecrement
		LinkedHashIter tmp = *this;
		--*this;
		return (tmp);
	}
};
