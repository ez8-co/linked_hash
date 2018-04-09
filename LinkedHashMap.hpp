#pragma once

#include "LinkedHashStructure.h"

namespace std
{
	template<class _Kty, class _Ty>
	struct equal_to<lh_entry<std::pair<_Kty, _Ty> >*>
	{
		inline bool operator()(const lh_entry<std::pair<_Kty, _Ty> >* lhs, const lh_entry<std::pair<_Kty, _Ty> >* rhs) const
		{
			return lhs->val.first == rhs->val.first;
		}
	};
}

template < class _Kty, class _Ty, class HashFcn = lh_hash_fcn<_Kty> >
class linked_hash_map
{
private:
	struct lhm_hasher
	{
		typedef std::pair<_Kty, _Ty> value_type;
		lhm_hasher() : cmp() {}
#ifdef _MSC_VER
		enum
		{	// parameters for hash table
			bucket_size = 4,	// 0 < bucket_size
			min_buckets = 8
		};	// min_buckets = 2 ^^ N, 0 < N

		inline bool operator()(const lh_entry<value_type>* lhs, const lh_entry<value_type>* rhs) const
		{
			return(cmp(lhs->val.first, rhs->val.first));
		}
#else
		inline size_t operator()(const lh_entry<value_type>* entry) const
		{
			return cmp(entry->val.first);
		}
#endif

	private:
		HashFcn cmp;
	}; // end class lhm_hasher

public:
	typedef _Kty key_type;
	typedef std::pair<_Kty, _Ty> value_type;
	typedef lh_hash_set<lh_entry<value_type>*, lhm_hasher> _linked_hash_map;
	typedef typename _linked_hash_map::iterator _lhm_iter;
	typedef typename _linked_hash_map::const_iterator _lhm_const_iter;
	typedef typename _linked_hash_map::size_type size_type;
	typedef value_type* pointer;
	typedef value_type& reference;
	typedef const value_type* const_pointer;
	typedef const value_type& const_reference;
	typedef lh_iter<value_type> iterator;
	typedef lh_const_iter<value_type> const_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
	typedef std::pair<iterator, bool> _Pairib;
	typedef std::pair<iterator, iterator> _Pairii;
	typedef std::pair<const_iterator, const_iterator> _Paircc;

	linked_hash_map() : _head(&_head, &_head), _lhm() {}
	linked_hash_map(const linked_hash_map& rhs) : _head(&_head, &_head), _lhm() { assign(rhs); }
	linked_hash_map& operator=(const linked_hash_map& rhs) { clear(); assign(rhs); return *this; }
	~linked_hash_map() { clear(); }

	iterator find(const key_type& key)
	{
		_lhm_iter it = _lhm.find((lh_entry<value_type>*)&key);
		return iterator(it != _lhm.end() ? *it : &_head);
	}

	const_iterator find(const key_type& key) const
	{
		_lhm_const_iter it = _lhm.find((lh_entry<value_type>*)&key);
		return const_iterator(it != _lhm.end() ? *it : &_head);
	}

	bool count(const key_type& key) const
	{
		return _lhm.count((lh_entry<value_type>*)&key);
	}

	size_type size() const { return (_lhm.size()); }
	size_type max_size() const { return (_lhm.max_size()); }
	bool empty() const { return (_lhm.empty()); }

	iterator begin() { return iterator(_head.next); }
	const_iterator begin() const { return const_iterator(_head.next); }
	iterator end() { return iterator(&_head); }
	const_iterator end() const { return const_iterator(&_head); }
	reverse_iterator rbegin() { return (reverse_iterator(end())); }
	const_reverse_iterator rbegin() const { return (const_reverse_iterator(end())); }
	reverse_iterator rend() { return (reverse_iterator(begin())); }
	const_reverse_iterator rend() const { return (const_reverse_iterator(begin())); }

	iterator access(const key_type& key);

	_Pairib insert(const value_type& value);

	_Ty& operator [](const key_type& key);

	_Ty& front() { assert(!empty()); return _head.next->val.second; }
	const _Ty& front() const { assert(!empty()); return _head.next->val.second;}
	void pop_front();

	size_type erase(const key_type& key);

	void erase(const_iterator iter) { erase(iter->first); }

	void erase(const_iterator first, const_iterator last);

	void clear();

private:
	void assign(const linked_hash_map& rhs);

