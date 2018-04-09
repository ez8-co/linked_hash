#pragma once

#include <cassert>
#include <iterator>

#ifdef _MSC_VER
	#if _MSC_VER >= 1500
		#include <unordered_set>
		#define lh_hash_set std::tr1::unordered_set
		#define lh_hash_fcn std::tr1::hash
	#else
		#include <hash_set>
		#define lh_hash_set stdext::hash_set
		#define lh_hash_fcn stdext::hash_compare
	#endif
#else
	#ifdef _GXX_EXPERIMENTAL_CXX0X__
		#include <tr1/unordered_set>
		#define lh_hash_set std::tr1::unordered_set
		#define lh_hash_fcn std::tr1::hash
	#elif __cplusplus >= 201103L
		#include <unordered_set>
		#define lh_hash_set std::unordered_set
		#define lh_hash_fcn std::hash
	#else
		#include <hash_set>
		#define lh_hash_set __gnu_cxx::hash_set
		#define lh_hash_fcn __gnu_cxx::hash
	#endif
#endif

using namespace std;


template<class _Kty, class HashFcn = lh_hash_fcn<_Kty> >
class linked_hash_set;

template<class _Kty, class _Ty, class HashFcn = lh_hash_fcn<_Kty> >
class linked_hash_map;


#if !defined(_MSC_VER) && !defined(_GXX_EXPERIMENTAL_CXX0X__) && __cplusplus < 201103L
#include <string>
namespace __gnu_cxx
{
	template<>
	struct hash<string>
	{
		inline size_t operator()(const string& s) const {
			return __stl_hash_string(s.c_str());
		}
	};
}
#endif

template<class value_type>
struct lh_entry;

namespace std
{
	template<class _Kty>
	struct equal_to<lh_entry<_Kty>*>
	{
		inline bool operator()(const lh_entry<_Kty>* lhs, const lh_entry<_Kty>* rhs) const {
			return lhs->val == rhs->val;
		}
	};

	template<class _Kty, class _Ty>
	struct equal_to<lh_entry<std::pair<_Kty, _Ty> >*>
	{
		inline bool operator()(const lh_entry<std::pair<_Kty, _Ty> >* lhs, const lh_entry<std::pair<_Kty, _Ty> >* rhs) const {
			return lhs->val.first == rhs->val.first;
		}
	};
}

template<class value_type>
struct lh_entry
{
	lh_entry(lh_entry* p = NULL, lh_entry* n = NULL) : val() , prev(p), next(n) {}
	lh_entry(const value_type& v, lh_entry* p, lh_entry* n) : val(v), prev(p), next(n) {}

	value_type val;
	lh_entry *prev, *next;
};

template<class _Ty>
struct lh_const_iter : public std::iterator<std::bidirectional_iterator_tag, _Ty>
{
	typedef _Ty value_type;
	typedef value_type& reference;
	typedef value_type* pointer;
	typedef const value_type& const_reference;
	typedef const value_type* const_pointer;

	explicit lh_const_iter(lh_entry<value_type>* ptr) : _ptr(ptr) {}
	const_reference operator*() const { return static_cast<const_reference>(_ptr->val); }
	const_pointer operator->() const  { return(&**this); }
	lh_const_iter& operator++()       { _ptr = _ptr->next; return(*this); }
	lh_const_iter operator++(int)     { lh_const_iter tmp = *this; ++*this; return(tmp); }
	lh_const_iter& operator--()       { _ptr = _ptr->prev; return(*this); }
	lh_const_iter operator--(int)     { lh_const_iter tmp = *this; --*this; return(tmp); }

	bool operator==(const lh_const_iter& rhs) const { return(_ptr == rhs._ptr); }
	bool operator!=(const lh_const_iter& rhs) const { return(!(*this == rhs)); }

protected:
	lh_entry<value_type>* _ptr;
};

template<class _Ty>
struct lh_iter : public lh_const_iter<_Ty>
{
	typedef _Ty value_type;
	typedef value_type& reference;
	typedef value_type* pointer;

	explicit lh_iter(lh_entry<value_type>*const ptr) : lh_const_iter<value_type>(ptr) {}
	reference operator*() const { return((reference)**(lh_const_iter<value_type> *)this); }
	pointer operator->() const  { return(&**this); }
	lh_iter& operator++()       { ++(*(lh_const_iter<value_type> *)this); return(*this); }
	lh_iter& operator--()       { --(*(lh_const_iter<value_type> *)this); return(*this); }
};

template<class _Kty, class HashFcn/* = lh_hash_fcn <_Kty> */>
class linked_hash_set
{
private:
	struct lhs_hasher
	{
		lhs_hasher() : cmp() {}
#ifdef _MSC_VER
		enum {	// parameters for hash table
			bucket_size = 4,	// 0 < bucket_size
			min_buckets = 8
		};	// min_buckets = 2 ^^ N, 0 < N

