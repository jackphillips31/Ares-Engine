#ifndef ARES_CORE_PACKED_ARRAY_H
#define ARES_CORE_PACKED_ARRAY_H
#include <EASTL/algorithm.h>
#include "core/platform.h"
#include "core/packed_array_iterator.h"
#include "core/type_traits.h"

namespace ares::core {

	template <typename value, unsigned int array_size>
	class packed_array
	{
	public:
		friend bool operator==(const packed_array&, const packed_array&);
		friend bool operator<(const packed_array&, const packed_array&);

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

		packed_array() noexcept(eastl::is_nothrow_default_constructible_v<value_type>);
		packed_array(std::initializer_list<value_type> init) noexcept(eastl::is_nothrow_copy_constructible_v<value_type>&& eastl::is_nothrow_default_constructible_v<value_type>);
		~packed_array() noexcept { destruct_all(); }

		packed_array(const packed_array& other) noexcept(eastl::is_trivially_copyable_v<value_type> || eastl::is_nothrow_copy_constructible_v<value_type>);
		packed_array& operator=(const packed_array& other) noexcept(eastl::is_trivially_copyable_v<value_type> || eastl::is_nothrow_copy_assignable_v<value_type>);
		packed_array(packed_array&& other) noexcept(eastl::is_trivially_copyable_v<value_type> || eastl::is_nothrow_move_constructible_v<value_type>);
		packed_array& operator=(packed_array&& other) noexcept(eastl::is_trivially_copyable_v<value_type> || eastl::is_nothrow_move_assignable_v<value_type>);

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
		reference operator[](size_type index) noexcept { return *get_element(index); }
		const_reference operator[](size_type index) const noexcept { return *get_element(index); }
		reference front() noexcept { return *get_element(0); }
		const_reference front() const noexcept { return *get_element(0); }
		reference back() noexcept { return *get_element(size_ - 1); }
		const_reference back() const noexcept { return *get_element(size_ - 1); }

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

		void fill(const_reference fill_value) noexcept(eastl::is_nothrow_copy_assignable_v<value_type>);
		void swap(packed_array& other) noexcept(eastl::is_trivially_copyable_v<value_type> || is_nothrow_swappable_v<value_type>);

	private:
		pointer get_element(size_type index) noexcept;
		const_pointer get_element(size_type index) const noexcept;
		size_type get_offset(size_type index) noexcept;
		void destruct_all() noexcept(eastl::is_trivially_destructible_v<value_type> || eastl::is_nothrow_destructible_v<value_type>);
		void destruct_range(size_type start, size_type end) noexcept(eastl::is_trivially_destructible_v<value_type> || eastl::is_nothrow_destructible_v<value_type>);

	private:
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
		uint8_t* buffer_end_ = buffer_ + get_buffer_size();
	};

	template <typename value, unsigned int array_size>
	packed_array<value, array_size>::packed_array() noexcept(eastl::is_nothrow_default_constructible_v<value_type>)
	{
		if constexpr (!eastl::is_trivially_default_constructible_v<value_type>)
		{
			if constexpr (!eastl::is_nothrow_default_constructible_v<value_type>)
			{
				size_type i = 0;
				try
				{
					for (; i < size_; i++)
					{
						new (get_element(i)) value_type();
					}
				}
				catch (...)
				{
					destruct_range(0, i);
					throw;
				}
			}
			else
			{
				for (size_type i = 0; i < size_; i++)
				{
					new (get_element(i)) value_type();
				}
			}
		}
	}

	template <typename value, unsigned int array_size>
	packed_array<value, array_size>::packed_array(std::initializer_list<value_type> init) noexcept(eastl::is_nothrow_copy_constructible_v<value_type> && eastl::is_nothrow_default_constructible_v<value_type>)
	{
		size_type i = 0;
		const size_type limit = static_cast<size_type>(eastl::min(init.size(), size_));
		if constexpr (!eastl::is_nothrow_copy_constructible_v<value_type> || !eastl::is_nothrow_default_constructible_v<value_type>)
		{
			try
			{
				for (; i < limit; i++)
				{
					new (get_element(i)) value_type(*(init.begin() + i));
				}

				if constexpr (!eastl::is_trivially_default_constructible_v<value_type>)
				{
					for (; i < size_; i++)
					{
						new (get_element(i)) value_type();
					}
				}
			}
			catch (...)
			{
				destruct_range(0, i);
				throw;
			}
		}
		else
		{
			for (; i < limit; i++)
			{
				new (get_element(i)) value_type(*(init.begin() + i));
			}
			if constexpr (!eastl::is_trivially_default_constructible_v<value_type>)
			{
				for (; i < size_; i++)
				{
					new (get_element(i)) value_type();
				}
			}
		}
	}

