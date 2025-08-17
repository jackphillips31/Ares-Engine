///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2025 Jack Phillips. All rights reserved.
// 
///////////////////////////////////////////////////////////////////////////////

#ifndef ARES_CORE_ATOMIC_H
#define ARES_CORE_ATOMIC_H

#include "core/internal/common.h"

#if ARES_EASTL_DISABLED
#include <atomic.h>

namespace ares::core {

	template <typename value>
	using atomic = std::atomic<value>;

	using atomic_flag = std::atomic_flag;

	ARES_CONSTEXPR auto memory_order_relaxed std::internal::memory_order_relaxed_s{};

}
#else
#include <EASTL/atomic.h>

namespace ares::core {

	template <typename T>
	using atomic = eastl::atomic<T>;

	using atomic_flag = eastl::atomic_flag;

	ARES_CONSTEXPR auto memory_order_relaxed = eastl::internal::memory_order_relaxed_s{};
	ARES_CONSTEXPR auto memory_order_aquire = eastl::internal::memory_order_acquire_s{};
	ARES_CONSTEXPR auto memory_order_acq_rel = eastl::internal::memory_order_acq_rel_s{};
	ARES_CONSTEXPR auto memory_order_release = eastl::internal::memory_order_release_s{};
	ARES_CONSTEXPR auto memory_order_read_depends = eastl::internal::memory_order_read_depends_s{};
	ARES_CONSTEXPR auto memory_order_seq_cst = eastl::internal::memory_order_seq_cst_s{};

}
#endif // ARES_EASTL_DISABLED

#endif // ARES_CORE_ATOMIC_H