		inline bool operator()(const lh_entry<_Kty>* lhs, const lh_entry<_Kty>* rhs) const {
			return cmp(lhs->val, rhs->val);
		}
#else
		inline size_t operator()(const lh_entry<_Kty>* entry) const {
			return cmp(entry->val);
		}
#endif

	private:
		HashFcn cmp;
	}; // end class lhs_hasher

public:
	typedef lh_hash_set<lh_entry<_Kty>*, lhs_hasher> _linked_hash_set;
	typedef typename _linked_hash_set::iterator _lhs_iter;
	typedef typename _linked_hash_set::const_iterator _lhs_const_iter;
	typedef typename _linked_hash_set::size_type size_type;
	typedef _Kty key_type;
	typedef _Kty value_type;
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

	linked_hash_set() : _head(&_head, &_head), _lhs() {}
	linked_hash_set(const linked_hash_set& rhs) : _head(&_head, &_head), _lhs() { assign(rhs); }
	linked_hash_set& operator=(const linked_hash_set& rhs) { clear(); assign(rhs); return *this; }
	~linked_hash_set() { clear(); }

	iterator find(const key_type& key) {
		_lhs_iter it = _lhs.find((lh_entry<value_type>*)&key);
		return iterator(it != _lhs.end() ? *it : &_head);
	}

	const_iterator find(const key_type& key) const {
		_lhs_const_iter it = _lhs.find((lh_entry<value_type>*)&key);
		return const_iterator(it != _lhs.end() ? *it : &_head);
	}

	iterator access(const key_type& key);

	_Pairib insert(const key_type& value);

	bool count(const key_type& key) const { return _lhs.count((lh_entry<value_type>*)&key); }

	size_type size() const                { return _lhs.size(); }
	size_type max_size() const            { return _lhs.max_size(); }
	bool empty() const                    { return _lhs.empty(); }

	iterator begin()                      { return iterator(_head.next); }
	const_iterator begin() const          { return const_iterator(_head.next); }
	iterator end()                        { return iterator(&_head); }
	const_iterator end() const            { return const_iterator(&_head); }
	reverse_iterator rbegin()             { return reverse_iterator(end()); }
	const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
	reverse_iterator rend()               { return reverse_iterator(begin()); }
	const_reverse_iterator rend() const   { return const_reverse_iterator(begin()); }

	const _Kty& front()                   { assert(!empty()); return _head.next->val; }
	const _Kty& front() const             { assert(!empty()); return _head.next->val; }
	void pop_front();

	size_type erase(const key_type& key);
	void erase(const_iterator iter)       { erase(*iter); }
	void erase(const_iterator first, const_iterator last);
	void clear();

private:
	void assign(const linked_hash_set& rhs);

	lh_entry<value_type> _head;
	_linked_hash_set _lhs;
}; // end class linked_hash_set

template<class _Kty, class HashFcn>
void
linked_hash_set<_Kty, HashFcn>::assign(const linked_hash_set& rhs)
{
	if(this != &rhs) {
		lh_entry<_Kty>* pos = rhs._head.next;
		while(pos != &rhs._head) {
			lh_entry<_Kty>* entry = new lh_entry<_Kty>(pos->val, _head.prev, &_head);
			_lhs.insert(entry);
			_head.prev->next = entry;
			_head.prev = entry;
			pos = pos->next;
		}
	}
}

template<class _Kty, class HashFcn>
typename linked_hash_set<_Kty, HashFcn>::_Pairib
linked_hash_set<_Kty, HashFcn>::insert(const key_type& value)
{
	_lhs_iter it = _lhs.find((lh_entry<value_type>*)&value);
	if(it == _lhs.end()) {
		lh_entry<value_type>* entry = new lh_entry<value_type>(value, _head.prev, &_head);
		_lhs.insert(entry);
		_head.prev->next = entry;
		_head.prev = entry;
		return std::make_pair(iterator(*it), true);
	}
	return std::make_pair(iterator(&_head), false);
}