	template <typename value, unsigned int array_size>
	packed_array<value, array_size>::packed_array(const packed_array& other) noexcept(eastl::is_trivially_copyable_v<value_type> || eastl::is_nothrow_copy_constructible_v<value_type>)
	{
		if constexpr (eastl::is_trivially_copyable_v<value_type>)
		{
			std::memcpy(buffer_, other.buffer_, get_buffer_size());
		}
		else if constexpr (!eastl::is_nothrow_copy_constructible_v<value_type>)
		{
			size_type i = 0;
			try
			{
				for (; i < size_; i++)
				{
					new (get_element(i)) value_type(*other.get_element(i));
				}
			}
			catch (...)
			{
				destruct_range(0, i);
				throw;
			}
		}
		else
		{
			for (size_type i = 0; i < size_; i++)
			{
				new (get_element(i)) value_type(*other.get_element(i));
			}
		}
	}

	template <typename value, unsigned int array_size>
	packed_array<value, array_size>& packed_array<value, array_size>::operator=(const packed_array& other) noexcept(eastl::is_trivially_copyable_v<value_type> || eastl::is_nothrow_copy_assignable_v<value_type>)
	{
		if (this == &other) return *this;

		if constexpr (eastl::is_trivially_copyable_v<value_type>)
		{
			std::memcpy(buffer_, other.buffer_, get_buffer_size());
		}
		else
		{
			for (size_type i = 0; i < size_; i++)
			{
				*get_element(i) = *other.get_element(i);
			}
		}
		return *this;
	}

	template <typename value, unsigned int array_size>
	packed_array<value, array_size>::packed_array(packed_array&& other) noexcept(eastl::is_trivially_copyable_v<value_type> || eastl::is_nothrow_move_constructible_v<value_type>)
	{
		if constexpr (eastl::is_trivially_copyable_v<value_type>)
		{
			std::memcpy(buffer_, other.buffer_, get_buffer_size());
		}
		else if constexpr (!eastl::is_nothrow_move_constructible_v<value_type>)
		{
			size_type i = 0;
			try
			{
				for (; i < size_; i++)
				{
					new (get_element(i)) value_type(eastl::move(*other.get_element(i)));
				}
			}
			catch (...)
			{
				destruct_range(0, i);
				throw;
			}
		}
		else
		{
			for (size_type i = 0; i < size_; i++)
			{
				new (get_element(i)) value_type(eastl::move(*other.get_element(i)));
			}
		}
	}

	template <typename value, unsigned int array_size>
	packed_array<value, array_size>& packed_array<value, array_size>::operator=(packed_array&& other) noexcept(eastl::is_trivially_copyable_v<value_type> || eastl::is_nothrow_move_assignable_v<value_type>)
	{
		if (this == &other) return *this;
		
		if constexpr (eastl::is_trivially_copyable_v<value_type>)
		{
			std::memcpy(buffer_, other.buffer_, get_buffer_size());
		}
		else
		{
			for (size_type i = 0; i < size_; i++)
			{
				*get_element(i) = eastl::move(*other.get_element(i));
			}
		}
		return *this;
	}

	template <typename value, unsigned int array_size>
	typename packed_array<value, array_size>::reference packed_array<value, array_size>::at(size_type index)
	{
		if (index >= size_) throw std::out_of_range("Index out of range.");
		return *get_element(index);
	}

	template <typename value, unsigned int array_size>
	typename packed_array<value, array_size>::const_reference packed_array<value, array_size>::at(size_type index) const
	{
		if (index >= size_) throw std::out_of_range("Index out of range.");
		return *get_element(index);
	}

	template <typename value, unsigned int array_size>
	void packed_array<value, array_size>::fill(const_reference fill_value) noexcept(eastl::is_nothrow_copy_assignable_v<value_type>)
	{
		for (size_type i = 0; i < size_; i++)
		{
			*get_element(i) = fill_value;
		}
	}

