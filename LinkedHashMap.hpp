#pragma once

#include "LinkedHashStructure.h"

#ifdef __WINDOWS__
template < class _Kty, class _Ty, class HashFcn = stdext::hash_compare <_Kty> >
#else
template < class _Kty, class _Ty, class HashFcn = stdext::hash <_Kty> >
#endif
class LinkedHashMap
{
public:
	typedef _Kty key_type;
	typedef std::pair<_Kty, _Ty> value_type;
	typedef stdext::hash_map <_Kty, LinkedHashEntry<value_type>*, HashFcn> _LinkedHashMap;
	typedef typename _LinkedHashMap::iterator _MyIter;
	typedef typename _LinkedHashMap::const_iterator _MyCIter;
	typedef typename _LinkedHashMap::size_type size_type;
	typedef value_type* pointer;
	typedef value_type& reference;
	typedef const value_type* const_pointer;
	typedef const value_type& const_reference;
	typedef LinkedHashIter<value_type, LinkedHashMap> iterator;
	typedef LinkedHashConstIter<value_type, LinkedHashMap> const_iterator;
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
		_MyIter findIter = _linkedHashMap.find (key);
		if (findIter != _linkedHashMap.end ()) {
			return iterator (&findIter->second->_myValue, this);
		}
		return iterator ();
	}

	const_iterator find (const key_type& key) const
	{
		_MyCIter findIter = _linkedHashMap.find (key);
		if (findIter != _linkedHashMap.end ()) {
			return const_iterator (&findIter->second->_myValue, this);
		}
		return const_iterator ();
	}

	bool exist (const key_type& key) const
	{
		return (_linkedHashMap.find (key) != _linkedHashMap.end ());
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
		return iterator (&_head._after->_myValue, this);
	}

	const_iterator begin (void) const
	{
		return const_iterator (&_head._after->_myValue, this);
	}

	iterator end (void)
	{
		return iterator (0, this);
	}

	const_iterator end (void) const
	{
		return const_iterator (0, this);
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
	friend class LinkedHashConstIter<value_type, LinkedHashMap>;
	void _Inc (pointer& ptr)
	{
		assert (ptr != 0);
		_MyIter findIter = _linkedHashMap.find (ptr->first);
		if (findIter != _linkedHashMap.end ()) {
			ptr = &findIter->second->_after->_myValue;
		}
		else {
			ptr = 0;
		}
	}

	void _Dec (pointer& ptr)
	{
		if (ptr) {
			_MyIter findIter = _linkedHashMap.find (ptr->first);
			ptr = &findIter->second->_before->_myValue;
		}
		else {
			ptr = &_tail->_myValue;
		}
	}

	void assign (const LinkedHashMap& rhs);

	LinkedHashEntry<value_type> _head;
	LinkedHashEntry<value_type>* _tail;
	_LinkedHashMap _linkedHashMap;
}; // end class LinkedHashMap

// public
template <class _Kty, class _Ty, class HashFcn>
LinkedHashMap<_Kty, _Ty, HashFcn>::LinkedHashMap (void)
	: _head ()
	, _tail (&_head)
{
}

template <class _Kty, class _Ty, class HashFcn>
LinkedHashMap<_Kty, _Ty, HashFcn>::LinkedHashMap (const LinkedHashMap& rhs)
	: _head ()
	, _tail (&_head)
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
		LinkedHashEntry<value_type>* pos = rhs._head._after;
		while (pos) {
			LinkedHashEntry<value_type>* entry =
				new LinkedHashEntry<value_type> (pos->_myValue, _tail);
			_linkedHashMap.insert (std::make_pair (pos->_myValue.first, entry));
			_tail->_after = entry;
			_tail = entry;
			pos = pos->_after;
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
	if (!exist (value.first)) {
		LinkedHashEntry<value_type>* entry =
			new LinkedHashEntry<value_type> (value, _tail);
		_linkedHashMap.insert (std::make_pair (value.first, entry));
		_tail->_after = entry;
		_tail = entry;
	}
}

template < class _Kty, class _Ty, class HashFcn>
_Ty&
LinkedHashMap<_Kty, _Ty, HashFcn>::operator[] (const key_type& key)
{
	_MyIter findIter = _linkedHashMap.find (key);
	if (findIter == _linkedHashMap.end ()) {
		LinkedHashEntry<value_type>* entry =
			new LinkedHashEntry<value_type> (std::make_pair (key, _Ty ()), _tail);
		_linkedHashMap.insert (std::make_pair (key, entry));
		_tail->_after = entry;
		_tail = entry;
		return entry->_myValue.second;
	}
	else {
		return findIter->second->_myValue.second;
	}
}

// public
template <class _Kty, class _Ty, class HashFcn>
typename LinkedHashMap<_Kty, _Ty, HashFcn>::iterator
LinkedHashMap<_Kty, _Ty, HashFcn>::access (const key_type& key)
{
	_MyIter findIter = _linkedHashMap.find (key);
	if (findIter != _linkedHashMap.end ()) {
		// is not tail in link
		if (findIter->second->_after) {
			// remove it from the link
			findIter->second->_before->_after = findIter->second->_after;
			findIter->second->_after->_before = findIter->second->_before;

			// relink to tail
			findIter->second->_before = _tail;
			_tail->_after = findIter->second;
			_tail = findIter->second;
			_tail->_after = 0;
		}
		return iterator (&findIter->second->_myValue, this);
	}
	return iterator ();
}

// public
template <class _Kty, class _Ty, class HashFcn>
_Ty&
LinkedHashMap<_Kty, _Ty, HashFcn>::front (void)
{
	assert (_head._after != 0);
	return _head._after->_myValue.second;
}

// public
template <class _Kty, class _Ty, class HashFcn>
const _Ty&
LinkedHashMap<_Kty, _Ty, HashFcn>::front (void) const
{
	assert (_head._after != 0);
	return _head._after->_myValue.second;
}

// public
template <class _Kty, class _Ty, class HashFcn>
void
LinkedHashMap<_Kty, _Ty, HashFcn>::pop_front (void)
{
	assert (_head._after != 0);
	LinkedHashEntry<value_type>* entry = _head._after;
	_linkedHashMap.erase (entry->_myValue.first);
	_head._after = entry->_after;
	// is not tail in link
	if (entry->_after) {
		entry->_after->_before = &_head;
	}
	else {
		_tail = &_head;
	}
	delete entry;
}

// public
template <class _Kty, class _Ty, class HashFcn>
typename LinkedHashMap<_Kty, _Ty, HashFcn>::size_type
LinkedHashMap<_Kty, _Ty, HashFcn>::erase (const key_type& key)
{
	_MyIter findIter = _linkedHashMap.find (key);
	if (findIter != _linkedHashMap.end ()) {
		// is not tail in link
		if (findIter->second->_after) {
			// remove it from the link
			findIter->second->_before->_after = findIter->second->_after;
			findIter->second->_after->_before = findIter->second->_before;
		}
		else {
			_tail = findIter->second->_before;
			_tail->_after = 0;
		}
		LinkedHashEntry<value_type>* entry = findIter->second;
		_linkedHashMap.erase (findIter);
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
	LinkedHashEntry<value_type>* pos = _head._after;
	LinkedHashEntry<value_type>* next = 0;
	_linkedHashMap.clear ();
	_head._after = 0;
	_tail = &_head;
	while (pos) {
		next = pos->_after;
		delete pos;
		pos = next;
	}
}
