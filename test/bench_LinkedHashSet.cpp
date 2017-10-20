#include <gtest/gtest.h>
#include "common.h"

class bench_LinkedHashSet : public testing::Test 
{
protected:
	bench_LinkedHashSet (void)
	{
	}

	virtual ~bench_LinkedHashSet (void)
	{
	}

	static void SetUpTestCase ()
	{
	}

	static void TearDownTestCase ()
	{
	}

	virtual void SetUp (void)
	{
	}

	virtual void TearDown (void)
	{
	}
};

class set_insert_test
{
public:
	template <class T>
	void operator () (T& container, uint64_t i)
	{
		container.insert ((*g_data)[i]);
	}
};

TEST_F (bench_LinkedHashSet, insert)
{
	set<int> testSet;
	BenchRun ("set", set_insert_test (), testSet, DATA_COUNT);

	hash_set<int> testHashSet;
	BenchRun ("hash_set", set_insert_test (), testHashSet, DATA_COUNT);
	ASSERT_EQ (testSet.size (), testHashSet.size ());

	LinkedHashSet<int> testLinkedHashSet;
	BenchRun ("LinkedHashSet", set_insert_test (), testLinkedHashSet, DATA_COUNT);
	ASSERT_EQ (testSet.size (), testLinkedHashSet.size ());
}

TEST_F (bench_LinkedHashSet, find)
{
	set<int> testSet;
	NormalRun (set_insert_test (), testSet, DATA_COUNT);
	BenchRun ("set", st_find_test (), testSet, DATA_COUNT);

	hash_set<int> testHashSet;
	NormalRun (set_insert_test (), testHashSet, DATA_COUNT);
	BenchRun ("hash_set", st_find_test (), testHashSet, DATA_COUNT);

	LinkedHashSet<int> testLinkedHashSet;
	NormalRun (set_insert_test (), testLinkedHashSet, DATA_COUNT);
	BenchRun ("LinkedHashSet", st_find_test (), testLinkedHashSet, DATA_COUNT);
}

TEST_F (bench_LinkedHashSet, erase)
{
	set<int> testSet;
	NormalRun (set_insert_test (), testSet, DATA_COUNT);
	BenchRun ("set", st_erase_test (), testSet, DATA_COUNT);

	hash_set<int> testHashSet;
	NormalRun (set_insert_test (), testHashSet, DATA_COUNT);
	BenchRun ("hash_set", st_erase_test (), testHashSet, DATA_COUNT);

	LinkedHashSet<int> testLinkedHashSet;
	NormalRun (set_insert_test (), testLinkedHashSet, DATA_COUNT);
	BenchRun ("LinkedHashSet", st_erase_test (), testLinkedHashSet, DATA_COUNT);
}

TEST_F (bench_LinkedHashSet, pop_front)
{
	set<int> testSet;
	NormalRun (set_insert_test (), testSet, DATA_COUNT);
	BenchRun ("set", st_pop_front_test (), testSet, testSet.size ());
	ASSERT_TRUE (testSet.empty ());

	hash_set<int> testHashSet;
	NormalRun (set_insert_test (), testHashSet, DATA_COUNT);
	BenchRun ("hash_set", st_pop_front_test (), testHashSet, testHashSet.size ());
	ASSERT_TRUE (testHashSet.empty ());

	LinkedHashSet<int> testLinkedHashSet;
	NormalRun (set_insert_test (), testLinkedHashSet, DATA_COUNT);
	BenchRun ("LinkedHashSet", st_pop_front_test (), testLinkedHashSet, testLinkedHashSet.size ());
	ASSERT_TRUE (testLinkedHashSet.empty ());
}

TEST_F (bench_LinkedHashSet, clear)
{
	set<int> testSet;
	NormalRun (set_insert_test (), testSet, DATA_COUNT);
	BenchRun ("set", st_clear_test (), testSet);
	ASSERT_TRUE (testSet.empty ());

	hash_set<int> testHashSet;
	NormalRun (set_insert_test (), testHashSet, DATA_COUNT);
	BenchRun ("hash_set", st_clear_test (), testHashSet);
	ASSERT_TRUE (testHashSet.empty ());

	LinkedHashSet<int> testLinkedHashSet;
	NormalRun (set_insert_test (), testLinkedHashSet, DATA_COUNT);
	BenchRun ("LinkedHashSet", st_clear_test (), testLinkedHashSet);
	ASSERT_TRUE (testLinkedHashSet.empty ());
}

class LRU_linked_hash_set_insert_test
{
public:
	template <class T>
	void operator () (T& container, uint64_t i)
	{
		typename T::iterator findIter = container.access ((*g_data)[i]);
		if (findIter == container.end ()) {
			container.insert ((*g_data)[i]);
		}
	}
};

