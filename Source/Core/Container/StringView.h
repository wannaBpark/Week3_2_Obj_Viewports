#pragma once
#include "climits"
#include "String.h"


class FStringView
{
public:
	using ElementType = TCHAR;
	using ViewType = const FStringView;

public:
	constexpr FStringView() = default;

	constexpr FStringView(const ElementType* InString, const int32 InStringLength)
		: DataPtr(InString), Size(InStringLength)
	{
	}

	constexpr FStringView(const ElementType* InString)
		: DataPtr(InString), Size(InString ? TCString<ElementType>::Strlen((const ElementType*)InString) : 0)
	{
	}

	inline const ElementType& operator[](int32 Index) const;

	[[nodiscard]] constexpr inline const ElementType* GetData() const { return DataPtr; }

	/** @returns Number of bytes used for the characters in the string view */
	[[nodiscard]] constexpr inline SIZE_T NumBytes() const { return static_cast<SIZE_T>(Size) * sizeof(ElementType); }

	/** Returns the length of the string view. */
	[[nodiscard]] constexpr inline int32 Len() const { return Size; }

	/** Returns whether the string view is empty. */
	[[nodiscard]] constexpr inline bool IsEmpty() const { return Size == 0; }

	inline void RemovePrefix(int32 CharCount) { DataPtr += CharCount; Size -= CharCount; }
	inline void RemoveSuffix(int32 CharCount) { Size -= CharCount; }
	inline void Reset() { DataPtr = nullptr; Size = 0; }

	inline int32 CopyString(ElementType* Dest, int32 CharCount, int32 Position = 0) const;

	/** Returns the left-most part of the view by taking the given number of characters from the left. */
	[[nodiscard]] inline ViewType Left(int32 CharCount) const;
	/** Returns the left-most part of the view by chopping the given number of characters from the right. */
	[[nodiscard]] inline ViewType LeftChop(int32 CharCount) const;
	/** Returns the right-most part of the view by taking the given number of characters from the right. */
	[[nodiscard]] inline ViewType Right(int32 CharCount) const;
	/** Returns the right-most part of the view by chopping the given number of characters from the left. */
	[[nodiscard]] inline ViewType RightChop(int32 CharCount) const;
	/** Returns the middle part of the view by taking up to the given number of characters from the given position. */
	[[nodiscard]] inline ViewType Mid(int32 Position, int32 CharCount = MAXINT32) const;
	/** Returns the middle part of the view between any whitespace at the start and end. */
	[[nodiscard]] inline ViewType TrimStartAndEnd() const;
	/** Returns the right part of the view after any whitespace at the start. */
	[[nodiscard]] inline ViewType TrimStart() const;
	/** Returns the left part of the view before any whitespace at the end. */
	[[nodiscard]] inline ViewType TrimEnd() const;

	/** Modifies the view to be the given number of characters from the left. */
	inline void LeftInline(int32 CharCount) { *this = Left(CharCount); }
	/** Modifies the view by chopping the given number of characters from the right. */
	inline void LeftChopInline(int32 CharCount) { *this = LeftChop(CharCount); }
	/** Modifies the view to be the given number of characters from the right. */
	inline void RightInline(int32 CharCount) { *this = Right(CharCount); }
	/** Modifies the view by chopping the given number of characters from the left. */
	inline void RightChopInline(int32 CharCount) { *this = RightChop(CharCount); }
	/** Modifies the view to be the middle part by taking up to the given number of characters from the given position. */
	inline void MidInline(int32 Position, int32 CharCount = MAXINT32) { *this = Mid(Position, CharCount); }
	/** Modifies the view to be the middle part between any whitespace at the start and end. */
	inline void TrimStartAndEndInline() { *this = TrimStartAndEnd(); }
	/** Modifies the view to be the right part after any whitespace at the start. */
	inline void TrimStartInline() { *this = TrimStart(); }
	/** Modifies the view to be the left part before any whitespace at the end. */
	inline void TrimEndInline() { *this = TrimEnd(); }

	// Comparison
	//template <typename OtherRangeType, decltype(MakeStringView(std::declval<OtherRangeType>())) = nullptr>
	//inline bool Equals(OtherRangeType&& Other, ESearchCase::Type SearchCase = ESearchCase::CaseSensitive) const;
	//{
	//	const auto OtherView = MakeStringVeiw(std::forward<OtherRangeType>(Other));
	//	return Len() == OtherView.Len() && Compare(OhterView, SearchCase) == 0;
	//}

	//inline bool Equals(ViewType Other, ESearchCase::Type SearchCase = ESearchCase::CaseSensitive) const;
	//{
	//	return PrivateEquals(Other, SearchCase);
	//}

	//inline bool Equals(ViewType* Other, ESearchCase::Type SearchCase = ESearchCase::CaseSensitive) const
	//{
	//	return Equals(Other, SearchCase);
	//}

	//inline bool Compare(ViewType&& Other, ESearchCase::Type SearchCase = ESearchCase::CaseSensitive) const
	//{
	//	return Compare(MakeStringView(Forward<OtherRangeType>(Other)), SearchCase);
	//}

	//inline bool Compare(ViewType Other, ESearchCase::Type SearchCase = ESearchCase::CaseSensitive) const
	//{
	//	return 
	//}

protected:
	const ElementType* DataPtr = nullptr;
	int32 Size = 0;
};

template <typename CharPtrOrRangeType>
constexpr inline auto MakeStringView(CharPtrOrRangeType&& CharPtrOrRange) -> decltype(FStringView(std::forward<CharPtrOrRangeType>(CharPtrOrRange)))
{
	return FStringView{std::forward< CharPtrOrRangeType>(CharPtrOrRange)};
}

template <typename CharPtrType>
constexpr inline auto MakeStringVeiw(CharPtrType&& CharPtr, int32 Size) -> decltype(FStringView(std::forward<CharPtrType>(CharPtr), Size))
{
	return FStringView(std::forward<CharPtrType>(CharPtr), Size);
}

inline FStringView operator ""_PrivateSV(const TCHAR * String, size_t Size)
{
	return FStringView(String, Size);
}