	lh_entry<value_type> _head;
	_linked_hash_map _lhm;
}; // end class linked_hash_map

template <class _Kty, class _Ty, class HashFcn>
void
linked_hash_map<_Kty, _Ty, HashFcn>::assign(const linked_hash_map& rhs)
{
	if(this != &rhs) {
		lh_entry<value_type>* pos = rhs._head.next;
		while(pos != &rhs._head) {
			lh_entry<value_type>* entry = new lh_entry<value_type>(pos->val, _head.prev, &_head);
			_lhm.insert(entry);
			_head.prev->next = entry;
			_head.prev = entry;
			pos = pos->next;
		}
	}
}

template < class _Kty, class _Ty, class HashFcn>
typename linked_hash_map<_Kty, _Ty, HashFcn>::_Pairib
linked_hash_map<_Kty, _Ty, HashFcn>::insert(const value_type& value)
{
	if(!_lhm.count((lh_entry<value_type>*)&value)) {
		lh_entry<value_type>* entry = new lh_entry<value_type>(value, _head.prev, &_head);
		std::pair<_lhm_iter, bool> ib = _lhm.insert(entry);
		_head.prev->next = entry;
		_head.prev = entry;
		return std::make_pair(iterator(*ib.first), true);
	}
	return std::make_pair(iterator(&_head), false);
}

template < class _Kty, class _Ty, class HashFcn>
_Ty&
linked_hash_map<_Kty, _Ty, HashFcn>::operator[](const key_type& key)
{
	_lhm_iter it = _lhm.find((lh_entry<value_type>*)&key);
	if(it == _lhm.end()) {
		lh_entry<value_type>* entry =
			new lh_entry<value_type>(std::make_pair(key, _Ty()), _head.prev, &_head);
		_lhm.insert(entry);
		_head.prev->next = entry;
		_head.prev = entry;
		return entry->val.second;
	}
	else {
		return(*it)->val.second;
	}
}

template <class _Kty, class _Ty, class HashFcn>
typename linked_hash_map<_Kty, _Ty, HashFcn>::iterator
linked_hash_map<_Kty, _Ty, HashFcn>::access(const key_type& key)
{
	_lhm_iter it = _lhm.find((lh_entry<value_type>*)&key);
	if(it != _lhm.end()) {
		// remove it from the link
		(*it)->prev->next =(*it)->next;
		(*it)->next->prev =(*it)->prev;

		// relink to tail
		(*it)->prev = _head.prev;
		(*it)->next = &_head;
		_head.prev->next = *it;
		_head.prev = *it;
		return iterator(*it);
	}
	return iterator(&_head);
}

template <class _Kty, class _Ty, class HashFcn>
void
linked_hash_map<_Kty, _Ty, HashFcn>::pop_front()
{
	assert(!empty());
	lh_entry<value_type>* entry = _head.next;
	_lhm.erase(entry);
	_head.next = entry->next;
	_head.next->prev = &_head;
	delete entry;
}

template <class _Kty, class _Ty, class HashFcn>
typename linked_hash_map<_Kty, _Ty, HashFcn>::size_type
linked_hash_map<_Kty, _Ty, HashFcn>::erase(const key_type& key)
{
	_lhm_iter it = _lhm.find((lh_entry<value_type>*)&key);
	if(it != _lhm.end()) {
		// remove it from the link
		(*it)->prev->next =(*it)->next;
		(*it)->next->prev =(*it)->prev;
		lh_entry<value_type>* entry = *it;
		_lhm.erase(entry);
		delete entry;
		return 1;
	}
	return 0;
}

template <class _Kty, class _Ty, class HashFcn>
void
linked_hash_map<_Kty, _Ty, HashFcn>::erase(const_iterator first, const_iterator last)
{
	if(first == begin() && last == end())
		clear();
	else {
		const_iterator it = first;
		while(it != last) {
			erase(it++);
		}
	}
}

template <class _Kty, class _Ty, class HashFcn>
void
linked_hash_map<_Kty, _Ty, HashFcn>::clear()
{
	lh_entry<value_type>* pos = _head.next;
	lh_entry<value_type>* next = 0;
	_lhm.clear();
	_head.prev = _head.next = &_head;
	while(pos != &_head) {
		next = pos->next;
		delete pos;
		pos = next;
	}
}
