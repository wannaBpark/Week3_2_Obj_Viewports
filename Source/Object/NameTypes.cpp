#include "./Object/NameTypes.h"

#include "Core/AbstractClass/Singleton.h"
#include "Core/Container/Map.h"
#include "Core/Container/String.h"


enum ENameCase : uint8
{
	IgnoreCase,   // 대소문자 무시
	CaseSensitive // 대소문자 구분
};

/** ANSICAHR나 WIDECHAR를 담는 인터페이스 비슷한 클래스 */
struct FNameStringView
{
	FNameStringView() : Data(nullptr), Len(0), bIsWide(false) {}
	FNameStringView(const ANSICHAR* Str, uint32 InLen) : Ansi(Str), Len(InLen), bIsWide(false) {}
	FNameStringView(const WIDECHAR* Str, uint32 InLen) : Wide(Str), Len(InLen), bIsWide(true) {}
	FNameStringView(const void* InData, uint32 InLen, bool bInIsWide) : Data(InData), Len(InLen), bIsWide(bInIsWide) {}

	union
	{
		const void* Data;
		const ANSICHAR* Ansi;
		const WIDECHAR* Wide;
	};

	uint32 Len;
	bool bIsWide;

	bool IsAnsi() const { return !bIsWide; }
	int32 BytesWithTerminator() const
	{
		return (Len + 1) * (bIsWide ? sizeof(WIDECHAR) : sizeof(ANSICHAR));
	}

	int32 BytesWithoutTerminator() const
	{
		return Len * (bIsWide ? sizeof(WIDECHAR) : sizeof(ANSICHAR));
	}
};

#pragma region Deprecated variables
static constexpr uint32 FNameMaxBlockBits = 13; // Limit block array a bit, still allowing 8k * block size = 1GB - 2G of FName entry data
static constexpr uint32 FNameBlockOffsetBits = 16;
static constexpr uint32 FNameMaxBlocks = 1 << FNameMaxBlockBits;
static constexpr uint32 FNameBlockOffsets = 1 << FNameBlockOffsetBits;
static constexpr uint32 FNameEntryIdBits = FNameBlockOffsetBits + FNameMaxBlockBits;
static constexpr uint32 FNameEntryIdMask = (1 << FNameEntryIdBits) - 1;

#pragma endregion

/** FName의 Hash값을 담는 클래스, FNameEntry에서 사용 */
struct FNameEntryId
{
	uint32 Value = 0;  // 비교 문자열이 있는 해시

	bool IsNone() const { return !Value; }

	bool operator==(const FNameEntryId& Other) const
	{
		return Value == Other.Value;
	}

	bool operator!=(const FNameEntryId& Other) const
	{
		return !(*this == Other);
	}

	explicit operator bool() const noexcept
	{
		return Value != 0;
	}
};

/** Entry에 담기는 Name의 정보 */
struct FNameEntryHeader
{
	uint16 IsWide : 1; // wchar인지 여부
	uint16 Len : 15;   // FName의 길이 0 ~ 32767
};


struct FNameEntry
{
	static constexpr uint32 NAME_SIZE = 256; // FName에 저장될 수 있는 최대 길이

	FNameEntryId ComparisonId; // 비교 문자열이 있는 해시
	FNameEntryHeader Header;   // Name의 정보

	union
	{
		ANSICHAR AnsiName[NAME_SIZE];
		WIDECHAR WideName[NAME_SIZE];
	};

	void StoreName(const ANSICHAR* InName, uint32 Len)
	{
		memcpy(AnsiName, InName, sizeof(ANSICHAR) * Len);
		AnsiName[Len] = '\0';
	}

	void StoreName(const WIDECHAR* InName, uint32 Len)
	{
		memcpy(WideName, InName, sizeof(WIDECHAR) * Len);
		WideName[Len] = '\0';
	}
};

namespace
{
	template <typename CharType>
	uint32 HashString(const CharType* Str)
	{
		// djb2 문자열 해싱 알고리즘
		uint32 Hash = 1234;
		while (*Str)
		{
			Hash = ((Hash << 5) + Hash) + *Str;
			++Str;
		}
		return Hash;
	}

	template <typename CharType>
	uint32 HashStringLower(const CharType* Str, uint32 InLen)
	{
		CharType LowerStr[FNameEntry::NAME_SIZE];
		if constexpr (std::is_same_v<CharType, wchar_t>)
		{
			for (uint32 i = 0; i < InLen; i++)
			{
				LowerStr[i] = towlower(Str[i]);
			}
			LowerStr[InLen] = '\0';
		}
		else
		{
			for (uint32 i = 0; i < InLen; ++i)
			{
				LowerStr[i] = static_cast<CharType>(tolower(Str[i]));
			}
			LowerStr[InLen] = '\0';
		}
		return HashString(LowerStr);
	}

	template <ENameCase Sensitivity>
	uint32 HashName(FNameStringView InName);

	template <>
	uint32 HashName<IgnoreCase>(FNameStringView InName)
	{
		return InName.IsAnsi() ? HashStringLower(InName.Ansi, InName.Len) : HashStringLower(InName.Wide, InName.Len);
	}

