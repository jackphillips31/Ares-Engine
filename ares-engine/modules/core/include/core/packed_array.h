#ifndef ARES_CORE_PACKED_ARRAY
#define ARES_CORE_PACKED_ARRAY
#include "core/platform.h"

namespace ares::core {

	namespace internal {

		class sys_allocator;

	}

	class default_allocator;

	template <typename value, unsigned int size, typename allocator = default_allocator>
	class packed_array
	{
	public:
		friend class packed_array_iterator;
		template <typename it_value>
		class packed_array_iterator
		{
		public:
			using iterator_category = eastl::random_access_iterator_tag;
			using value_type = it_value;
			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;
			using pointer = value_type*;
			using reference = value_type&;

			packed_array_iterator(packed_array* owner, size_type index)
				: owner_(owner), index_(index)
			{
			}
			packed_array_iterator(packed_array_iterator& other)
				: owner_(other.owner_), index_(other.index_)
			{
			}
			packed_array_iterator& operator=(packed_array_iterator& other)
			{
				if (this != &other)
				{
					owner_ = other.owner_;
					index_ = other.index_;
				}
				return *this;
			}

			reference operator[](difference_type n) const
			{
				assert(index_ + n >= 0 && static_cast<size_type>(index_ + n) < owner_->size());
				return *owner_->get_offset(index_ + n);
			}
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
			packed_array* owner_;
			size_type index_;
		};
	public:
		using value_type = value;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using reference = value_type&;
		using const_reference = const reference;
		using pointer = value_type*;
		using const_pointer = const pointer;
		using iterator = packed_array_iterator<value_type>;
		using const_iterator = packed_array_iterator<const value_type>;
		using reverse_iterator = eastl::reverse_iterator<iterator>;
		using const_reverse_iterator = eastl::reverse_iterator<const_iterator>;
		using allocator_type = internal::sys_allocator<allocator>;

		packed_array()
			: count_(size), allocator_()
		{
			buffer_ = allocator_.allocate(get_buffer_size(), ARES_CACHE_LINE_SIZE);
			for (size_type i = 0; i < count_; i++)
			{
				new (get_offset(i)) value_type();
			}
		}
		~packed_array()
		{
			if (buffer_)
			{
				for (size_type i = 0; i < count_; i++)
				{
					get_offset(i)->~value_type();
				}
				allocator_.deallocate(buffer_);
			}
		}

		packed_array(const packed_array& other)
			: count_(other.count_), allocator_(other.allocator_)
		{
			size_type buffer_size = get_buffer_size();
			buffer_ = allocator_.allocate(buffer_size, ARES_CACHE_LINE_SIZE);
			
			try
			{
				for (size_type i = 0; i < count_; i++)
				{
					new (get_offset(i)) value_type(other[i]);
				}
			}
			catch (...)
			{
				for (size_type i = 0; i < count_; i++)
				{
					get_offset(i)->~value_type();
				}
				allocator_.deallocate(buffer_);
				throw;
			}
		}
		packed_array& operator=(const packed_array& other)
		{
			if (this != &other)
			{
				if (count_ != other.count_)
				{
					for (size_type i = 0; i < count_; i++)
					{
						get_offset(i)->~value_type();
					}

					if (buffer_)
					{
						allocator_.deallocate(buffer_);
					}

					count_ = other.count_;
					allocator_ = other.allocator_;

					size_type buffer_size = get_buffer_size();
					buffer_ = allocator_.allocate(buffer_size, ARES_CACHE_LINE_SIZE);

					try
					{
						for (size_type i = 0; i < count_; i++)
						{
							new (get_offset(i)) value_type(other[i]);
						}
					}
					catch (...)
					{
						for (size_type i = 0; i < count_; i++)
						{
							get_offset(i)->~value_type();
						}
						allocator_.deallocate(buffer_);
						buffer_ = nullptr;
						count_ = 0;
						throw;
					}
				}
				else
				{
					for (size_type i = 0; i < count_; i++)
					{
						(*this)[i] = other[i];
					}
				}
			}
			return *this;
		}
		packed_array(packed_array&& other) noexcept
			: buffer_(eastl::exchange(other.buffer_, nullptr)), count_(eastl::move(other.count_)), allocator_(eastl::move(other.allocator_))
		{
		}
		packed_array& operator=(packed_array&& other) noexcept
		{
			if (this != &other)
			{
				buffer_ = eastl::exchange(other.buffer_, nullptr);
				count_ = eastl::move(other.count_);
				allocator_ = eastl::move(other.allocator_);
			}
			return *this;
		}

