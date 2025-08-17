///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2025 Jack Phillips. All rights reserved.
// 
///////////////////////////////////////////////////////////////////////////////

#ifndef ARES_CORE_ALLOCATOR_BASE_H
#define ARES_CORE_ALLOCATOR_BASE_H

#include "core/internal/common.h"

namespace ares::core {

	class allocator_base
	{
	public:
		virtual ~allocator_base() = default;

		virtual void* allocate(size_t size) = 0;
		virtual void* allocate(size_t size, size_t alignment) = 0;
		virtual void deallocate(void* ptr) noexcept = 0;
	};

}

#endif // ARES_CORE_ALLOCATOR_BASE_H