///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2025 Jack Phillips. All rights reserved.
// 
///////////////////////////////////////////////////////////////////////////////

#ifndef ARES_CORE_ALLOCATOR_H
#define ARES_CORE_ALLOCATOR_H

#include "core/internal/common.h"
#include "core/internal/sys_allocator.h"

namespace ares::core {

	template <typename allocator_type>
	using allocator = internal::sys_allocator<allocator_type>;

}

#endif // ARES_CORE_ALLOCATOR_H