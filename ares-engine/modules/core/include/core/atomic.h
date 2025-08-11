#ifndef ARES_CORE_ATOMIC_H
#define ARES_CORE_ATOMIC_H
#include <EASTL/atomic.h>

namespace ares::core {

	template <typename T>
	using atomic = eastl::atomic<T>;

	using atomic_flag = eastl::atomic_flag;

	constexpr auto memory_order_relaxed = eastl::internal::memory_order_relaxed_s{};
	constexpr auto memory_order_aquire = eastl::internal::memory_order_acquire_s{};
	constexpr auto memory_order_acq_rel = eastl::internal::memory_order_acq_rel_s{};
	constexpr auto memory_order_release = eastl::internal::memory_order_release_s{};
	constexpr auto memory_order_read_depends = eastl::internal::memory_order_read_depends_s{};
	constexpr auto memory_order_seq_cst = eastl::internal::memory_order_seq_cst_s{};

}

#endif // ARES_CORE_ATOMIC_H