		iterator begin() noexcept { return iterator(this, 0); }
		const_iterator begin() const noexcept { return const_iterator(this, 0); }
		const_iterator cbegin() const noexcept { return const_iterator(this, 0); }
		
		iterator end() noexcept { return iterator(this, count_); }
		const_iterator end() const noexcept { return const_iterator(this, count_); }
		const_iterator cend() const noexcept { return const_iterator(this, count_); }

		reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
		const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
		const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }

		reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
		const_reverse_iterator rend() const noexcept  { return const_reverse_iterator(begin()); }
		const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }

		reference at(size_type index)
		{
			if (index >= count_) throw std::out_of_range("Index out of range.");
			return *get_offset(index);
		}
		const_reference at(size_type index) const
		{
			if (index >= count_) throw std::out_of_range("Index out of range.");
			return *get_offset(index);
		}
		reference operator[](size_type index) { return *get_offset(index); }
		const_reference operator[](size_type index) { return *get_offset(index); }
		reference front() { return *get_offset(0); }
		const_reference front() const { return *get_offset(0); }
		reference back() { return *get_offset(count_ - 1); }
		const_reference back() const { return *get_offset(count_ - 1); }

		constexpr size_type size() const noexcept { return count_; }
		constexpr size_type max_size() const noexcept { return count_; }
		constexpr bool empty() const noexcept { return count_ == 0; }

		// Using pointer arithmetic on this returned pointer can result
		// in undefined behavior!
		// This should only be used if you understand how the array is
		// storing the underlying data.
		pointer data() noexcept { return reinterpret_cast<pointer>(buffer_); }
		const_pointer data() const noexcept { return reinterpret_cast<const_pointer>(buffer_); }

		void fill(const reference value) { for (size_type i = 0; i < count_; i++) *get_offset(i) = value; }
		void swap(packed_array& other) noexcept
		{
			count_ = eastl::exchange(other.count_, count_);
			allocator_ = eastl::exchange(other.allocator_, allocator_);
			buffer_ = eastl::exchange(other.buffer_, buffer_);
		}

		allocator_type get_allocator() { return allocator_; }

	protected:
		pointer get_offset(size_t index)
		{
			size_t line = index / elements_per_line_;
			size_t within_line = index % elements_per_line_;
			uintptr_t line_start = reinterpret_cast<uintptr_t>(buffer_) + line * ARES_CACHE_LINE_SIZE;
			return reinterpret_cast<pointer>(line_start + within_line * value_type_size_);
		}

	private:
		size_type get_buffer_size()
		{
			size_type full_lines = count_;
			size_type remainder = count_ % elements_per_line_;
			size_type total_lines = full_lines + (remainder != 0 ? 1 : 0);
			return total_lines * ARES_CACHE_LINE_SIZE;
		}

	private:
		void* buffer_ = nullptr;
		size_type count_ = 0;
		allocator_type allocator_ = nullptr;

		static constexpr size_t value_type_size_ = sizeof(value_type);
		static constexpr size_t value_type_align_ = alignof(value_type);
		static constexpr size_t elements_per_line_ = ARES_CACHE_LINE_SIZE / value_type_size_;
	};
	
}

#endif // ARES_CORE_PACKED_ARRAY