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

#if !defined(_MSC_VER) && !defined(_GXX_EXPERIMENTAL_CXX0X__) && __cplusplus < 201103L
#include <string>
namespace __gnu_cxx
{
	template<> struct hash<string>
	{
		inline size_t operator()(const string& s) const { return __stl_hash_string(s.c_str()); }
	};
}
#endif

template <class value_type>
struct lh_entry
{
	lh_entry(lh_entry* p = NULL, lh_entry* n = NULL) : val() , prev(p), next(n) {}
	lh_entry(const value_type& v, lh_entry* p, lh_entry* n) : val(v), prev(p), next(n) {}

	value_type val;
	lh_entry *prev, *next;
};

template <class _Ty>
struct lh_const_iter : public std::iterator <std::bidirectional_iterator_tag, _Ty>
{
	typedef _Ty value_type;
	typedef value_type& reference;
	typedef value_type* pointer;
	typedef const value_type& const_reference;
	typedef const value_type* const_pointer;

	explicit lh_const_iter(lh_entry<value_type>* ptr) : _ptr(ptr) {}
	const_reference operator*() const { return static_cast<const_reference>(_ptr->val); }
	const_pointer operator->() const { return(&**this); }
	lh_const_iter& operator++() { _ptr = _ptr->next; return(*this); }
	lh_const_iter operator++(int) { lh_const_iter tmp = *this; ++*this; return(tmp); }
	lh_const_iter& operator--() { _ptr = _ptr->prev; return(*this); }
	lh_const_iter operator--(int) { lh_const_iter tmp = *this; --*this; return(tmp); }

	bool operator==(const lh_const_iter& rhs) const { return(_ptr == rhs._ptr); }
	bool operator!=(const lh_const_iter& rhs) const { return(!(*this == rhs)); }

protected:
	lh_entry<value_type>* _ptr;
};

template <class _Ty>
struct lh_iter : public lh_const_iter<_Ty>
{
	typedef _Ty value_type;
	typedef value_type& reference;
	typedef value_type* pointer;

	explicit lh_iter(lh_entry<value_type>*const ptr) : lh_const_iter<value_type>(ptr) {}
	reference operator*() const { return((reference)**(lh_const_iter<value_type> *)this); }
	pointer operator->() const { return(&**this); }
	lh_iter& operator++() { ++(*(lh_const_iter<value_type> *)this); return(*this); }
	lh_iter operator++(int) { lh_iter tmp = *this; 	++*this; return(tmp); }
	lh_iter& operator--() { --(*(lh_const_iter<value_type> *)this); return(*this); }
	lh_iter operator--(int) { lh_iter tmp = *this; 	--*this; return(tmp); }
};
