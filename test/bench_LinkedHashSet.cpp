#include <gtest/gtest.h>
#include "common.h"

class bench_LinkedHashSet : public testing::Test 
{
protected:
	bench_LinkedHashSet(void)
	{
	}

	virtual ~bench_LinkedHashSet(void)
	{
	}

	static void SetUpTestCase()
	{
	}

	static void TearDownTestCase()
	{
	}

	virtual void SetUp(void)
	{
	}

	virtual void TearDown(void)
	{
	}
};

class set_insert_test
{
public:
	template <class T>
	void operator()(T& container, uint64_t i)
	{
		container.insert((*g_data)[i]);
	}
};

TEST_F(bench_LinkedHashSet, for_each)
{
	set<int> testSet;
	NormalRun(set_insert_test(), testSet, DATA_COUNT);
	BenchRun("set", st_for_each_test(), testSet, testSet.size());

	unordered_set<int> testHashSet;
	NormalRun(set_insert_test(), testHashSet, DATA_COUNT);
	BenchRun("unordered_set", st_for_each_test(), testHashSet, testHashSet.size());

	linked_hash_set<int> testLinkedHashSet;
	NormalRun(set_insert_test(), testLinkedHashSet, DATA_COUNT);
	BenchRun("linked_hash_set", st_for_each_test(), testLinkedHashSet, testLinkedHashSet.size());
}

TEST_F(bench_LinkedHashSet, insert)
{
	set<int> testSet;
	BenchRun("set", set_insert_test(), testSet, DATA_COUNT);

	unordered_set<int> testHashSet;
	BenchRun("unordered_set", set_insert_test(), testHashSet, DATA_COUNT);
	ASSERT_EQ(testSet.size(), testHashSet.size());

	linked_hash_set<int> testLinkedHashSet;
	BenchRun("linked_hash_set", set_insert_test(), testLinkedHashSet, DATA_COUNT);
	ASSERT_EQ(testSet.size(), testLinkedHashSet.size());
}

TEST_F(bench_LinkedHashSet, count)
{
	set<int> testSet;
	NormalRun(set_insert_test(), testSet, DATA_COUNT);
	BenchRun("set", st_count_test(), testSet, DATA_COUNT);

	unordered_set<int> testHashSet;
	NormalRun(set_insert_test(), testHashSet, DATA_COUNT);
	BenchRun("unordered_set", st_count_test(), testHashSet, DATA_COUNT);

	linked_hash_set<int> testLinkedHashSet;
	NormalRun(set_insert_test(), testLinkedHashSet, DATA_COUNT);
	BenchRun("linked_hash_set", st_count_test(), testLinkedHashSet, DATA_COUNT);
}

TEST_F(bench_LinkedHashSet, find)
{
	set<int> testSet;
	NormalRun(set_insert_test(), testSet, DATA_COUNT);
	BenchRun("set", st_find_test(), testSet, DATA_COUNT);

	unordered_set<int> testHashSet;
	NormalRun(set_insert_test(), testHashSet, DATA_COUNT);
	BenchRun("unordered_set", st_find_test(), testHashSet, DATA_COUNT);

	linked_hash_set<int> testLinkedHashSet;
	NormalRun(set_insert_test(), testLinkedHashSet, DATA_COUNT);
	BenchRun("linked_hash_set", st_find_test(), testLinkedHashSet, DATA_COUNT);
}

TEST_F(bench_LinkedHashSet, erase)
{
	set<int> testSet;
	NormalRun(set_insert_test(), testSet, DATA_COUNT);
	BenchRun("set", st_erase_test(), testSet, DATA_COUNT);

	unordered_set<int> testHashSet;
	NormalRun(set_insert_test(), testHashSet, DATA_COUNT);
	BenchRun("unordered_set", st_erase_test(), testHashSet, DATA_COUNT);

	linked_hash_set<int> testLinkedHashSet;
	NormalRun(set_insert_test(), testLinkedHashSet, DATA_COUNT);
	BenchRun("linked_hash_set", st_erase_test(), testLinkedHashSet, DATA_COUNT);
}

TEST_F(bench_LinkedHashSet, pop_front)
{
	set<int> testSet;
	NormalRun(set_insert_test(), testSet, DATA_COUNT);
	BenchRun("set", st_pop_front_test(), testSet, testSet.size());
	ASSERT_TRUE(testSet.empty());

	unordered_set<int> testHashSet;
	NormalRun(set_insert_test(), testHashSet, DATA_COUNT);
	BenchRun("unordered_set", st_pop_front_test(), testHashSet, testHashSet.size());
	ASSERT_TRUE(testHashSet.empty());

	linked_hash_set<int> testLinkedHashSet;
	NormalRun(set_insert_test(), testLinkedHashSet, DATA_COUNT);
	BenchRun("linked_hash_set", st_pop_front_test(), testLinkedHashSet, testLinkedHashSet.size());
	ASSERT_TRUE(testLinkedHashSet.empty());
}

TEST_F(bench_LinkedHashSet, clear)
{
	set<int> testSet;
	NormalRun(set_insert_test(), testSet, DATA_COUNT);
	BenchRun("set", st_clear_test(), testSet);
	ASSERT_TRUE(testSet.empty());

	unordered_set<int> testHashSet;
	NormalRun(set_insert_test(), testHashSet, DATA_COUNT);
	BenchRun("unordered_set", st_clear_test(), testHashSet);
	ASSERT_TRUE(testHashSet.empty());

	linked_hash_set<int> testLinkedHashSet;
	NormalRun(set_insert_test(), testLinkedHashSet, DATA_COUNT);
	BenchRun("linked_hash_set", st_clear_test(), testLinkedHashSet);
	ASSERT_TRUE(testLinkedHashSet.empty());
}

