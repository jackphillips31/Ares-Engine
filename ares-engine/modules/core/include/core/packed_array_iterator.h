#ifndef ARES_CORE_PACKED_ARRAY_ITERATOR_H
#define ARES_CORE_PACKED_ARRAY_ITERATOR_H
#include <EASTL/iterator.h>
#include <cstdint>

namespace ares::core {

	template <typename value, unsigned int size, typename allocator>
	class packed_array;

	template <typename value, unsigned int size, typename allocator>
	class packed_array_iterator
	{
	public:
		using iterator_category = eastl::random_access_iterator_tag;
		using value_type = value;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using pointer = value_type*;
		using reference = value_type&;

		packed_array_iterator(packed_array<value, size, allocator>* owner, size_type index) : owner_(owner), index_(index) {}
		packed_array_iterator(const packed_array_iterator& other) : owner_(other.owner_), index_(other.index_) {}
		packed_array_iterator& operator=(const packed_array_iterator& other);

		reference operator[](difference_type n) const;
		reference operator*() const { assert(index_ < owner_->size()); return *owner_->get_offset(index_); }
		pointer operator->() const { assert(index_ < owner_->size()); return owner_->get_offset(index_); }

		packed_array_iterator& operator++() { ++index_; return *this; }
		packed_array_iterator operator++(int) { packed_array_iterator tmp = *this; ++(*this); return tmp; }
		packed_array_iterator& operator--() { --index_; return *this; }
		packed_array_iterator operator--(int) { packed_array_iterator tmp = *this; --(*this); return tmp; }

		packed_array_iterator& operator+=(difference_type n) { index_ += n; return *this; }
		packed_array_iterator& operator-=(difference_type n) { index_ -= n; return *this; }
		packed_array_iterator operator+(difference_type n) const { return packed_array_iterator(owner_, index_ + n); }
		packed_array_iterator operator-(difference_type n) const { return packed_array_iterator(owner_, index_ - n); }
		difference_type operator-(const packed_array_iterator& other) const { return index_ - other.index_; }

		bool operator==(const packed_array_iterator& other) const { return index_ == other.index_; }
		bool operator!=(const packed_array_iterator& other) const { return !(*this == other); }
		bool operator<(const packed_array_iterator& other) const { return index_ < other.index_; }
		bool operator>(const packed_array_iterator& other) const { return index_ > other.index_; }
		bool operator<=(const packed_array_iterator& other) const { return index_ <= other.index_; }
		bool operator>=(const packed_array_iterator& other) const { return index_ >= other.index_; }

	private:
		packed_array<value, size, allocator>* owner_;
		size_type index_;
	};

	template <typename value, unsigned int size, typename allocator>
	packed_array_iterator<value, size, allocator>& packed_array_iterator<value, size, allocator>::operator=(const packed_array_iterator& other)
	{
		if (this != &other)
		{
			owner_ = other.owner_;
			index_ = other.index_;
		}
		return *this;
	}

	template <typename value, unsigned int size, typename allocator>
	typename packed_array_iterator<value, size, allocator>::reference packed_array_iterator<value, size, allocator>::operator[](difference_type n) const
	{
		assert(index_ + n >= 0 && static_cast<size_type>(index_ + n) < owner_->size());
		return *owner_->get_offset(index_ + n);
	}
}

#endif // ARES_CORE_PACKED_ARRAY_ITERATOR_H