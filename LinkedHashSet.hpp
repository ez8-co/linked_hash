#pragma once

#include "LinkedHashStructure.h"

#ifdef __WINDOWS__
template < class _Kty, class HashFcn = stdext::hash_compare <_Kty, less<_Kty> > >
#else
template < class _Kty, class HashFcn = stdext::hash <_Kty> >
#endif
class LinkedHashSet
{
public:
	struct LinkedHashSetEntryWrap
	{
		LinkedHashSetEntryWrap (LinkedHashEntry<_Kty>* e)
			: entry (e)
		{
		}
		bool operator==(const LinkedHashSetEntryWrap& rhs) const
		{
			return entry->_myValue == rhs.entry->_myValue;
		}
		LinkedHashEntry<_Kty>* entry;
	}; // end class LinkedHashSetEntryWrap

	struct LinkedHashSetEntryWrapHash
	{
		LinkedHashSetEntryWrapHash (void)
			: comp ()
		{
		}
#ifdef __WINDOWS__
		enum
		{	// parameters for hash table
			bucket_size = 4,	// 0 < bucket_size
			min_buckets = 8};	// min_buckets = 2 ^^ N, 0 < N

		bool operator()(const LinkedHashSetEntryWrap& lhs, const LinkedHashSetEntryWrap& rhs) const
		{	// test if _Keyval1 ordered before _Keyval2
			return (comp (lhs.entry->_myValue, rhs.entry->_myValue));
		}
#endif
		size_t operator() (const LinkedHashSetEntryWrap& wrap) const
		{
			return comp (wrap.entry->_myValue);
		}

	private:
		HashFcn comp;
	}; // end class LinkedHashSetEntryWrapHash

	typedef stdext::hash_set < LinkedHashSetEntryWrap, LinkedHashSetEntryWrapHash > _LinkedHashSet;
	typedef typename _LinkedHashSet::iterator _MyIter;
	typedef typename _LinkedHashSet::const_iterator _MyCIter;
	typedef typename _LinkedHashSet::size_type size_type;
	typedef _Kty key_type;
	typedef _Kty value_type;
	typedef value_type* pointer;
	typedef value_type& reference;
	typedef const value_type* const_pointer;
	typedef const value_type& const_reference;
	typedef LinkedHashIter<value_type, LinkedHashSet> iterator;
	typedef LinkedHashConstIter<value_type, LinkedHashSet> const_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
	typedef std::pair<iterator, bool> _Pairib;
	typedef std::pair<iterator, iterator> _Pairii;
	typedef std::pair<const_iterator, const_iterator> _Paircc;

	LinkedHashSet (void);
	LinkedHashSet (const LinkedHashSet& rhs);
	LinkedHashSet& operator= (const LinkedHashSet& rhs);
	~LinkedHashSet (void);

	iterator find (const key_type& key)
	{
		_entry._myValue = key;
		_MyIter findIter = _linkedHashSet.find (_entryWrap);
		if (findIter != _linkedHashSet.end ()) {
			return iterator (&findIter->entry->_myValue, this);
		}
		return iterator ();
	}

	const_iterator find (const key_type& key) const
	{
		_entry._myValue = key;
		_MyCIter findIter = _linkedHashSet.find (_entryWrap);
		if (findIter != _linkedHashSet.end ()) {
			return const_iterator (&findIter->entry->_myValue, this);
		}
		return const_iterator ();
	}

	bool exist (const key_type& key) const
	{
		_entry._myValue = key;
		return (_linkedHashSet.find (_entryWrap) != _linkedHashSet.end ());
	}

	size_type size (void) const
	{
		return (_linkedHashSet.size ());
	}

	size_type max_size (void) const
	{
		return (_linkedHashSet.max_size());
	}

