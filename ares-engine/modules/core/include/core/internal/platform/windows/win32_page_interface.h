///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2025 Jack Phillips. All rights reserved.
// 
///////////////////////////////////////////////////////////////////////////////

#ifndef ARES_CORE_INTERNAL_WIN32_PAGE_INTERFACE_H
#define ARES_CORE_INTERNAL_WIN32_PAGE_INTERFACE_H

#include "core/internal/common.h"
#include "core/internal/engine_api.h"
#include "core/internal/os_page_interface.h"

namespace ares::core::internal {

	class ARES_CORE_API win32_page_interface : public os_page_interface
	{
	public:
		win32_page_interface();

		size_t page_size() const override;
		void* reserve_memory(size_t size) override;
		bool commit_memory(void* address, size_t size) override;
		bool decommit_memory(void* address, size_t size) override;
		bool release_memory(void* address, size_t size) override;
	};

}

#endif // ARES_CORE_INTERNAL_WIN32_PAGE_INTERFACE_H