	template <>
	uint32 HashName<CaseSensitive>(FNameStringView InName)
	{
		return InName.IsAnsi() ? HashString(InName.Ansi) : HashString(InName.Wide);
	}
}

template <ENameCase Sensitivity>
struct FNameValue
{
	explicit FNameValue(FNameStringView InName)
		: Name(InName)
		, Hash(HashName<Sensitivity>(InName))
	{
	}

	FNameStringView Name;
	uint32 Hash;
	FNameEntryId ComparisonId = {};
};

using FNameComparisonValue = FNameValue<IgnoreCase>;
using FNameDisplayValue = FNameValue<CaseSensitive>;

struct FNamePool : public TSingleton<FNamePool>
{
private:
//#if WITH_CASE_PRESERVING_NAME
//	FNamePoolShard<ENameCase::CaseSensitive> DisplayShards[FNamePoolShards];
//#endif
//	FNamePoolShard<ENameCase::IgnoreCase> ComparisonShards[FNamePoolShards];

	TMap<uint32, FNameEntry> DisplayPool;
	TMap<uint32, FNameEntry> ComparisonPool;

private:
	template <ENameCase Sensitivity>
	FNameEntry MakeEntry(const FNameValue<Sensitivity>& Value) const
	{
		FNameEntry Result;
		Result.ComparisonId = Value.ComparisonId;
		Result.Header = {
			.IsWide = Value.Name.bIsWide,
			.Len = static_cast<uint16>(Value.Name.Len)
		};
		if (Value.Name.bIsWide)
		{
			Result.StoreName(Value.Name.Wide, Value.Name.Len);
		}
		else
		{
			Result.StoreName(Value.Name.Ansi, Value.Name.Len);
		}
		return Result;
	}

public:
	/**
	* @param 해시 값
	* @return 원본 문자열
	*/
	FNameEntry Resolve(uint32 Hash) const
	{
		auto* t = this;
		return *DisplayPool.Find(Hash);
	}

	/**
	 * 문자열을 찾거나, 없으면 Hash화 해서 저장
	 *
	 * @return DisplayName의 Hash
	 */
	FNameEntryId FindOrStoreString(const FNameStringView& Name)
	{
		// DisplayPool에 같은 문자열이 있다면, 문자열의 Hash 반환
		FNameDisplayValue DisplayValue{ Name };
		if (DisplayPool.Find(DisplayValue.Hash))
		{
			return { DisplayValue.Hash };
		}

		const FNameComparisonValue ComparisonValue{ Name };
		if (!ComparisonPool.Find(ComparisonValue.Hash))
		{
			const FNameEntry Entry = MakeEntry(ComparisonValue);
			ComparisonPool.Add(ComparisonValue.Hash, Entry);
		}

		DisplayValue.ComparisonId = { ComparisonValue.Hash };
		DisplayPool.Add(DisplayValue.Hash, MakeEntry(DisplayValue));
		return { DisplayValue.Hash };
	}
};

struct FNameHelper
{
	template <typename CharType>
	static FName MakeFName(const CharType* Str)
	{
		if constexpr (std::is_same_v<CharType, char>)
		{
			return MakeFName(Str, static_cast<uint32>(strlen(Str)));
		}
		else if constexpr (std::is_same_v<CharType, wchar_t>)
		{
			return MakeFName(Str, static_cast<uint32>(wcslen(Str)));
		}
		else
		{
			static_assert(false, "Invalid Character type");
			return {};
		}
	}

	template <typename CharType>
	static FName MakeFName(const CharType* Char, uint32 Len)
	{
		// 문자열의 길이가 NAME_SIZE를 초과하면 None 반환
		if (Len >= FNameEntry::NAME_SIZE)
		{
			return {};
		}

		const FNameEntryId DisplayId = FNamePool::Get().FindOrStoreString({ Char, Len });

		FName Result;
		Result.DisplayIndex = DisplayId.Value;
		Result.ComparisonIndex = ResolveComparisonId(DisplayId).Value;
		return Result;
	}

	static FNameEntryId ResolveComparisonId(FNameEntryId DisplayId)
	{
		if (DisplayId.IsNone())
		{
			return {};
		}
		return FNamePool::Get().Resolve(DisplayId.Value).ComparisonId;
	}
};


FName::FName(const WIDECHAR* Name) :FName(FNameHelper::MakeFName(Name))
{
}

FName::FName(const ANSICHAR* Name)
	: FName(FNameHelper::MakeFName(Name))
{
}

FName::FName(const FString& Name)
	: FName(FNameHelper::MakeFName(*Name, Name.Len()))
{
}

FString FName::ToString() const
{
	if (DisplayIndex == 0 && ComparisonIndex == 0)
	{
		return { TEXT("None") };
	}

	// TODO: WIDECHAR에 대응 해야함
	FNameEntry Entry = FNamePool::Get().Resolve(DisplayIndex);
	return {
		// Entry.Header.IsWide ? Entry.WideName : Entry.AnsiName
		Entry.AnsiName
	};
}

bool FName::operator==(const FName& Other) const
{
	return ComparisonIndex == Other.ComparisonIndex;
}