template<class _Kty, class HashFcn>
typename linked_hash_set<_Kty, HashFcn>::iterator
linked_hash_set<_Kty, HashFcn>::access(const key_type& key)
{
	_lhs_iter it = _lhs.find((lh_entry<value_type>*)&key);
	if(it != _lhs.end()) {
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

template<class _Kty, class HashFcn>
void
linked_hash_set<_Kty, HashFcn>::pop_front()
{
	assert(!empty());
	lh_entry<value_type>* entry = _head.next;
	_lhs.erase(entry);
	_head.next = entry->next;
	_head.next->prev = &_head;
	delete entry;
}

template<class _Kty, class HashFcn>
typename linked_hash_set<_Kty, HashFcn>::size_type
linked_hash_set<_Kty, HashFcn>::erase(const key_type& key)
{
	_lhs_iter it = _lhs.find((lh_entry<value_type>*)&key);
	if(it != _lhs.end()) {
		// remove it from the link
		(*it)->prev->next =(*it)->next;
		(*it)->next->prev =(*it)->prev;
		lh_entry<_Kty>* entry =(*it);
		_lhs.erase((*it));
		delete entry;
		return 1;
	}
	return 0;
}

template<class _Kty, class HashFcn>
void
linked_hash_set<_Kty, HashFcn>::erase(const_iterator first, const_iterator last)
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

template<class _Kty, class HashFcn>
void
linked_hash_set<_Kty, HashFcn>::clear()
{
	lh_entry<value_type>* pos = _head.next;
	_lhs.clear();
	_head.prev = _head.next = &_head;
	while(pos != &_head) {
		lh_entry<value_type>* next = pos->next;
		delete pos;
		pos = next;
	}
}

template<class _Kty, class _Ty, class HashFcn/* = lh_hash_fcn<_Kty> */>
class linked_hash_map
{
private:
	struct lhm_hasher
	{
		typedef std::pair<_Kty, _Ty> value_type;
		lhm_hasher() : cmp() {}
#ifdef _MSC_VER
		enum {	// parameters for hash table
			bucket_size = 4,	// 0 < bucket_size
			min_buckets = 8
		};	// min_buckets = 2 ^^ N, 0 < N

		inline bool operator()(const lh_entry<value_type>* lhs, const lh_entry<value_type>* rhs) const {
			return cmp(lhs->val.first, rhs->val.first);
		}
#else
		inline size_t operator()(const lh_entry<value_type>* entry) const {
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

	iterator find(const key_type& key) {
		_lhm_iter it = _lhm.find((lh_entry<value_type>*)&key);
		return iterator(it != _lhm.end() ? *it : &_head);
	}

	const_iterator find(const key_type& key) const {
		_lhm_const_iter it = _lhm.find((lh_entry<value_type>*)&key);
		return const_iterator(it != _lhm.end() ? *it : &_head);
	}

	iterator access(const key_type& key);

	_Pairib insert(const value_type& value);

	bool count(const key_type& key) const { return _lhm.count((lh_entry<value_type>*)&key); }

	size_type size() const                { return (_lhm.size()); }
	size_type max_size() const            { return (_lhm.max_size()); }
	bool empty() const                    { return (_lhm.empty()); }

	iterator begin()                      { return iterator(_head.next); }
	const_iterator begin() const          { return const_iterator(_head.next); }
	iterator end()                        { return iterator(&_head); }
	const_iterator end() const            { return const_iterator(&_head); }
	reverse_iterator rbegin()             { return (reverse_iterator(end())); }
	const_reverse_iterator rbegin() const { return (const_reverse_iterator(end())); }
	reverse_iterator rend()               { return (reverse_iterator(begin())); }
	const_reverse_iterator rend() const   { return (const_reverse_iterator(begin())); }

	_Ty& operator [](const key_type& key);

	_Ty& front() { assert(!empty()); return _head.next->val.second; }
	const _Ty& front() const { assert(!empty()); return _head.next->val.second;}
	void pop_front();

	size_type erase(const key_type& key);
	void erase(const_iterator iter)       { erase(iter->first); }
	void erase(const_iterator first, const_iterator last);

	void clear();

private:
	void assign(const linked_hash_map& rhs);

	lh_entry<value_type> _head;
	_linked_hash_map _lhm;
}; // end class linked_hash_map

template<class _Kty, class _Ty, class HashFcn>
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

template<class _Kty, class _Ty, class HashFcn>
typename linked_hash_map<_Kty, _Ty, HashFcn>::_Pairib
linked_hash_map<_Kty, _Ty, HashFcn>::insert(const value_type& value)
{
	_lhm_iter it = _lhm.find((lh_entry<value_type>*)&value);
	if(it == _lhm.end()) {
		lh_entry<value_type>* entry = new lh_entry<value_type>(value, _head.prev, &_head);
		_lhm.insert(entry);
		_head.prev->next = entry;
		_head.prev = entry;
		return std::make_pair(iterator(*it), true);
	}
	return std::make_pair(iterator(&_head), false);
}

template<class _Kty, class _Ty, class HashFcn>
_Ty&
linked_hash_map<_Kty, _Ty, HashFcn>::operator[](const key_type& key)
{
	_lhm_iter it = _lhm.find((lh_entry<value_type>*)&key);
	if(it == _lhm.end()) {
		lh_entry<value_type>* entry = new lh_entry<value_type>(std::make_pair(key, _Ty()), _head.prev, &_head);
		_lhm.insert(entry);
		_head.prev->next = entry;
		_head.prev = entry;
		return entry->val.second;
	}
	return(*it)->val.second;
}

template<class _Kty, class _Ty, class HashFcn>
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

template<class _Kty, class _Ty, class HashFcn>
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

template<class _Kty, class _Ty, class HashFcn>
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

template<class _Kty, class _Ty, class HashFcn>
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

template<class _Kty, class _Ty, class HashFcn>
void
linked_hash_map<_Kty, _Ty, HashFcn>::clear()
{
	lh_entry<value_type>* pos = _head.next;
	_lhm.clear();
	_head.prev = _head.next = &_head;
	while(pos != &_head) {
		lh_entry<value_type>* next = pos->next;
		delete pos;
		pos = next;
	}
}
