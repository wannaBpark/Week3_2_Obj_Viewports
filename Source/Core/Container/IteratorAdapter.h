#pragma once

template <typename Base>
class TIteratorAdapter;
/**
 * 이터레이터 구현을 위한 어댑터 클래스입니다.
 *
 * 이 클래스는 최소한의 구현 요구사항만으로 UE(Unreal Engine) 호환 이터레이터를 구현할 수 있는 인터페이스를 제공합니다.
 * 이를 통해 사용자는 커스텀 이터레이터를 보다 쉽게 구현할 수 있습니다.
 *
 * 이 어댑터 클래스는 CRTP(Curiously Recurring Template Pattern)를 사용하며, 기본 클래스(Base)가 다음과 같은 항목들을 구현해야 합니다:
 *
 * [타입]
 *   - ElementType: 컨테이너에서 사용되는 요소의 타입.
 *   - SizeType: 컨테이너의 인덱싱 및 이터레이터의 오프셋에 사용되는 타입.
 *
 * [메소드]
 *   - bool Equals(const IteratorType& other): 다른 이터레이터와의 동등성 비교를 수행합니다.
 *   - ElementType& Dereference() const: 현재 이터레이터가 가리키는 요소에 대한 참조를 반환합니다.
 *   - void Increment(): 이터레이터를 다음 위치로 이동시킵니다.
 *
 * [선택적 메소드]
 *   - void Decrement(): 이터레이터를 이전 위치로 이동시킬 수 있습니다.
 */

template <typename Base>
class TIteratorAdapter : public Base
{
public:
	using BaseType = Base;
	using ThisType = TIteratorAdapter<Base>;
	using ElementType = typename BaseType::ElementType;
	using SizeType = typename BaseType::SizeType;

	TIteratorAdapter() = default;

	/*완벽한 전달(perfect forwarding)** 을 사용하는 생성자 */
	template<typename... Args>
	TIteratorAdapter(Args&&... args) : BaseType(std::forward<Args>(args)...) {}

	ElementType& operator*() const
	{
		return this->Dereference();
	}

	ElementType* operator->() const
	{
		return &this->Dereference();
	}

	ThisType& operator++()
	{
		this->Increment();
		return *this;
	}

	ThisType operator++(int)
	{
		ThisType Temp = *this;
		++*this;
		return Temp;
	}

	ThisType& operator--()
	{
		this->Decrement();
		return *this;
	}

	ThisType operator--(int)
	{
		ThisType Temp = *this;
		--*this;
		return Temp;
	}

	ThisType& operator+=(SizeType Offset)
	{
		this->Increment(Offset);
		return *this;
	}

	ThisType& operator-=(SizeType Offset)
	{
		this->Decrement(Offset);
		return *this;
	}

	ThisType operator+(SizeType Offset) const
	{
		ThisType Temp = *this;
		return Temp += Offset;
	}

	ThisType operator-(SizeType Offset) const
	{
		ThisType Temp = *this;
		return Temp -= Offset;
	}

protected:
	// BaseType는 using 선언과 함께 강제되는 최소한의 앞으로 진행하는 반복(iteration) 기능을 반드시 구현을 구현해야 합니다.
	using BaseType::Dereference;
	using BaseType::Equals;
	using BaseType::Increment;

	template <typename AnyElementType>
	friend class TIteratorAdapter;

public:
	bool operator==(const TIteratorAdapter& Right) const
	{
		return Equals(Right);
	}

	bool operator!=(const TIteratorAdapter& Right) const
	{
		return !Equals(Right);
	}
};