class LRU_linked_hash_set_insert_test
{
public:
	template <class T>
	void operator()(T& container, uint64_t i)
	{
		typename T::iterator findIter = container.access((*g_data)[i]);
		if(findIter == container.end()) {
			container.insert((*g_data)[i]);
		}
	}
};

class LRU_linked_hash_set_pop_test
{
public:
	template <class T>
	void operator()(T& container)
	{
		int popCnt = DATA_POP_CNT;
		while(popCnt--) {
			container.pop_front();
		}
	}
};

template <class D, class S>
bool CompareCache_Set(D& d, S& s)
{
	if(d.size() != s. size()) {
		return false;
	}
	typename D::iterator Diter = d.begin();
	typename S::iterator Siter = s.begin();
	for(;
		 Diter != d.end();
		 ++Diter, ++Siter) {
			 if((*Diter) !=(*Siter)) {
				 return false;
			 }
	}
	return true;
}

template <class D, class S>
bool ReverseCompareCache_Set(D& d, S& s)
{
	if(d.size() != s. size()) {
		return false;
	}
	typename D::reverse_iterator DReverseIter = d.rbegin();
	typename S::reverse_iterator SReverseIter = s.rbegin();
	for(;
		DReverseIter != d.rend();
		++DReverseIter, ++SReverseIter) {
			if((*DReverseIter) !=(*SReverseIter)) {
				return false;
			}
	}
	return true;
}

template <class D, class S>
bool PopFrontCheck_Set(D& d, S& s)
{
	if(d.size() != s. size()) {
		return false;
	}
	for(typename D::iterator Diter = d.begin();
		 Diter != d.end();
		 ++Diter) {
			if((*Diter) != s.front()) {
				return false;
			}
			s.pop_front();
	}
	return true;
}

TEST_F(bench_LinkedHashSet, LRUModeTest)
{
	printf("######### list cache with dedup ##########\n");
	list<int> testListWithSet;
	double cost = BenchRun("==ESTABLISH==", LRU_list_push_back_test(), testListWithSet, DATA_COUNT);
	BenchRun("set dedup", LRU_dedup_test< set<int> >(), testListWithSet, &cost);

	list<int> testListWithHashSet;
	NormalRun(LRU_list_push_back_test(), testListWithHashSet, DATA_COUNT);
	BenchRun("unordered_set dedup", LRU_dedup_test< unordered_set<int> >(), testListWithHashSet, &cost);
	ASSERT_EQ(testListWithSet.size(), testListWithHashSet.size());

	list<int> testListSortUnique;
	NormalRun(LRU_list_push_back_test(), testListSortUnique, DATA_COUNT);
	BenchRun("sort+unique dedup", LRU_sort_unique_dedup_test(), testListSortUnique, &cost);
	ASSERT_EQ(testListWithSet.size(), testListSortUnique.size());

	printf("\n######### map cache with timestamp aux ##########\n");
	map<int, int64_t> testMapWithTimeAuxMap;
	cost = BenchRun("==ESTABLISH==", LRU_map_insert_test(), testMapWithTimeAuxMap, DATA_COUNT);
	BenchRun("map", LRU_map_pop_test< map<int64_t, set<int> >, map<int, int64_t> >(), testMapWithTimeAuxMap, &cost);
	ASSERT_EQ(testMapWithTimeAuxMap.size(), DATA_RANGE - DATA_POP_CNT);

	printf("\n######### unordered_map cache with timestamp aux ##########\n");
	unordered_map<int, int64_t> testHashMapWithTimeAuxMap;
	cost = BenchRun("==ESTABLISH==", LRU_map_insert_test(), testHashMapWithTimeAuxMap, DATA_COUNT);
	BenchRun("unordered_map", LRU_map_pop_test< map<int64_t, set<int> >, unordered_map<int, int64_t> >(), testHashMapWithTimeAuxMap, &cost);
	ASSERT_EQ(testHashMapWithTimeAuxMap.size(), DATA_RANGE - DATA_POP_CNT);

	printf("\n######### linked hash set cache ##########\n");
	linked_hash_set<int> testLinkedHashSet;
	cost = BenchRun("==ESTABLISH==", LRU_linked_hash_set_insert_test(), testLinkedHashSet, DATA_COUNT);
	BenchRun("cache pop", LRU_linked_hash_set_pop_test(), testLinkedHashSet, &cost);
	ASSERT_EQ(testLinkedHashSet.size(), DATA_RANGE - DATA_POP_CNT);
	ASSERT_TRUE(CompareCache_Set(testListWithSet, testLinkedHashSet));
	ASSERT_TRUE(ReverseCompareCache_Set(testListWithSet, testLinkedHashSet));
	ASSERT_TRUE(PopFrontCheck_Set(testListWithSet, testLinkedHashSet));

	printf("\n######### simple list pop benchmark ##########\n");
	list<int> testList;
	cost = BenchRun("==ESTABLISH==", LRU_list_push_back_test(), testList, DATA_COUNT);
	BenchRun("cache pop", LRU_pop_test(DATA_COUNT - testListWithSet.size()), testList, &cost);
	ASSERT_EQ(testList.size(), DATA_RANGE - DATA_POP_CNT);
}
