// Copyright okdshin 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef MY_OPTIONAL_HPP
#define MY_OPTIONAL_HPP

#include <cstddef> // for std::size_t
#include <cassert>

//
// static_assert
//
#define MY_PP_CAT(a, b) MY_PP_CAT_I(a, b)
#define MY_PP_CAT_I(a, b) a##b

#define MY_STATIC_ASSERT(expr) \
	enum { MY_PP_CAT(my_static_assert_typedef, __LINE__) = \
		sizeof(my::static_assert_failure<static_cast<bool>(expr)>) };
namespace my {
	template<bool>
	struct static_assert_failure;

	template<>
	struct static_assert_failure<true> {
		static char const value = 1;
	};
}// namespace my

//
// optional
//
namespace my {
	namespace detail {
		template<std::size_t a, std::size_t b>
		struct min {
			static std::size_t const value = a < b ? a : b;
		};
		template<typename T>
		struct hack {
			char dummy;
			T t;
		};
	}// namespace detail
	
	// 実装の詳細についてはCryolite氏の記事を参照のこと
	template<typename T>
	struct alignment_of {
		static std::size_t const value =
			my::detail::min<
				sizeof(my::detail::hack<T>) - sizeof(T),
				sizeof(T)
			>::value;
	};

	class alignment_dummy;
	typedef void (*function_ptr)();
	typedef int (alignment_dummy::*member_ptr);
	typedef int (alignment_dummy::*member_function_ptr)();

	// 全ての型のアライメント定数の公倍数である最大のアライメント定数を持つことが期待される型
	union max_align {
        char c;
		short s;
		int i;
		long l;
		float f;
		double d;
		long double ld;
        void* vp;
		function_ptr fp;
		member_ptr mp;
		member_function_ptr mfp;
	};

	template<std::size_t Align>
	struct type_with_alignment {
	private:
		typedef max_align align_t;
		static std::size_t const align_val = my::alignment_of<align_t>::value;
		
		//　アライメントのチェック
		MY_STATIC_ASSERT(align_val >= Align);
		MY_STATIC_ASSERT(align_val % Align == 0);
	public:
		typedef align_t type;
	};

	template<std::size_t Size, std::size_t Align>
	struct aligned_storage {
		union type {
			char dummy[Size];
			typename my::type_with_alignment<Align>::type align_dummy;
		};
	};
	
	template<typename T>
	void* address_of(T& t) {
		return &t;
	}

	template<typename T>
	class optional {
	public:
		typedef T value_type;
		optional() : value_ptr_(0) {}
		optional(optional const& other) :
			value_ptr_(other ? 
				new(my::address_of(data_)) value_type(*other) : 
				0
			) {}
		explicit optional(value_type const& value) :
			value_ptr_(new(my::address_of(data_)) value_type(value)) {}

		~optional() {
			if(value_ptr_) { value_ptr_->~value_type(); }
		}
		
		optional& operator=(optional const& other) {
			optional& self = *this;
			if(self && other) {
				*self = *other;
			} else 
			if(!self && other) {
				value_ptr_ = new(my::address_of(data_)) value_type(*other);
			} else
			if(self && !other) {
				value_ptr_->~value_type();
				value_ptr_ = 0;
			} else
			if(!self && !other) {
				// do nothing
			}
			return self;
		}

		optional& operator=(value_type const& value) {
			return *this = optional(value);
		}

		operator bool() const { return value_ptr_; }

		value_type const& operator*() const {
			assert(value_ptr_ && "無効状態のoptionalに対して*演算子を適用すると未定義動作を引き起こす。");
			return *value_ptr_;
		}
		value_type& operator*() {
			return const_cast<value_type&>(*const_cast<optional const&>(*this));
		}
	private:
		value_type* value_ptr_;
		typename my::aligned_storage<
			sizeof(value_type),
			my::alignment_of<value_type>::value
		>::type data_;
	};
	
}// namespace my
#endif //MY_OPTIONAL_HPP
