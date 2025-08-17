///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2025 Jack Phillips. All rights reserved.
// 
///////////////////////////////////////////////////////////////////////////////

#ifndef ARES_CORE_VECTOR_H
#define ARES_CORE_VECTOR_H

#include "core/internal/config.h"
#include "core/allocator.h"

#if ARES_CORE_EASTL_DISABLED
	#include <vector>
	namespace ares::core {
		template <typename value, typename allocator_type = temp_allocator>
		using vector = std::vector<value, allocator<allocator_type>>;
	}
#else
	#include <EASTL/vector.h>
	namespace ares::core {
		template <typename value, typename allocator_type = temp_allocator>
		using vector = eastl::vector<value, allocator<allocator_type>>;
	}
#endif // ARES_CORE_EASTL_DISABLED 

#endif // ARES_CORE_VECTOR_H