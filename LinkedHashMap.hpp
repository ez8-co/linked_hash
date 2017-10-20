#pragma once

#include "LinkedHashStructure.h"

namespace std
{
	template<class _Kty, class _Ty>
	struct equal_to<LinkedHashEntry<std::pair<_Kty, _Ty> >*>
	{
		inline bool operator() (const LinkedHashEntry<std::pair<_Kty, _Ty> >* lhs, const LinkedHashEntry<std::pair<_Kty, _Ty> >* rhs) const
		{
			return lhs->val.first == rhs->val.first;
		}
	};
}

#ifdef _WIN32
template < class _Kty, class _Ty, class HashFcn = stdext::hash_compare <_Kty> >
#else
template < class _Kty, class _Ty, class HashFcn = stdext::hash <_Kty> >
#endif
class LinkedHashMap
{
private:
	struct LHMHasher
	{
		typedef std::pair<_Kty, _Ty> value_type;
		LHMHasher (void)
			: comp ()
		{
		}
#ifdef _WIN32
		enum
		{	// parameters for hash table
			bucket_size = 4,	// 0 < bucket_size
			min_buckets = 8
		};	// min_buckets = 2 ^^ N, 0 < N

		inline bool operator()(const LinkedHashEntry<value_type>* lhs, const LinkedHashEntry<value_type>* rhs) const
		{	// test if _Keyval1 ordered before _Keyval2
			return (comp (lhs->val.first, rhs->val.first));
		}
#else
		inline size_t operator() (const LinkedHashEntry<value_type>* entry) const
		{
			return comp (entry->val.first);
		}
#endif

	private:
		HashFcn comp;
	}; // end class LHMHasher

public:
	typedef _Kty key_type;
	typedef std::pair<_Kty, _Ty> value_type;
	typedef stdext::hash_set<LinkedHashEntry<value_type>*, LHMHasher> _LinkedHashMap;
	typedef typename _LinkedHashMap::iterator _MyIter;
	typedef typename _LinkedHashMap::const_iterator _MyCIter;
	typedef typename _LinkedHashMap::size_type size_type;
	typedef value_type* pointer;
	typedef value_type& reference;
	typedef const value_type* const_pointer;
	typedef const value_type& const_reference;
	typedef LinkedHashIter<value_type> iterator;
	typedef LinkedHashConstIter<value_type> const_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
	typedef std::pair<iterator, bool> _Pairib;
	typedef std::pair<iterator, iterator> _Pairii;
	typedef std::pair<const_iterator, const_iterator> _Paircc;

	LinkedHashMap (void);
	LinkedHashMap (const LinkedHashMap& rhs);
	LinkedHashMap& operator= (const LinkedHashMap& rhs);
	~LinkedHashMap (void);

	iterator find (const key_type& key)
	{
		_entry.val.first = key;
		_MyIter it = _linkedHashMap.find (&_entry);
		if (it != _linkedHashMap.end ()) {
			return iterator (*it);
		}
		return iterator (&_head);
	}

	const_iterator find (const key_type& key) const
	{
		_entry.val.first = key;
		_MyCIter it = _linkedHashMap.find (&_entry);
		if (it != _linkedHashMap.end ()) {
			return const_iterator (*it);
		}
		return const_iterator (&_head);
	}

	bool exist (const key_type& key) const
	{
		_entry.val.first = key;
		return (_linkedHashMap.find (&_entry) != _linkedHashMap.end ());
	}

	size_type size (void) const
	{
		return (_linkedHashMap.size ());
	}

	size_type max_size (void) const
	{
		return (_linkedHashMap.max_size());
	}

	bool empty (void) const
	{
		return (_linkedHashMap.empty ());
	}

	iterator begin (void)
	{
		return iterator (_head.next);
	}

	const_iterator begin (void) const
	{
		return const_iterator (_head.next);
	}

	iterator end (void)
	{
		return iterator (&_head);
	}

	const_iterator end (void) const
	{
		return const_iterator (&_head);
	}

	reverse_iterator rbegin (void)
	{
		return (reverse_iterator (end ()));
	}

	const_reverse_iterator rbegin (void) const
	{
		return (const_reverse_iterator (end ()));
	}

	reverse_iterator rend (void)
	{
		return (reverse_iterator (begin ()));
	}

	const_reverse_iterator rend (void) const
	{
		return (const_reverse_iterator (begin ()));
	}

	iterator access (const key_type& key);

	void insert (const value_type& value);

	_Ty& operator [] (const key_type& key);

	_Ty& front (void);

	const _Ty& front (void) const;

	void pop_front (void);

	size_type erase (const key_type& key);

	void erase (const_iterator iter);

	void erase (const_iterator first, const_iterator last);

	void clear (void);

private:
	void assign (const LinkedHashMap& rhs);

	LinkedHashEntry<value_type> _head;
	_LinkedHashMap _linkedHashMap;

	mutable LinkedHashEntry<value_type> _entry;
}; // end class LinkedHashMap

// public
template <class _Kty, class _Ty, class HashFcn>
LinkedHashMap<_Kty, _Ty, HashFcn>::LinkedHashMap (void)
	: _head (&_head, &_head)
	, _linkedHashMap ()
	, _entry ()
{
}

template <class _Kty, class _Ty, class HashFcn>
LinkedHashMap<_Kty, _Ty, HashFcn>::LinkedHashMap (const LinkedHashMap& rhs)
	: _head (&_head, &_head)
	, _linkedHashMap ()
	, _entry ()
{
	assign (rhs);
}