class LRU_linked_hash_set_pop_test
{
public:
	template <class T>
	void operator () (T& container)
	{
		int popCnt = DATA_POP_CNT;
		while (popCnt--) {
			container.pop_front ();
		}
	}
};

template <class D, class S>
bool CompareCache_Set (D& d, S& s)
{
	if (d.size () != s. size ()) {
		return false;
	}
	typename D::iterator Diter = d.begin ();
	typename S::iterator Siter = s.begin ();
	for (;
		 Diter != d.end ();
		 ++Diter, ++Siter) {
			 if ((*Diter) != (*Siter)) {
				 return false;
			 }
	}
	return true;
}

template <class D, class S>
bool ReverseCompareCache_Set (D& d, S& s)
{
	if (d.size () != s. size ()) {
		return false;
	}
	typename D::reverse_iterator DReverseIter = d.rbegin ();
	typename S::reverse_iterator SReverseIter = s.rbegin ();
	for (;
		DReverseIter != d.rend ();
		++DReverseIter, ++SReverseIter) {
			if ((*DReverseIter) != (*SReverseIter)) {
				return false;
			}
	}
	return true;
}

template <class D, class S>
bool PopFrontCheck_Set (D& d, S& s)
{
	if (d.size () != s. size ()) {
		return false;
	}
	for (typename D::iterator Diter = d.begin ();
		 Diter != d.end ();
		 ++Diter) {
			if ((*Diter) != s.front ()) {
				return false;
			}
			s.pop_front ();
	}
	return true;
}

TEST_F (bench_LinkedHashSet, LRUModeTest)
{
	printf ("######### list cache with dedup ##########\n");
	list<int> testListWithSet;
	BenchRun ("list cache establish", LRU_list_push_back_test (), testListWithSet, DATA_COUNT);
	BenchRun ("- set dedup", LRU_dedup_test< set<int> > (), testListWithSet);

	list<int> testListWithHashSet;
	NormalRun (LRU_list_push_back_test (), testListWithHashSet, DATA_COUNT);
	BenchRun ("- hash_set dedup", LRU_dedup_test< hash_set<int> > (), testListWithHashSet);
	ASSERT_EQ (testListWithSet.size (), testListWithHashSet.size ());

	list<int> testListSortUnique;
	NormalRun (LRU_list_push_back_test (), testListSortUnique, DATA_COUNT);
	BenchRun ("- sort+unique dedup", LRU_sort_unique_dedup_test (), testListSortUnique);
	ASSERT_EQ (testListWithSet.size (), testListSortUnique.size ());

	BenchRun ("cache pop", LRU_pop_test (), testListWithSet);
	ASSERT_EQ (testListWithSet.size (), DATA_RANGE - DATA_POP_CNT);

	printf ("\n######### map cache with time aux ##########\n");
	map<int, int64_t> testMapWithTimeAuxMap;
	BenchRun ("map cache establish", LRU_map_insert_test (), testMapWithTimeAuxMap, DATA_COUNT);
	BenchRun ("¦² by map", LRU_map_pop_test< map<int64_t, set<int> >, map<int, int64_t> > (), testMapWithTimeAuxMap);
	ASSERT_EQ (testMapWithTimeAuxMap.size (), DATA_RANGE - DATA_POP_CNT);

	hash_map<int, int64_t> testHashMapWithTimeAuxMap;
	BenchRun ("hash_map cache establish", LRU_map_insert_test (), testHashMapWithTimeAuxMap, DATA_COUNT);
	BenchRun ("¦² by hash_map", LRU_map_pop_test< map<int64_t, set<int> >, hash_map<int, int64_t> > (), testHashMapWithTimeAuxMap);
	ASSERT_EQ (testHashMapWithTimeAuxMap.size (), DATA_RANGE - DATA_POP_CNT);

	printf ("\n######### linked hash set cache ##########\n");
	LinkedHashSet<int> testLinkedHashSet;
	BenchRun ("lhs cache establish", LRU_linked_hash_set_insert_test (), testLinkedHashSet, DATA_COUNT);
	BenchRun ("cache pop", LRU_linked_hash_set_pop_test (), testLinkedHashSet);
	ASSERT_EQ (testLinkedHashSet.size (), DATA_RANGE - DATA_POP_CNT);
	ASSERT_TRUE (CompareCache_Set (testListWithSet, testLinkedHashSet));
	ASSERT_TRUE (ReverseCompareCache_Set (testListWithSet, testLinkedHashSet));
	ASSERT_TRUE (PopFrontCheck_Set (testListWithSet, testLinkedHashSet));
}
