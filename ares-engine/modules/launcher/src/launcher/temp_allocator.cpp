#include <ares_launcher_pch.h>
#include "core/temp_allocator.h"

namespace ares::core {

	temp_allocator& get_temp_allocator()
	{
		static temp_allocator result;
		return result;
	}

}