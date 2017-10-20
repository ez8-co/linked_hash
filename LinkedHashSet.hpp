#pragma once

#include "LinkedHashStructure.h"

namespace std
{
	template<class _Kty>
	struct equal_to<LinkedHashEntry<_Kty>*>
	{
		inline bool operator() (const LinkedHashEntry<_Kty>* lhs, const LinkedHashEntry<_Kty>* rhs) const
		{
			return lhs->val == rhs->val;
		}
	};
}

#ifdef _WIN32
template < class _Kty, class HashFcn = stdext::hash_compare <_Kty, less<_Kty> > >
#else
template < class _Kty, class HashFcn = stdext::hash <_Kty> >
#endif
class LinkedHashSet
{
private:
	struct LHSHasher
	{
		LHSHasher (void)
			: comp ()
		{
		}
#ifdef _WIN32
		enum
		{	// parameters for hash table
			bucket_size = 4,	// 0 < bucket_size
			min_buckets = 8
		};	// min_buckets = 2 ^^ N, 0 < N

		inline bool operator()(const LinkedHashEntry<_Kty>* lhs, const LinkedHashEntry<_Kty>* rhs) const
		{	// test if _Keyval1 ordered before _Keyval2
			return (comp (lhs->val, rhs->val));
		}
#else
		inline size_t operator() (const LinkedHashEntry<_Kty>* entry) const
		{
			return comp (entry->val);
		}
#endif

	private:
		HashFcn comp;
	}; // end class LHSHasher

public:
	typedef stdext::hash_set<LinkedHashEntry<_Kty>*, LHSHasher> _LinkedHashSet;
	typedef typename _LinkedHashSet::iterator _MyIter;
	typedef typename _LinkedHashSet::const_iterator _MyCIter;
	typedef typename _LinkedHashSet::size_type size_type;
	typedef _Kty key_type;
	typedef _Kty value_type;
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

	LinkedHashSet (void);
	LinkedHashSet (const LinkedHashSet& rhs);
	LinkedHashSet& operator= (const LinkedHashSet& rhs);
	~LinkedHashSet (void);

	iterator find (const key_type& key)
	{
		_entry.val = key;
		_MyIter it = _linkedHashSet.find (&_entry);
		if (it != _linkedHashSet.end ()) {
			return iterator (*it);
		}
		return iterator (&_head);
	}

	const_iterator find (const key_type& key) const
	{
		_entry.val = key;
		_MyCIter it = _linkedHashSet.find (&_entry);
		if (it != _linkedHashSet.end ()) {
			return const_iterator (*it);
		}
		return const_iterator (&_head);
	}

	bool exist (const key_type& key) const
	{
		_entry.val = key;
		return (_linkedHashSet.find (&_entry) != _linkedHashSet.end ());
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

	void insert (const key_type& value);

	const _Kty& front (void);

	const _Kty& front (void) const;

	void pop_front (void);

	size_type erase (const key_type& key);

	void erase (const_iterator iter);

	void erase (const_iterator first, const_iterator last);

	void clear (void);

private:
	void assign (const LinkedHashSet& rhs);

	LinkedHashEntry<value_type> _head;
	_LinkedHashSet _linkedHashSet;

	mutable LinkedHashEntry<value_type> _entry;
}; // end class LinkedHashSet// public

template <class _Kty, class HashFcn>
LinkedHashSet<_Kty, HashFcn>::LinkedHashSet (void)
	: _head (&_head, &_head)
	, _linkedHashSet ()
	, _entry ()
{
}

template <class _Kty, class HashFcn>
LinkedHashSet<_Kty, HashFcn>::LinkedHashSet (const LinkedHashSet& rhs)
	: _head (&_head, &_head)
	, _linkedHashSet ()
	, _entry ()
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
		LinkedHashEntry<_Kty>* pos = rhs._head.next;
		while (pos != &rhs._head) {
			LinkedHashEntry<_Kty>* entry =
				new LinkedHashEntry<_Kty> (pos->val, _head.prev, &_head);
			_linkedHashSet.insert (entry);
			_head.prev->next = entry;
			_head.prev = entry;
			pos = pos->next;
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
			new LinkedHashEntry<_Kty> (value, _head.prev, &_head);
		_linkedHashSet.insert (entry);
		_head.prev->next = entry;
		_head.prev = entry;
	}
}

// public
template <class _Kty, class HashFcn>
typename LinkedHashSet<_Kty, HashFcn>::iterator
LinkedHashSet<_Kty, HashFcn>::access (const key_type& key)
{
	_entry.val = key;
	_MyIter it = _linkedHashSet.find (&_entry);
	if (it != _linkedHashSet.end ()) {
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
template <class _Kty, class HashFcn>
const _Kty&
LinkedHashSet<_Kty, HashFcn>::front (void)
{
	assert (_head.next != &_head);
	return _head.next->val;
}

// public
template <class _Kty, class HashFcn>
const _Kty&
LinkedHashSet<_Kty, HashFcn>::front (void) const
{
	assert (_head.next != &_head);
	return _head.next->val;
}

// public
template <class _Kty, class HashFcn>
void
LinkedHashSet<_Kty, HashFcn>::pop_front (void)
{
	assert (_head.next != &_head);
	LinkedHashEntry<_Kty>* entry = _head.next;
	_linkedHashSet.erase (entry);
	_head.next = entry->next;
	_head.next->prev = &_head;
	delete entry;
}

// public
template <class _Kty, class HashFcn>
typename LinkedHashSet<_Kty, HashFcn>::size_type
LinkedHashSet<_Kty, HashFcn>::erase (const key_type& key)
{
	_entry.val = key;
	_MyIter it = _linkedHashSet.find (&_entry);
	if (it != _linkedHashSet.end ()) {
		// remove it from the link
		(*it)->prev->next = (*it)->next;
		(*it)->next->prev = (*it)->prev;
		LinkedHashEntry<_Kty>* entry = (*it);
		_linkedHashSet.erase ((*it));
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
	LinkedHashEntry<_Kty>* pos = _head.next;
	LinkedHashEntry<_Kty>* next = 0;
	_linkedHashSet.clear ();
	_head.prev = _head.next = &_head;
	while (pos != &_head) {
		next = pos->next;
		delete pos;
		pos = next;
	}
}
