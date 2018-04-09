#include <gtest/gtest.h>
#include "common.h"

class bench_LinkedHashMap : public testing::Test 
{
protected:
	bench_LinkedHashMap(void)
	{
	}

	virtual ~bench_LinkedHashMap(void)
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

class map_insert_test
{
public:
	template <class T>
	void operator()(T& container, uint64_t i)
	{
		container.insert(make_pair((*g_data)[i],(*g_data)[i]));
	}
};

TEST_F(bench_LinkedHashMap, insert)
{
	map<int, int> testMap;
	BenchRun("map", map_insert_test(), testMap, DATA_COUNT);

	hash_map<int, int> testHashMap;
	BenchRun("hash_map", map_insert_test(), testHashMap, DATA_COUNT);
	ASSERT_EQ(testMap.size(), testHashMap.size());

	linked_hash_map<int, int> testLinkedHashMap;
	BenchRun("linked_hash_map", map_insert_test(), testLinkedHashMap, DATA_COUNT);
	ASSERT_EQ(testMap.size(), testLinkedHashMap.size());
}

TEST_F(bench_LinkedHashMap, find)
{
	map<int, int> testMap;
	NormalRun(map_insert_test(), testMap, DATA_COUNT);
	BenchRun("map", st_find_test(), testMap, DATA_COUNT);

	hash_map<int, int> testHashMap;
	NormalRun(map_insert_test(), testHashMap, DATA_COUNT);
	BenchRun("hash_map", st_find_test(), testHashMap, DATA_COUNT);

	linked_hash_map<int, int> testLinkedHashMap;
	NormalRun(map_insert_test(), testLinkedHashMap, DATA_COUNT);
	BenchRun("linked_hash_map", st_find_test(), testLinkedHashMap, DATA_COUNT);
}

TEST_F(bench_LinkedHashMap, erase)
{
	map<int, int> testMap;
	NormalRun(map_insert_test(), testMap, DATA_COUNT);
	BenchRun("map", st_erase_test(), testMap, DATA_COUNT);

	hash_map<int, int> testHashMap;
	NormalRun(map_insert_test(), testHashMap, DATA_COUNT);
	BenchRun("hash_map", st_erase_test(), testHashMap, DATA_COUNT);

	linked_hash_map<int, int> testLinkedHashMap;
	NormalRun(map_insert_test(), testLinkedHashMap, DATA_COUNT);
	BenchRun("linked_hash_map", st_erase_test(), testLinkedHashMap, DATA_COUNT);
}

TEST_F(bench_LinkedHashMap, pop_front)
{
	map<int, int> testMap;
	NormalRun(map_insert_test(), testMap, DATA_COUNT);
	BenchRun("map", st_pop_front_test(), testMap, testMap.size());
	ASSERT_TRUE(testMap.empty());

	hash_map<int, int> testHashMap;
	NormalRun(map_insert_test(), testHashMap, DATA_COUNT);
	BenchRun("hash_map", st_pop_front_test(), testHashMap, testHashMap.size());
	ASSERT_TRUE(testHashMap.empty());

	linked_hash_map<int, int> testLinkedHashMap;
	NormalRun(map_insert_test(), testLinkedHashMap, DATA_COUNT);
	BenchRun("linked_hash_map", st_pop_front_test(), testLinkedHashMap, testLinkedHashMap.size());
	ASSERT_TRUE(testLinkedHashMap.empty());
}

TEST_F(bench_LinkedHashMap, clear)
{
	map<int, int> testMap;
	NormalRun(map_insert_test(), testMap, DATA_COUNT);
	BenchRun("map", st_clear_test(), testMap);
	ASSERT_TRUE(testMap.empty());

	hash_map<int, int> testHashMap;
	NormalRun(map_insert_test(), testHashMap, DATA_COUNT);
	BenchRun("hash_map", st_clear_test(), testHashMap);
	ASSERT_TRUE(testHashMap.empty());

	linked_hash_map<int, int> testLinkedHashMap;
	NormalRun(map_insert_test(), testLinkedHashMap, DATA_COUNT);
	BenchRun("linked_hash_map", st_clear_test(), testLinkedHashMap);
	ASSERT_TRUE(testLinkedHashMap.empty());
}

class LRU_linked_hash_map_insert_test
{
public:
	template <class T>
	void operator()(T& container, uint64_t i)
	{
		typename T::iterator findIter = container.access((*g_data)[i]);
		if(findIter == container.end()) {
			container.insert(make_pair((*g_data)[i],(*g_data)[i]));
		}
	}
};

class LRU_linked_hash_map_pop_test
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
bool CompareCache_Map(D& d, S& s)
{
	if(d.size() != s. size()) {
		return false;
	}
	typename D::iterator Diter = d.begin();
	typename S::iterator Siter = s.begin();
	for(;
		Diter != d.end() && Siter != s.end();
		++Diter, ++Siter) {
			if((*Diter) != Siter->first) {
				return false;
			}
	}
	return true;
}

template <class D, class S>
bool ReverseCompareCache_Map(D& d, S& s)
{
	if(d.size() != s. size()) {
		return false;
	}
	typename D::reverse_iterator DReverseIter = d.rbegin();
	typename S::reverse_iterator SReverseIter = s.rbegin();
	for(;
		DReverseIter != d.rend() && SReverseIter != s.rend();
		++DReverseIter, ++SReverseIter) {
			if((*DReverseIter) != SReverseIter->first) {
				return false;
			}
	}
	return true;
}

template <class D, class S>
bool PopFrontCheck_Map(D& d, S& s)
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

TEST_F(bench_LinkedHashMap, LRUModeTest)
{
	printf("######### list cache with dedup ##########\n");
	list<int> testListWithSet;
	double cost = BenchRun("==ESTABLISH==", LRU_list_push_back_test(), testListWithSet, DATA_COUNT);
	BenchRun("set dedup", LRU_dedup_test< set<int> >(), testListWithSet, &cost);

	list<int> testListWithHashSet;
	NormalRun(LRU_list_push_back_test(), testListWithHashSet, DATA_COUNT);
	BenchRun("hash_set dedup", LRU_dedup_test< hash_set<int> >(), testListWithHashSet, &cost);
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

	printf("\n######### hash_map cache with timestamp aux ##########\n");
	hash_map<int, int64_t> testHashMapWithTimeAuxMap;
	cost = BenchRun("==ESTABLISH==", LRU_map_insert_test(), testHashMapWithTimeAuxMap, DATA_COUNT);
	BenchRun("hash_map", LRU_map_pop_test< map<int64_t, set<int> >, hash_map<int, int64_t> >(), testHashMapWithTimeAuxMap, &cost);
	ASSERT_EQ(testHashMapWithTimeAuxMap.size(), DATA_RANGE - DATA_POP_CNT);

	printf("\n######### linked hash map cache ##########\n");
	linked_hash_map<int, int> testLinkedHashMap;
	cost = BenchRun("==ESTABLISH==", LRU_linked_hash_map_insert_test(), testLinkedHashMap, DATA_COUNT);
	BenchRun("cache pop", LRU_linked_hash_map_pop_test(), testLinkedHashMap, &cost);
	ASSERT_EQ(testLinkedHashMap.size(), DATA_RANGE - DATA_POP_CNT);
	ASSERT_TRUE(CompareCache_Map(testListWithSet, testLinkedHashMap));
	ASSERT_TRUE(ReverseCompareCache_Map(testListWithSet, testLinkedHashMap));
	ASSERT_TRUE(PopFrontCheck_Map(testListWithSet, testLinkedHashMap));

	printf("\n######### simple list pop benchmark ##########\n");
	list<int> testList;
	cost = BenchRun("==ESTABLISH==", LRU_list_push_back_test(), testList, DATA_COUNT);
	BenchRun("cache pop", LRU_pop_test(DATA_COUNT - testListWithSet.size()), testList, &cost);
	ASSERT_EQ(testList.size(), DATA_RANGE - DATA_POP_CNT);
}
