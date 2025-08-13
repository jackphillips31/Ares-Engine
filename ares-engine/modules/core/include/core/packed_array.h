#ifndef ARES_CORE_PACKED_ARRAY_H
#define ARES_CORE_PACKED_ARRAY_H
#include "core/platform.h"
#include "core/packed_array_iterator.h"

namespace ares::core {

	template <typename value, unsigned int array_size>
	class packed_array
	{
	public:
		using value_type = value;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using reference = value_type&;
		using const_reference = const value_type&;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using iterator = packed_array_iterator<value_type>;
		using const_iterator = packed_array_iterator<const value_type>;
		using reverse_iterator = eastl::reverse_iterator<iterator>;
		using const_reverse_iterator = eastl::reverse_iterator<const_iterator>;

		packed_array();
		packed_array(const packed_array& other);
		packed_array& operator=(const packed_array& other);
		packed_array(packed_array&& other) noexcept(eastl::is_nothrow_move_constructible_v<value_type>);
		packed_array& operator=(packed_array&& other) noexcept(eastl::is_nothrow_move_constructible_v<value_type>);
		~packed_array();

		iterator begin() noexcept { return iterator(buffer_, buffer_, buffer_end_, stride_); }
		const_iterator begin() const noexcept { return const_iterator(buffer_, buffer_, buffer_end_, stride_); }
		const_iterator cbegin() const noexcept { return const_iterator(buffer_, buffer_, buffer_end_, stride_); }
		
		iterator end() noexcept { return iterator(buffer_end_, buffer_, buffer_end_, stride_); }
		const_iterator end() const noexcept { return const_iterator(buffer_end_, buffer_, buffer_end_, stride_); }
		const_iterator cend() const noexcept { return const_iterator(buffer_end_, buffer_, buffer_end_, stride_); }

		reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
		const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
		const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }

		reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
		const_reverse_iterator rend() const noexcept  { return const_reverse_iterator(begin()); }
		const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }

		reference at(size_type index);
		const_reference at(size_type index) const;
		reference operator[](size_type index) { return *get_offset(index); }
		const_reference operator[](size_type index) const { return *get_offset(index); }
		reference front() { return *get_offset(0); }
		const_reference front() const { return *get_offset(0); }
		reference back() { return *get_offset(size_ - 1); }
		const_reference back() const { return *get_offset(size_ - 1); }

		constexpr size_type size() const noexcept { return size_; }
		constexpr size_type max_size() const noexcept { return size_; }
		constexpr bool empty() const noexcept { return size_ == 0; }

		// Using pointer arithmetic on this returned pointer can result
		// in undefined behavior!
		// This should only be used if you understand how the array is
		// storing the underlying data.
		pointer data() noexcept { return reinterpret_cast<pointer>(buffer_); }
		// Using pointer arithmetic on this returned pointer can result
		// in undefined behavior!
		// This should only be used if you understand how the array is
		// storing the underlying data.
		const_pointer data() const noexcept { return reinterpret_cast<const_pointer>(buffer_); }
		size_type stride() noexcept { return stride_; }
		const size_type stride() const noexcept { return stride_; }

		void fill(const_reference fill_value);
		void swap(packed_array& other) noexcept;

	private:
		static pointer offset_thunk(void* owner, size_type index);
		pointer get_offset(size_type index) noexcept;
		const_pointer get_offset(size_type index) const noexcept;
		void destruct_all() noexcept;
		void destruct_range(size_type start, size_type end) noexcept;

	private:
		/*
		static constexpr size_type size_ = array_size;
		static constexpr size_type value_type_size_ = sizeof(value_type);
		static constexpr size_type value_type_align_ = alignof(value_type);
		static constexpr size_type elements_per_line_ = ARES_CACHE_LINE_SIZE / value_type_size_;
		*/

		static constexpr size_type align_up(size_type offset, size_type align) noexcept;
		static constexpr size_type get_buffer_size() noexcept;
		static constexpr size_type size_ = array_size;
		static constexpr size_type value_type_size_ = sizeof(value_type);
		static constexpr size_type value_type_align_ = alignof(value_type);
		static constexpr size_type stride_ = align_up(value_type_size_, value_type_align_);
		static constexpr size_type elements_per_line_ = ARES_CACHE_LINE_SIZE / stride_;

		static_assert(array_size > 0, "Array size must be greater than 0.");
		static_assert(elements_per_line_ > 0 || stride_ <= ARES_CACHE_LINE_SIZE, "Element too large for cache-line optimization.");

	private:
		alignas(ARES_CACHE_LINE_SIZE) uint8_t buffer_[get_buffer_size()];
		uint8_t* buffer_end_;
	};

	template <typename value, unsigned int array_size>
	packed_array<value, array_size>::packed_array()
		: buffer_end_(buffer_ + size_ * stride_)
	{
		size_type i = 0;
		try
		{
			for (; i < size_; i++)
			{
				new (get_offset(i)) value_type();
			}
		}
		catch (...)
		{
			destruct_range(0, i);
			throw;
		}
	}

	template <typename value, unsigned int array_size>
	packed_array<value, array_size>::packed_array(const packed_array& other)
		: buffer_end_(buffer_ + size_ * stride_)
	{
		size_t i = 0;
		try
		{
			for (; i < size_; i++)
			{
				new (get_offset(i)) value_type(*other.get_offset(i));
			}
		}
		catch (...)
		{
			destruct_range(0, i);
			throw;
		}
	}

	template <typename value, unsigned int array_size>
	packed_array<value, array_size>& packed_array<value, array_size>::operator=(const packed_array& other)
	{
		if (this == &other)
		{
			return *this;
		}

		size_t i = 0;
		try
		{
			for (; i < size_; i++)
			{
				*get_offset(i) = *other.get_offset(i);
			}
		}
		catch (...)
		{
			destruct_range(0, i);
			throw;
		}

		return *this;
	}

	template <typename value, unsigned int array_size>
	packed_array<value, array_size>::packed_array(packed_array&& other) noexcept(eastl::is_nothrow_move_constructible_v<value_type>)
		: buffer_end_(buffer_ + size_ * stride_)
	{
		if constexpr (eastl::is_nothrow_move_constructible_v<value_type>)
		{
			for (size_type i = 0; i < size_; i++)
			{
				new (get_offset(i)) value_type(eastl::move(*other.get_offset(i)));
			}
		}
		else
		{
			size_type i = 0;
			try
			{
				for (; i < size_; i++)
				{
					new (get_offset(i)) value_type(eastl::move(*other.get_offset(i)));
				}
			}
			catch (...)
			{
				destruct_range(0, i);
				throw;
			}
		}
	}

	template <typename value, unsigned int array_size>
	packed_array<value, array_size>& packed_array<value, array_size>::operator=(packed_array&& other) noexcept(eastl::is_nothrow_move_constructible_v<value_type>)
	{
		if (this == &other)
		{
			return *this;
		}
		
		if constexpr (eastl::is_nothrow_move_constructible_v<value_type>)
		{
			for (size_type i = 0; i < size_; i++)
			{
				*get_offset(i) = eastl::move(*other.get_offset(i));
			}
		}
		else
		{
			size_type i = 0;
			try
			{
				for (; i < size_; i++)
				{
					*get_offset(i) = *other.get_offset(i);
				}
			}
			catch (...)
			{
				destruct_range(0, i);
				throw;
			}
		}

		return *this;
	}

	template <typename value, unsigned int array_size>
	packed_array<value, array_size>::~packed_array()
	{
		destruct_all();
	}

	template <typename value, unsigned int array_size>
	typename packed_array<value, array_size>::reference packed_array<value, array_size>::at(size_type index)
	{
		if (index >= size_) throw std::out_of_range("Index out of range.");
		return *get_offset(index);
	}

	template <typename value, unsigned int array_size>
	typename packed_array<value, array_size>::const_reference packed_array<value, array_size>::at(size_type index) const
	{
		if (index >= size_) throw std::out_of_range("Index out of range.");
		return *get_offset(index);
	}

	template <typename value, unsigned int array_size>
	void packed_array<value, array_size>::fill(const_reference fill_value)
	{
		for (size_type i = 0; i < size_; i++)
		{
			*get_offset(i) = fill_value;
		}
	}

	template <typename value, unsigned int array_size>
	void packed_array<value, array_size>::swap(packed_array& other) noexcept
	{
		if constexpr (eastl::is_trivially_copyable_v<value_type>)
		{
			alignas(ARES_CACHE_LINE_SIZE) uint8_t tmp[get_buffer_size()];
			std::memcpy(tmp, buffer_, get_buffer_size());
			std::memcpy(buffer_, other.buffer_, get_buffer_size());
			std::memcpy(other.buffer_, tmp, get_buffer_size());
		}
		else
		{
			for (size_type i = 0; i < size_; i++)
			{
				eastl::swap(*get_offset(i), *other.get_offset(i));
			}
		}
	}

	template <typename value, unsigned int array_size>
	typename packed_array<value, array_size>::pointer packed_array<value, array_size>::offset_thunk(void* owner, size_type index)
	{
		packed_array* self = static_cast<packed_array*>(owner);
		return self->get_offset(index);
	}

	template <typename value, unsigned int array_size>
	typename packed_array<value, array_size>::pointer packed_array<value, array_size>::get_offset(size_type index) noexcept
	{
		/*
		size_type line = index / elements_per_line_;
		size_type within_line = index % elements_per_line_;
		uint8_t* line_start = buffer_ + line * ARES_CACHE_LINE_SIZE;
		return reinterpret_cast<pointer>(line_start + within_line * value_type_size_);
		*/
	#if ARES_BUILD_DEBUG
		assert(index < size_ && "packed_array out of bounds!");
	#endif
		if constexpr (elements_per_line_ == 0)
		{
			return reinterpret_cast<pointer>(buffer_ + index * stride_);
		}
		else
		{
			size_type line = index / elements_per_line_;
			size_type within_line = index % elements_per_line_;
			uint8_t* line_start = buffer_ + line * ARES_CACHE_LINE_SIZE;
			size_type element_offset = within_line * stride_;
			return reinterpret_cast<pointer>(line_start + element_offset);
		}
	}

	template <typename value, unsigned int array_size>
	typename packed_array<value, array_size>::const_pointer packed_array<value, array_size>::get_offset(size_type index) const noexcept
	{
		/*
		size_type line = index / elements_per_line_;
		size_type within_line = index % elements_per_line_;
		const uint8_t* line_start = buffer_ + line * ARES_CACHE_LINE_SIZE;
		return reinterpret_cast<const_pointer>(line_start + within_line * value_type_size_);
		*/
		if constexpr (elements_per_line_ == 0)
		{
			return reinterpret_cast<const_pointer>(buffer_ + index * stride_);
		}
		else
		{
			size_type line = index / elements_per_line_;
			size_type within_line = index % elements_per_line_;
			const uint8_t* line_start = buffer_ + line * ARES_CACHE_LINE_SIZE;
			size_type element_offset = within_line * stride_;
			return reinterpret_cast<const_pointer>(line_start + element_offset);
		}
	}

	template <typename value, unsigned int array_size>
	constexpr typename packed_array<value, array_size>::size_type packed_array<value, array_size>::align_up(size_type offset, size_type align) noexcept
	{
		return (offset + align - 1) & ~(align - 1);
	}

	template <typename value, unsigned int array_size>
	constexpr typename packed_array<value, array_size>::size_type packed_array<value, array_size>::get_buffer_size() noexcept
	{
		/*
		const size_type full_lines = size_;
		const size_type remainder = size_ % elements_per_line_;
		const size_type total_lines = full_lines + (remainder != 0 ? 1 : 0);
		return total_lines * ARES_CACHE_LINE_SIZE;
		*/
		if constexpr (elements_per_line_ == 0)
		{
			return align_up(size_ * stride_, ARES_CACHE_LINE_SIZE);
		}
		else
		{
			size_type full_lines = size_ / elements_per_line_;
			size_type remainder = size_ % elements_per_line_;
			size_type total_lines = full_lines + (remainder > 0 ? 1 : 0);
			return total_lines * ARES_CACHE_LINE_SIZE;
		}
	}

	template <typename value, unsigned int array_size>
	void packed_array<value, array_size>::destruct_all() noexcept
	{
		destruct_range(0, size_);
	}

	template <typename value, unsigned int array_size>
	void packed_array<value, array_size>::destruct_range(size_type start, size_type end) noexcept
	{
		if constexpr (!eastl::is_trivially_destructible_v<value_type>)
		{
			for (size_type i = end; i-- > start; )
			{
				get_offset(i)->~value_type();
			}
		}
	}

}

#endif // ARES_CORE_PACKED_ARRAY_H