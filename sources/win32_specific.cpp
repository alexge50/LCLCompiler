#include <cstddef>
#include <cassert>
#include <exception>
#include <windows.h>

namespace lcl::memory
{
    static SYSTEM_INFO win32_global_system_info;

    [[nodiscard]] static inline auto is_win32_global_system_info_initialised() noexcept -> bool 
    {
        return win32_global_system_info.dwPageSize != 0;
    }

    [[nodiscard]] auto get_page_size() -> std::size_t
    {
        if (!is_win32_global_system_info_initialised())
        {
            GetNativeSystemInfo(&win32_global_system_info);
            assert(is_win32_global_system_info_initialised());
        }

        return win32_global_system_info.dwPageSize;
    }

    [[nodiscard]] auto reserve_memory_pages(const std::size_t pages_to_reserve) -> std::byte*
    {
        auto result = VirtualAlloc(nullptr, pages_to_reserve * get_page_size(), MEM_RESERVE, PAGE_READWRITE);
    
        if (result == nullptr)
        {
            throw std::bad_alloc{};
        }

        return static_cast<std::byte*>(result);
    }
    
    auto commit_memory_pages(const void* base_address, const std::size_t pages_to_commit) -> void
    {
        //@Todo
    }

    auto decommit_memory_pages(const void* base_address, const std::size_t pages_to_uncommit) -> void
    {
        //@Todo
    }

    auto unreserve_memory_pages(const void* base_address, const std::size_t pages_to_unreserve) -> void
    {
        //@Todo
    }
}