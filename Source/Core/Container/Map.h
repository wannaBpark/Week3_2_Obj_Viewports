#pragma once
#include <unordered_map>
#include <utility>
#include <algorithm>
#include <vector>
#include "ContainerAllocator.h"
#include "Pair.h"
#include "Serialization/Archive.h"


template <typename KeyType, typename ValueType, typename Allocator = FDefaultAllocator<std::pair<const KeyType, ValueType>>>
class TMap
{
public:
	using PairType = TPair<const KeyType, ValueType>;
	using MapType = std::unordered_map<KeyType, ValueType, std::hash<KeyType>, std::equal_to<KeyType>, Allocator>;
	using SizeType = typename MapType::size_type;
	using Iterator = typename MapType::iterator;
	using ConstIterator = typename MapType::const_iterator;

private:
	MapType PrivateMap;
public:
	// TPair를 반환하는 커스텀 반복자
	Iterator begin() noexcept { return Iterator(PrivateMap.begin()); }
	Iterator end() noexcept { return Iterator(PrivateMap.end()); }
	ConstIterator begin() const noexcept { return ConstIterator(PrivateMap.begin()); }
	ConstIterator end() const noexcept { return ConstIterator(PrivateMap.end()); }

	// 생성자 및 소멸자
	TMap() = default;
	~TMap() = default;

	// 복사 생성자
	TMap(const TMap& Other) : PrivateMap(Other.PrivateMap) {}

	// 이동 생성자
	TMap(TMap&& Other) noexcept : PrivateMap(std::move(Other.PrivateMap)) {}

	// 복사 할당 연산자
	TMap& operator=(const TMap& Other)
	{
		if (this != &Other)
		{
			PrivateMap = Other.PrivateMap;
		}
		return *this;
	}

	// 이동 할당 연산자
	TMap& operator=(TMap&& Other) noexcept
	{
		if (this != &Other)
		{
			PrivateMap = std::move(Other.PrivateMap);
		}
		return *this;
	}

	// 요소 접근 및 수정
	ValueType& operator[](const KeyType& Key)
	{
		return PrivateMap[Key];
	}

	const ValueType& operator[](const KeyType& Key) const
	{
		return PrivateMap.at(Key);
	}

	void Add(const KeyType& Key, const ValueType& Value)
	{
		PrivateMap.insert_or_assign(Key, Value);
	}

	void Emplace(KeyType&& Key, ValueType&& Value)
	{
		PrivateMap.emplace(std::move(Key), std::move(Value));
	}

	void Remove(const KeyType& Key)
	{
		PrivateMap.erase(Key);
	}

	void Empty()
	{
		PrivateMap.clear();
	}

	// 검색 및 조회
	bool Contains(const KeyType& Key) const
	{
		return PrivateMap.find(Key) != PrivateMap.end();
	}

	const ValueType* Find(const KeyType& Key) const
	{
		auto it = PrivateMap.find(Key);
		return it != PrivateMap.end() ? &(it->second) : nullptr;
	}

	ValueType* Find(const KeyType& Key)
	{
		auto it = PrivateMap.find(Key);
		return it != PrivateMap.end() ? &(it->second) : nullptr;
	}

	ValueType& FindOrAdd(const KeyType& Key)
	{
		auto It = PrivateMap.find(Key);
		if (It != PrivateMap.end())
		{
			return It->second;
		}
		else
		{
			// 기본 생성자로 값을 생성하여 삽입
			auto Pair = PrivateMap.emplace(Key, ValueType());
			return Pair.first->second;
		}
	}

	// 크기 관련
	SizeType Num() const
	{
		return PrivateMap.size();
	}

	bool IsEmpty() const
	{
		return PrivateMap.empty();
	}

	// 용량 관련
	void Reserve(SizeType Number)
	{
		PrivateMap.reserve(Number);
	}

	void Compact()
	{
		// 새 버킷 수를 현재 요소 수에 기반해 계산 (기본 해시 로드 팩터 1.0 가정)
		SizeType OptimalBucketCount = PrivateMap.size();

		// 새 map 생성 및 교체 → 요소 복사로 인해 삭제된 슬롯이나 낭비된 메모리 제거
		MapType NewMap;
		NewMap.reserve(OptimalBucketCount);

		for (auto& Pair : PrivateMap)
		{
			NewMap.insert(Pair); // 또는 emplace(Pair)도 가능
		}

		PrivateMap.swap(NewMap); // 기존 맵과 교체
	}

	void Serialize(FArchive& Ar) const
	{
		std::unordered_map<KeyType, ValueType> TempMap(PrivateMap.begin(), PrivateMap.end());
		Ar << TempMap;
	}
	void Deserialize(FArchive& Ar)
	{
		std::unordered_map<KeyType, ValueType> TempMap;
		Ar >> TempMap;
		PrivateMap = MapType(TempMap.begin(), TempMap.end());
	}
};

template<typename KeyType, typename ValueType>
class TMultiMap
{
public:
	using UnderlyingType = std::unordered_multimap<KeyType, ValueType>;
	using KeyInitType = const KeyType&;
	using ValueInitType = const ValueType&;

private:
	UnderlyingType Map;

public:
	TMultiMap() = default;
	TMultiMap(std::initializer_list<std::pair<KeyType, ValueType>> InitList)
	{
		for (const auto& Pair : InitList)
		{
			Add(Pair.first, Pair.second);
		}
	}


	FORCEINLINE void Add(KeyInitType Key, ValueInitType Value)
	{
		Map.emplace(Key, Value);
	}

	FORCEINLINE int32 Remove(KeyInitType Key)
	{
		return static_cast<int32>(Map.erase(Key));
	}

	FORCEINLINE int32 Remove(KeyInitType Key, ValueInitType Value)
	{
		int32 RemovedCount = 0;
		auto Range = Map.equal_range(Key);
		for (auto It = Range.first; It != Range.second;)
		{
			if (It->second == Value)
			{
				It = Map.erase(It);
				++RemovedCount;
			}
			else
			{
				++It;
			}
		}
		return RemovedCount;
	}

	template<typename Allocator = std::allocator<ValueType>>
	void MultiFind(KeyInitType Key, std::vector<ValueType, Allocator>& OutValues) const
	{
		auto Range = Map.equal_range(Key);
		for (auto It = Range.first; It != Range.second; ++It)
		{
			OutValues.push_back(It->second);
		}
	}

	FORCEINLINE const ValueType* FindPair(KeyInitType Key, ValueInitType Value) const
	{
		auto Range = Map.equal_range(Key);
		for (auto It = Range.first; It != Range.second; ++It)
		{
			if (It->second == Value)
			{
				return &It->second;
			}
		}
		return nullptr;
	}

	FORCEINLINE int32 Num(KeyInitType Key) const
	{
		return static_cast<int32>(Map.count(Key));
	}

	FORCEINLINE int32 Num() const
	{
		return static_cast<int32>(Map.size());
	}

	void Reset()
	{
		Map.clear();
	}

	auto begin() const { return Map.begin(); }
	auto end() const { return Map.end(); }

	void Compact()
	{
		UnderlyingType NewMap;
		NewMap.reserve(Map.size());

		for (const auto& Pair : Map)
		{
			NewMap.emplace(Pair);
		}

		Map.swap(NewMap);
	};
};