///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2025 Jack Phillips. All rights reserved.
// 
///////////////////////////////////////////////////////////////////////////////

#ifndef ARES_CORE_INTERNAL_PACKED_ARRAY_ITERATOR_H
#define ARES_CORE_INTERNAL_PACKED_ARRAY_ITERATOR_H

#include <EASTL/iterator.h>
#include <cstdint>
#include "core/internal/common.h"

namespace ares::core {

	template <typename value>
	class packed_array_iterator
	{
	public:
		template <typename, unsigned int>
		friend class packed_array;

		using iterator_category = eastl::random_access_iterator_tag;
		using value_type = value;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using pointer = value_type*;
		using reference = value_type&;
		//using get_offset_fn = pointer (*)(void*, size_type);

	private:
		//explicit packed_array_iterator(void* owner, size_type index, size_type size, get_offset_fn offset_fn) noexcept
		//	: owner_(owner), index_(index), size_(size), offset_fn_(offset_fn) {}

		packed_array_iterator(uint8_t* current, uint8_t* begin, uint8_t* end, size_type stride);

	public:
		/*
		~packed_array_iterator();
		packed_array_iterator(const packed_array_iterator& other)
			: owner_(other.owner_), index_(other.index_), size_(other.size_), offset_fn_(other.offset_fn_) {}
		packed_array_iterator& operator=(const packed_array_iterator& other);
		packed_array_iterator(packed_array_iterator&& other) noexcept
			: owner_(other.owner_), index_(other.index_), size_(other.size_), offset_fn_(other.offset_fn_) {}
		packed_array_iterator& operator=(packed_array_iterator&& other) noexcept;

		reference operator[](difference_type n) const;
		reference operator*() const { assert(index_ < size_); return *offset_fn_(owner_, index_); }
		pointer operator->() const { assert(index_ < size_); return offset_fn_(owner_, index_); }

		packed_array_iterator& operator++() { ++index_; return *this; }
		packed_array_iterator operator++(int) { packed_array_iterator tmp = *this; ++(*this); return tmp; }
		packed_array_iterator& operator--() { --index_; return *this; }
		packed_array_iterator operator--(int) { packed_array_iterator tmp = *this; --(*this); return tmp; }

		packed_array_iterator& operator+=(difference_type n) { index_ += n; return *this; }
		packed_array_iterator& operator-=(difference_type n) { index_ -= n; return *this; }
		packed_array_iterator operator+(difference_type n) const { return packed_array_iterator(owner_, index_ + n, size_, offset_fn_); }
		packed_array_iterator operator-(difference_type n) const { return packed_array_iterator(owner_, index_ - n, size_, offset_fn_); }
		difference_type operator-(const packed_array_iterator& other) const { return index_ - other.index_; }

		bool operator==(const packed_array_iterator& other) const { return index_ == other.index_; }
		bool operator!=(const packed_array_iterator& other) const { return !(*this == other); }
		bool operator<(const packed_array_iterator& other) const { return index_ < other.index_; }
		bool operator>(const packed_array_iterator& other) const { return index_ > other.index_; }
		bool operator<=(const packed_array_iterator& other) const { return index_ <= other.index_; }
		bool operator>=(const packed_array_iterator& other) const { return index_ >= other.index_; }
		*/

		~packed_array_iterator() = default;
		packed_array_iterator(const packed_array_iterator& other)
			: current_(other.current_), begin_(other.begin_), end_(other.end_), stride_(other.stride_) {}
		packed_array_iterator& operator=(const packed_array_iterator& other);
		packed_array_iterator(packed_array_iterator&& other) noexcept
			: current_(eastl::exchange(other.current_, nullptr)), begin_(eastl::exchange(other.begin_, nullptr)), end_(eastl::exchange(other.end_, nullptr)), stride_(eastl::exchange(other.stride_, 0)) {}
		packed_array_iterator& operator=(packed_array_iterator&& other) noexcept;

		reference operator[](difference_type n) const;
		reference operator*() const;
		pointer operator->() const;

		packed_array_iterator& operator++();
		packed_array_iterator operator++(int) { packed_array_iterator tmp = *this; ++(*this); return tmp; }
		packed_array_iterator& operator--();
		packed_array_iterator operator--(int) { packed_array_iterator tmp = *this; --(*this); return tmp; }

		packed_array_iterator& operator+=(difference_type n);
		packed_array_iterator& operator-=(difference_type n);
		packed_array_iterator operator+(difference_type n) const { return packed_array_iterator(current_ + stride * n, begin_, end_, stride_); }
		packed_array_iterator operator-(difference_type n) const { return packed_array_iterator(current_ - stride * n, begin_, end_, stride_); }
		difference_type operator-(const packed_array_iterator& other) const { return (current_ - other.current_) / stride_; }

		bool operator==(const packed_array_iterator& other) const { return current_ == other.current_; }
		bool operator!=(const packed_array_iterator& other) const { return !(*this == other); }
		bool operator<(const packed_array_iterator& other) const { return current_ < other.current_; }
		bool operator>(const packed_array_iterator& other) const { return current_ > other.current_; }
		bool operator<=(const packed_array_iterator& other) const { return current_ <= other.current_; }
		bool operator>=(const packed_array_iterator& other) const { return current_ >= other.current_; }
	private:
		uint8_t* current_;
		uint8_t* begin_;
		uint8_t* end_;
		size_type stride_;
		//void* owner_;
		//size_type index_;
		//size_type size_;
		//get_offset_fn offset_fn_;
	};

