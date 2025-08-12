#ifndef ARES_CORE_OS_PAGE_INTERFACE_H
#define ARES_CORE_OS_PAGE_INTERFACE_H

namespace ares::core::internal {

	class os_page_interface
	{
	public:
		virtual ~os_page_interface() = default;

		virtual size_t page_size() const = 0;
		virtual void* reserve_memory(size_t size) = 0;
		virtual bool commit_memory(void* address, size_t size) = 0;
		virtual bool decommit_memory(void* address, size_t size) = 0;
		virtual bool release_memory(void* address, size_t size) = 0;
	};

	os_page_interface& get_os_page_interface();

}

#endif // ARES_CORE_OS_PAGE_INTERFACE_H