#include <cstddef>
#include <cassert>
#include <exception>
#include <windows.h>

namespace lcl::memory
{
    static SYSTEM_INFO win32_global_system_info;

    [[nodiscard]] static inline bool is_win32_global_system_info_initialised() 
    {
        return win32_global_system_info.dwPageSize != 0;
    }

    [[nodiscard]] size_t get_page_size()
    {
        if (!is_win32_global_system_info_initialised())
        {
            GetNativeSystemInfo(&win32_global_system_info);
            assert(is_win32_global_system_info_initialised());
        }

        return win32_global_system_info.dwPageSize;
    }

    [[nodiscard]] std::byte* reserve_memory_pages(const size_t pages_to_reserve)
    {
        auto result = VirtualAlloc(nullptr, pages_to_reserve * get_page_size(), MEM_RESERVE, PAGE_READWRITE);
    
        if (result == nullptr)
        {
            throw std::bad_alloc{};
        }

        return static_cast<std::byte*>(result);
    }
    
    void commit_memory_pages(const void* base_address, const size_t pages_to_commit)
    {
        //@Todo
    }

    void decommit_memory_pages(const void* base_address, const size_t pages_to_uncommit)
    {
        //@Todo
    }

    void unreserve_memory_pages(const void* base_address, const size_t pages_to_unreserve)
    {
        //@Todo
    }
}