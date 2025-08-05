#ifndef ARES_CORE_WIN32_PAGE_ALLOCATOR_H
#define ARES_CORE_WIN32_PAGE_ALLOCATOR_H
#include "core/core_api.h"
#include "core/internal/page_allocator.h"

namespace ares::core::internal {

	class ARES_CORE_API win32_page_allocator : public page_allocator
	{
	public:
		win32_page_allocator();

		size_t page_size() const override;
		void* reserve_memory(size_t size) override;
		bool commit_memory(void* address, size_t size) override;
		bool decommit_memory(void* address, size_t size) override;
		bool release_memory(void* address, size_t size) override;
	};

}

#endif // ARES_CORE_WIN32_PAGE_ALLOCATOR_H