	template <typename value>
	inline packed_array_iterator<value>::packed_array_iterator(uint8_t* current, uint8_t* begin, uint8_t* end, size_type stride)
		: current_(current), begin_(begin), end_(end), stride_(stride)
	{
		size_t result = (begin - end) / stride;
	#if ARES_BUILD_DEBUG
		if (current_ > end_ || current_ < begin_)
			assert(false && "Iterator out of bounds!");
	#endif
	}

	template <typename value>
	inline packed_array_iterator<value>& packed_array_iterator<value>::operator=(const packed_array_iterator& other)
	{
		if (this != &other)
		{
			current_ = other.current_;
			begin_ = other.begin_;
			end_ = other.end_;
			stride_ = other.stride_;
		}
		return *this;
	}

	template <typename value>
	inline packed_array_iterator<value>& packed_array_iterator<value>::operator=(packed_array_iterator&& other) noexcept
	{
		if (this != &other)
		{
			current_ = eastl::exchange(other.current_, nullptr);
			begin = eastl::exchange(other.begin_, nullptr);
			end_ = eastl::exchange(other.end_, nullptr);
			stride_ = eastl::exchange(other.stride_, 0);
		}
		return *this;
	}

	template <typename value>
	inline typename packed_array_iterator<value>::reference packed_array_iterator<value>::operator[](difference_type n) const
	{
	#if ARES_BUILD_DEBUG
		uint8_t* result = current + n * stride_;
		if (result > end_ || result < begin_)
			assert(false && "Iterator out of bounds!");
		return *reinterpret_cast<pointer>(result);
	#endif
		return *reinterpret_cast<pointer>(current_ + n * stride_);
	}

	template <typename value>
	inline typename packed_array_iterator<value>::reference packed_array_iterator<value>::operator*() const
	{
	#if ARES_BUILD_DEBUG
		if (current_ > end_ || current_ < begin_)
			assert(false && "Iterator out of bounds!");
	#endif
		return *reinterpret_cast<pointer>(current_);
	}

	template <typename value>
	inline typename packed_array_iterator<value>::pointer packed_array_iterator<value>::operator->() const
	{
	#if ARES_BUILD_DEBUG
		if (current_ > end_ || current_ < begin_)
			assert(false && "Iterator out of bounds!");
	#endif
		return reinterpret_cast<pointer>(current_);
	}

	template <typename value>
	inline packed_array_iterator<value>& packed_array_iterator<value>::operator++()
	{
		current_ += stride_;
	#if ARES_BUILD_DEBUG
		if (current_ > end_ || current_ < begin_)
			assert(false && "Iterator out of bounds!");
	#endif
		return *this;
	}

	template <typename value>
	inline packed_array_iterator<value>& packed_array_iterator<value>::operator--()
	{
		current_ -= stride_;
	#if ARES_BUILD_DEBUG
		if (current_ > end_ || current_ < begin_)
			assert(false && "Iterator out of bounds!");
	#endif
		return *this;
	}

	template <typename value>
	inline packed_array_iterator<value>& packed_array_iterator<value>::operator+=(difference_type n)
	{
		current_ += stride_ * n;
	#if ARES_BUILD_DEBUG
		if (current_ > end_ || current_ < begin_)
			assert(false && "Iterator out of bounds!");
	#endif
		return *this;
	}

	template <typename value>
	inline packed_array_iterator<value>& packed_array_iterator<value>::operator-=(difference_type n)
	{
		current_ -= stride_ * n;
	#if ARES_BUILD_DEBUG
		if (current_ > end_ || current_ < begin_)
			assert(false && "Iterator out of bounds!");
	#endif
		return *this;
	}

	/*
	template <typename value>
	packed_array_iterator<value>::~packed_array_iterator()
	{
		owner_ = nullptr;
		index_ = 0;
		size_ = 0;
		offset_fn_ = nullptr;
	}

	template <typename value>
	packed_array_iterator<value>& packed_array_iterator<value>::operator=(const packed_array_iterator& other)
	{
		if (this != &other)
		{
			owner_ = other.owner_;
			index_ = other.index_;
			size_ = other.size_;
			offset_fn_ = other.offset_fn_;
		}
		return *this;
	}

	template <typename value>
	packed_array_iterator<value>& packed_array_iterator<value>::operator=(packed_array_iterator&& other) noexcept
	{
		if (this != &other)
		{
			owner_ = other.owner_;
			index_ = other.index_;
			size_ = other.size_;
			offset_fn_ = other.offset_fn_;
		}
		return *this;
	}

	template <typename value>
	typename packed_array_iterator<value>::reference packed_array_iterator<value>::operator[](difference_type n) const
	{
		assert(index_ + n >= 0 && static_cast<size_type>(index_ + n) < size_);
		return *offset_fn_(owner_, index_ + n);
	}
	*/
}

#endif // ARES_CORE_INTERNAL_PACKED_ARRAY_ITERATOR_H