	template <typename value, unsigned int array_size>
	void packed_array<value, array_size>::swap(packed_array& other) noexcept(eastl::is_trivially_copyable_v<value_type> || is_nothrow_swappable_v<value_type>)
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
				eastl::swap(*get_element(i), *other.get_element(i));
			}
		}
	}

	template <typename value, unsigned int array_size>
	typename packed_array<value, array_size>::pointer packed_array<value, array_size>::get_element(size_type index) noexcept
	{
	#if ARES_BUILD_DEBUG
		assert(index < size_ && "packed_array out of bounds!");
	#endif
		return reinterpret_cast<pointer>(buffer_ + get_offset(index));
	}

	template <typename value, unsigned int array_size>
	typename packed_array<value, array_size>::const_pointer packed_array<value, array_size>::get_element(size_type index) const noexcept
	{
	#if ARES_BUILD_DEBUG
		assert(index < size_ && "packed_array out of bounds!")
	#endif
		return reinterpret_cast<const_pointer>(buffer_ + get_offset(index));
	}

	template <typename value, unsigned int array_size>
	typename packed_array<value, array_size>::size_type packed_array<value, array_size>::get_offset(size_type index) noexcept
	{
		size_type line = (index / elements_per_line_) * ARES_CACHE_LINE_SIZE;
		size_type within_line = (index % elements_per_line_) * stride_;
		return line + within_line;
	}

	template <typename value, unsigned int array_size>
	constexpr typename packed_array<value, array_size>::size_type packed_array<value, array_size>::align_up(size_type offset, size_type align) noexcept
	{
		return (offset + align - 1) & ~(align - 1);
	}

	template <typename value, unsigned int array_size>
	constexpr typename packed_array<value, array_size>::size_type packed_array<value, array_size>::get_buffer_size() noexcept
	{
		size_type full_lines = size_ / elements_per_line_;
		size_type remainder = size_ % elements_per_line_;
		size_type total_lines = full_lines + (remainder > 0 ? 1 : 0);
		return total_lines * ARES_CACHE_LINE_SIZE;
	}

	template <typename value, unsigned int array_size>
	void packed_array<value, array_size>::destruct_all() noexcept(eastl::is_trivially_destructible_v<value_type> || eastl::is_nothrow_destructible_v<value_type>)
	{
		destruct_range(0, size_);
	}

	template <typename value, unsigned int array_size>
	void packed_array<value, array_size>::destruct_range(size_type start, size_type end) noexcept(eastl::is_trivially_destructible_v<value_type> || eastl::is_nothrow_destructible_v<value_type>)
	{
		if constexpr (!eastl::is_trivially_destructible_v<value_type>)
		{
			for (size_type i = end; i > start; i--)
			{
				get_element(i - 1)->~value_type();
			}
		}
	}

	template <typename value, unsigned int array_size>
	bool operator==(const packed_array<value, array_size>& lhs, const packed_array<value, array_size>& rhs)
	{
		if constexpr (eastl::is_trivially_copyable_v<value>)
		{
		#if ARES_BUILD_DEBUG
			assert(lhs.get_buffer_size() == rhs.get_buffer_size() && "packed_array size mismatch!");
		#endif
			return std::memcmp(lhs.data(), rhs.data(), lhs.get_buffer_size()) == 0;
		}
		else
		{
			for (size_t i = 0; i < array_size; i++)
			{
				if (!(lhs[i] == rhs[i]))
				{
					return false;
				}
			}
			return true;
		}		
	}

	template <typename value, unsigned int array_size>
	bool operator!=(const packed_array<value, array_size>& lhs, const packed_array<value, array_size>& rhs)
	{
		return !(lhs == rhs);
	}

	template <typename value, unsigned int array_size>
	bool operator<(const packed_array<value, array_size>& lhs, const packed_array<value, array_size>& rhs)
	{
		if constexpr (eastl::is_integral_v<value> && array_size > 0)
		{
			const value* l = lhs.data();
			const value* r = rhs.data();
			for (size_t i = 0; i < array_size; i++)
			{
				if (*l < *r) return true;
				if (*r < *l) return false;
				l += lhs.stride();
				r += lhs.stride();
			}
			return false;
		}
		else
		{
			for (size_t i = 0; i < array_size; i++)
			{
				if (lhs[i] < rhs[i]) return true;
				if (rhs[i] < lhs[i]) return false;
			}
			return false;
		}
	}

	template <typename value, unsigned int array_size>
	bool operator<=(const packed_array<value, array_size>& lhs, const packed_array<value, array_size>& rhs)
	{
		return !(rhs < lhs);
	}

	template <typename value, unsigned int array_size>
	bool operator>(const packed_array<value, array_size>& lhs, const packed_array<value, array_size>& rhs)
	{
		return rhs < lhs;
	}

	template <typename value, unsigned int array_size>
	bool operator>=(const packed_array<value, array_size>& lhs, const packed_array<value, array_size>& rhs)
	{
		return !(lhs < rhs);
	}

	template <typename value, unsigned int array_size>
	void swap(packed_array<value, array_size>& lhs, packed_array<value, array_size>& rhs) noexcept(noexcept(lhs.swap(rhs)))
	{
		lhs.swap(rhs);
	}

	template <unsigned int index, typename value, unsigned int array_size>
	constexpr value& get(packed_array<value, array_size>& arr) noexcept
	{
		//size_t line = (index / arr.elements_per_line_) * ARES_CACHE_LINE_SIZE;
		//size_t within_line = (index % arr.elements_per_line_) * arr.stride_;
		//return *reinterpret_cast<value*>(arr.buffer_ + line + within_line);
		return arr[index];
	}

	template <unsigned int index, typename value, unsigned int array_size>
	constexpr const value& get(const packed_array<value, array_size>& arr) noexcept
	{
		return arr[index];
	}

}

#endif // ARES_CORE_PACKED_ARRAY_H