// public
template <class _Kty, class _Ty, class HashFcn>
LinkedHashMap<_Kty, _Ty, HashFcn>&
LinkedHashMap<_Kty, _Ty, HashFcn>::operator= (const LinkedHashMap& rhs)
{
	clear ();
	assign (rhs);
	return *this;
}

// public
template <class _Kty, class _Ty, class HashFcn>
void
LinkedHashMap<_Kty, _Ty, HashFcn>::assign (const LinkedHashMap& rhs)
{
	if (this != &rhs) {
		LinkedHashEntry<value_type>* pos = rhs._head.next;
		while (pos != &rhs._head) {
			LinkedHashEntry<value_type>* entry =
				new LinkedHashEntry<value_type> (pos->val, _head.prev, &_head);
			_linkedHashMap.insert (entry);
			_head.prev->next = entry;
			_head.prev = entry;
			pos = pos->next;
		}
	}
}

// public
template <class _Kty, class _Ty, class HashFcn>
LinkedHashMap<_Kty, _Ty, HashFcn>::~LinkedHashMap (void)
{
	clear ();
}

template < class _Kty, class _Ty, class HashFcn>
void
LinkedHashMap<_Kty, _Ty, HashFcn>::insert (const value_type& value)
{
	_entry.val.first = value.first;
	_MyIter it = _linkedHashMap.find (&_entry);
	if (it == _linkedHashMap.end ()) {
		LinkedHashEntry<value_type>* entry =
			new LinkedHashEntry<value_type> (value, _head.prev, &_head);
		_linkedHashMap.insert (entry);
		_head.prev->next = entry;
		_head.prev = entry;
	}
}

template < class _Kty, class _Ty, class HashFcn>
_Ty&
LinkedHashMap<_Kty, _Ty, HashFcn>::operator[] (const key_type& key)
{
	_entry.val.first = key;
	_MyIter it = _linkedHashMap.find (&_entry);
	if (it == _linkedHashMap.end ()) {
		LinkedHashEntry<value_type>* entry =
			new LinkedHashEntry<value_type> (std::make_pair (key, _Ty ()), _head.prev, &_head);
		_linkedHashMap.insert (entry);
		_head.prev->next = entry;
		_head.prev = entry;
		return entry->val.second;
	}
	else {
		return (*it)->val.second;
	}
}

// public
template <class _Kty, class _Ty, class HashFcn>
typename LinkedHashMap<_Kty, _Ty, HashFcn>::iterator
LinkedHashMap<_Kty, _Ty, HashFcn>::access (const key_type& key)
{
	_entry.val.first = key;
	_MyIter it = _linkedHashMap.find (&_entry);
	if (it != _linkedHashMap.end ()) {
		// remove it from the link
		(*it)->prev->next = (*it)->next;
		(*it)->next->prev = (*it)->prev;

		// relink to tail
		(*it)->prev = _head.prev;
		(*it)->next = &_head;
		_head.prev->next = *it;
		_head.prev = *it;
		return iterator (*it);
	}
	return iterator (&_head);
}

// public
template <class _Kty, class _Ty, class HashFcn>
_Ty&
LinkedHashMap<_Kty, _Ty, HashFcn>::front (void)
{
	assert (_head.next != &_head);
	return _head.next->val.second;
}

// public
template <class _Kty, class _Ty, class HashFcn>
const _Ty&
LinkedHashMap<_Kty, _Ty, HashFcn>::front (void) const
{
	assert (_head.next != &_head);
	return _head.next->val.second;
}

// public
template <class _Kty, class _Ty, class HashFcn>
void
LinkedHashMap<_Kty, _Ty, HashFcn>::pop_front (void)
{
	assert (_head.next != &_head);
	LinkedHashEntry<value_type>* entry = _head.next;
	_linkedHashMap.erase (entry);
	_head.next = entry->next;
	_head.next->prev = &_head;
	delete entry;
}

// public
template <class _Kty, class _Ty, class HashFcn>
typename LinkedHashMap<_Kty, _Ty, HashFcn>::size_type
LinkedHashMap<_Kty, _Ty, HashFcn>::erase (const key_type& key)
{
	_entry.val.first = key;
	_MyIter it = _linkedHashMap.find (&_entry);
	if (it != _linkedHashMap.end ()) {
		// remove it from the link
		(*it)->prev->next = (*it)->next;
		(*it)->next->prev = (*it)->prev;
		LinkedHashEntry<value_type>* entry = *it;
		_linkedHashMap.erase (entry);
		delete entry;
		return 1;
	}
	return 0;
}

// public
template <class _Kty, class _Ty, class HashFcn>
void
LinkedHashMap<_Kty, _Ty, HashFcn>::erase (const_iterator iter)
{
	erase (iter->first);
}

// public
template <class _Kty, class _Ty, class HashFcn>
void
LinkedHashMap<_Kty, _Ty, HashFcn>::erase (const_iterator first, const_iterator last)
{
	if (first == begin () && last == end ()) {
		// erase all
		clear ();
	}
	else {
		const_iterator it = first;
		while (it != last) {
			erase (it++);
		}
	}
}

// public
template <class _Kty, class _Ty, class HashFcn>
void
LinkedHashMap<_Kty, _Ty, HashFcn>::clear (void)
{
	LinkedHashEntry<value_type>* pos = _head.next;
	LinkedHashEntry<value_type>* next = 0;
	_linkedHashMap.clear ();
	_head.prev = _head.next = &_head;
	while (pos != &_head) {
		next = pos->next;
		delete pos;
		pos = next;
	}
}