	bool empty (void) const
	{
		return (_linkedHashSet.empty ());
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

	void insert (const key_type& value);

	const _Kty& front (void);

	const _Kty& front (void) const;

	void pop_front (void);

	size_type erase (const key_type& key);

	void erase (const_iterator iter);

	void erase (const_iterator first, const_iterator last);

	void clear (void);

private:
	friend class LinkedHashConstIter<value_type, LinkedHashSet>;
	void _Inc (pointer& ptr)
	{
		assert (ptr != 0);
		_entry._myValue = *ptr;
		_MyIter findIter = _linkedHashSet.find (_entryWrap);
		if (findIter != _linkedHashSet.end ()) {
			ptr = &findIter->entry->_after->_myValue;
		}
		else {
			ptr = 0;
		}
	}

	void _Dec (pointer& ptr)
	{
		if (ptr) {
			_entry._myValue = *ptr;
			_MyIter findIter = _linkedHashSet.find (_entryWrap);
			ptr = &findIter->entry->_before->_myValue;
		}
		else {
			ptr = &_tail->_myValue;
		}
	}

	void assign (const LinkedHashSet& rhs);

	LinkedHashEntry<_Kty> _head;
	LinkedHashEntry<_Kty>* _tail;
	_LinkedHashSet _linkedHashSet;

	mutable LinkedHashEntry<_Kty> _entry;
	mutable LinkedHashSetEntryWrap _entryWrap;
}; // end class LinkedHashSet// public

template <class _Kty, class HashFcn>
LinkedHashSet<_Kty, HashFcn>::LinkedHashSet (void)
	: _head ()
	, _tail (&_head)
	, _linkedHashSet ()
	, _entry ()
	, _entryWrap (&_entry)
{
}

template <class _Kty, class HashFcn>
LinkedHashSet<_Kty, HashFcn>::LinkedHashSet (const LinkedHashSet& rhs)
	: _head ()
	, _tail (&_head)
	, _linkedHashSet ()
	, _entry ()
	, _entryWrap (&_entry)
{
	assign (rhs);
}

// public
template <class _Kty, class HashFcn>
LinkedHashSet<_Kty, HashFcn>&
LinkedHashSet<_Kty, HashFcn>::operator= (const LinkedHashSet& rhs)
{
	clear ();
	assign (rhs);
	return *this;
}

// public
template <class _Kty, class HashFcn>
void
LinkedHashSet<_Kty, HashFcn>::assign (const LinkedHashSet& rhs)
{
	if (this != &rhs) {
		LinkedHashEntry<_Kty>* pos = rhs._head._after;
		while (pos) {
			LinkedHashEntry<_Kty>* entry =
				new LinkedHashEntry<_Kty> (pos->_myValue, _tail);
			_linkedHashSet.insert (LinkedHashSetEntryWrap (entry));
			_tail->_after = entry;
			_tail = entry;
			pos = pos->_after;
		}
	}
}

// public
template <class _Kty, class HashFcn>
LinkedHashSet<_Kty, HashFcn>::~LinkedHashSet (void)
{
	clear ();
}

template < class _Kty, class HashFcn>
void
LinkedHashSet<_Kty, HashFcn>::insert (const key_type& value)
{
	if (!exist (value)) {
		LinkedHashEntry<_Kty>* entry =
			new LinkedHashEntry<_Kty> (value, _tail);
		_linkedHashSet.insert (LinkedHashSetEntryWrap (entry));
		_tail->_after = entry;
		_tail = entry;
	}
}

// public
template <class _Kty, class HashFcn>
typename LinkedHashSet<_Kty, HashFcn>::iterator
LinkedHashSet<_Kty, HashFcn>::access (const key_type& key)
{
	_entry._myValue = key;
	_MyIter findIter = _linkedHashSet.find (_entryWrap);
	if (findIter != _linkedHashSet.end ()) {
		// is not tail in link
		if (findIter->entry->_after) {
			// remove it from the link
			findIter->entry->_before->_after = findIter->entry->_after;
			findIter->entry->_after->_before = findIter->entry->_before;

			// relink to tail
			findIter->entry->_before = _tail;
			_tail->_after = findIter->entry;
			_tail = findIter->entry;
			_tail->_after = 0;
		}
		return iterator (&findIter->entry->_myValue, this);
	}
	return iterator ();
}

// public
template <class _Kty, class HashFcn>
const _Kty&
LinkedHashSet<_Kty, HashFcn>::front (void)
{
	assert (_head._after != 0);
	return _head._after->_myValue;
}

// public
template <class _Kty, class HashFcn>
const _Kty&
LinkedHashSet<_Kty, HashFcn>::front (void) const
{
	assert (_head._after != 0);
	return _head._after->_myValue;
}

// public
template <class _Kty, class HashFcn>
void
LinkedHashSet<_Kty, HashFcn>::pop_front (void)
{
	assert (_head._after != 0);
	LinkedHashEntry<_Kty>* entry = _head._after;
	LinkedHashSetEntryWrap entryWrap (entry);
	_linkedHashSet.erase (entryWrap);
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
template <class _Kty, class HashFcn>
typename LinkedHashSet<_Kty, HashFcn>::size_type
LinkedHashSet<_Kty, HashFcn>::erase (const key_type& key)
{
	_entry._myValue = key;
	_MyIter findIter = _linkedHashSet.find (_entryWrap);
	if (findIter != _linkedHashSet.end ()) {
		// is not tail in link
		if (findIter->entry->_after) {
			// remove it from the link
			findIter->entry->_before->_after = findIter->entry->_after;
			findIter->entry->_after->_before = findIter->entry->_before;
		}
		else {
			_tail = findIter->entry->_before;
			_tail->_after = 0;
		}
		LinkedHashEntry<_Kty>* entry = findIter->entry;
		_linkedHashSet.erase (findIter);
		delete entry;
		return 1;
	}
	return 0;
}

// public
template <class _Kty, class HashFcn>
void
LinkedHashSet<_Kty, HashFcn>::erase (const_iterator iter)
{
	erase (*iter);
}

// public
template <class _Kty, class HashFcn>
void
LinkedHashSet<_Kty, HashFcn>::erase (const_iterator first, const_iterator last)
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
template <class _Kty, class HashFcn>
void
LinkedHashSet<_Kty, HashFcn>::clear (void)
{
	LinkedHashEntry<_Kty>* pos = _head._after;
	LinkedHashEntry<_Kty>* next = 0;
	_linkedHashSet.clear ();
	_head._after = 0;
	_tail = &_head;
	while (pos) {
		next = pos->_after;
		delete pos;
		pos = next;
	}
}
