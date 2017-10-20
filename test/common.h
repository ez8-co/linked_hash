#include <inttypes.h>
#include <gtest/gtest.h>
#include <cstdio>
#include <ctime>
#include <list>
#include <hash_map>
#include <hash_set>
#include "../LinkedHashSet.hpp"
#include "../LinkedHashMap.hpp"

using namespace std;
using namespace stdext;

#define DATA_COUNT			10000000
#define DATA_RANGE			10000
#define DATA_POP_CNT			(DATA_RANGE / 2)

template <class FUNC_TYPE, class ARG_TYPE>
inline
void
BenchRun (const string& desc, FUNC_TYPE function, ARG_TYPE& arg, uint64_t times)
{
	clock_t start;
	start = clock();
	for (uint64_t i = 0; i < times; ++i) {
		function (arg, i);
	}
	double cost = (double)(clock() - start) / CLOCKS_PER_SEC;
	printf ("[%-18s] Exe times:%"PRId64", Cost time:%.2lf s\n", desc.c_str (), times, (double)cost);
}

template <class FUNC_TYPE, class ARG_TYPE>
inline
void
BenchRun (const string& desc, FUNC_TYPE function, ARG_TYPE& arg)
{
	clock_t start;
	start = clock();
	function (arg);
	double cost = (double)(clock() - start) / CLOCKS_PER_SEC;
	printf ("[%-18s] Cost time:%.2lf s\n", desc.c_str (), (double)cost);
}

template <class FUNC_TYPE, class ARG_TYPE>
inline
void
NormalRun (FUNC_TYPE function, ARG_TYPE& arg, uint64_t times)
{
	for (uint64_t i = 0; i < times; ++i) {
		function (arg, i);
	}
}

extern vector<int>* g_data;

class bench_Environment : public testing::Environment
{
public:
	virtual void SetUp ()
	{
		srand ((unsigned int)time (NULL));
		g_data = new vector<int>;
		g_data->reserve (DATA_COUNT);
		for (int64_t i = 0; i < DATA_COUNT; ++i)
		{
			g_data->push_back (rand () % DATA_RANGE);
		}
		printf ("g_data size:%zd\n", g_data->size ());
	}
	virtual void TearDown ()
	{
		g_data->clear ();
		delete g_data;
	}
};

class st_find_test
{
public:
	template <class T>
	void operator () (T& container, uint64_t i)
	{
		ASSERT_TRUE (container.find ((*g_data)[i]) != container.end ());
	}
};

class st_erase_test
{
public:
	template <class T>
	void operator () (T& container, uint64_t i)
	{
		container.erase ((*g_data)[i]);
	}
};

class st_pop_front_test
{
public:
	template <class T>
	void operator () (LinkedHashSet<T>& container, uint64_t i)
	{
		container.pop_front ();
	}
	template <class T>
	void operator () (LinkedHashMap<T, T>& container, uint64_t i)
	{
		container.pop_front ();
	}
	template <class T>
	void operator () (T& container, uint64_t i)
	{
		container.erase (container.begin ());
	}
};

class st_clear_test
{
public:
	template <class T>
	void operator () (T& container)
	{
		container.clear ();
	}
};

class LRU_list_push_back_test
{
public:
	template <class T>
	void operator () (T& container, uint64_t i)
	{
		container.push_back ((*g_data)[i]);
	}
};

template <class T>
class LRU_dedup_test
{
public:
	void operator () (list<int>& cache)
	{
		T tmp;
		for (list<int>::reverse_iterator iter = cache.rbegin ();
			iter != cache.rend ();
			) {
				typename T::iterator findIter = tmp.find (*iter);
				if (findIter == tmp.end ()) {
					tmp.insert (*iter++);
				}
				else {
					iter = list<int>::reverse_iterator (cache.erase ((++iter).base ()));
				}
		}
	}
};

class LRU_sort_unique_dedup_test
{
public:
	void operator () (list<int>& cache)
	{
		cache.sort ();
		cache.unique ();
	}
};

class LRU_pop_test
{
public:
	void operator () (list<int>& cache)
	{
		int popCnt = DATA_POP_CNT;
		while (popCnt--) {
			cache.pop_front ();
		}
	}
};

class LRU_map_insert_test
{
public:
	template <class T>
	void operator () (T& container, uint64_t i)
	{
		typename T::iterator findIter = container.find ((*g_data)[i]);
		if (findIter == container.end ()) {
			container.insert (make_pair ((*g_data)[i], clock()));
		}
		else {
			findIter->second = clock();
		}
	}
};

template <class T, class K>
class LRU_map_pop_test_aux
{
public:
	LRU_map_pop_test_aux (T& tmp)
		: _tmp (tmp)
	{
	}
	void operator () (K& cache)
	{
		for (typename K::iterator iter = cache.begin ();
			iter != cache.end ();
			++iter) {
				_tmp[iter->second].insert (iter->first);
		}
	}
private:
	T& _tmp;
};

template <class T, class K>
class LRU_map_pop_test_sub
{
public:
	LRU_map_pop_test_sub (K& cache)
		: _cache (cache)
	{
	}
	void operator () (T& tmp)
	{
		int popCnt = DATA_POP_CNT;
		typename T::iterator iter = tmp.begin ();
		while (true) {
			set<int>& vec = iter->second;
			for (set<int>::iterator setIter = vec.begin ();
				setIter != vec.end ();
				++setIter) {
					_cache.erase (*setIter);
					if (--popCnt == 0) {
						return;
					}
			}
			++iter;
		}
	}
private:
	K& _cache;
};

template <class T, class K>
class LRU_map_pop_test
{
public:
	void operator () (K& cache)
	{
		BenchRun (" 1.build aux", LRU_map_pop_test_aux<T, K> (_tmp), cache);
		BenchRun (" 2.cache pop", LRU_map_pop_test_sub<T, K> (cache), _tmp);
	}

private:
	T _tmp;
};
