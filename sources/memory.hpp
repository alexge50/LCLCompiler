#ifndef LCLCOMPILER_ALLOCATORS_HPP
#define LCLCOMPILER_ALLOCATORS_HPP

#include <cstddef>
#include <cassert>

namespace lcl::memory
{
    [[nodiscard]] auto get_page_size() -> std::size_t;
    [[nodiscard]] auto reserve_memory_pages(const std::size_t pages_to_reserve) -> std::byte*;
    
    auto commit_memory_pages   (const void* base_address, const std::size_t pages_to_commit) -> void;
    auto decommit_memory_pages (const void* base_address, const std::size_t pages_to_uncommit) -> void;
    auto unreserve_memory_pages(const void* base_address, const std::size_t pages_to_unreserve) -> void;

    class contiguous_virtual_memory_arena
    {
        const std::size_t m_reserved_pages;
        std::size_t       m_committed_pages = 0;
        std::byte*        m_base            = nullptr;

        public:
        explicit contiguous_virtual_memory_arena(const std::size_t pages_to_reserve) : m_reserved_pages(pages_to_reserve)
        {
            m_base = reserve_memory_pages(m_reserved_pages);
        }

        private:
        [[nodiscard]] auto address_to_commit_from() const noexcept -> std::byte*
        {
            return m_base + committed_bytes();
        }

        public:
        [[nodiscard]] auto reserved_pages() const noexcept -> std::size_t 
        {
            return m_reserved_pages;
        }

        [[nodiscard]] auto reserved_bytes() const noexcept -> std::size_t
        {
            return m_reserved_pages * get_page_size();
        }

        [[nodiscard]] auto committed_pages() const noexcept -> std::size_t 
        {
            return m_committed_pages;
        }

        [[nodiscard]] auto committed_bytes() const noexcept -> std::size_t 
        {
            return m_committed_pages * get_page_size();
        }

        [[nodiscard]] auto base_pointer() const noexcept -> std::byte*
        {
            return m_base;
        }

        [[nodiscard]] auto commit_memory(const std::size_t pages_to_commit) -> void 
        {
            commit_memory_pages(address_to_commit_from(), pages_to_commit);

            m_committed_pages += pages_to_commit;
        }

        ~contiguous_virtual_memory_arena()
        {
            unreserve_memory_pages(m_base, m_reserved_pages);
        }
    };

    template <class T> class virtual_arena_allocator 
    {
        public:
        using value_type      = T;
        using pointer         = T*;
        using const_pointer   = const T*;
        using reference       = T&;
        using const_reference = const T&;
        using size_type       = std::size_t;
        using difference_type = std::ptrdiff_t;

        template <class U> struct rebind 
        { 
            using other = virtual_arena_allocator<U>; 
        };

        private: 
        contiguous_virtual_arena& m_arena;

        public:
        explicit virtual_arena_allocator(const std::size_type reserve_count) noexcept : m_reseved(reserve_count)
        {
            //Empty
        }

        virtual_arena_allocator(const virtual_arena_allocator&)
        {

        }

        template<class U> virtual_arena_allocator(const virtual_arena_allocator<U>&) 
        {

        }
        
        auto address(reference value) const noexcept -> pointer
        {
            return &value;
        }

        auto address(const_reference value) const noexcept -> const_pointer 
        {
            return &value;
        }

        auto max_size() const noexcept -> std::size_type 
        {
            return m_reseved;
        }

        auto allocate(std::size_type size, const void* = nullptr) -> pointer 
        {
            auto ret = static_cast<pointer>(::operator new(num * sizeof(T)));
            std::cerr << " allocated at: " << static_cast<void*>(ret) << std::endl;
            return ret;
        }

        // initialize elements of allocated storage p with value value
        auto construct (pointer p, const T& value) -> void 
        {
            // initialize memory with placement new
            new(static_cast<void*>(p)) T{value};
        }

        // destroy elements of initialized storage p
        auto destroy (pointer p) -> void 
        {
            // destroy objects by calling their destructor
            p->~T();
        }

        // deallocate storage p of deleted elements
        auto deallocate (pointer p, std::size_type num) -> void 
        {
            // print message and deallocate memory with global delete
            std::cerr << "deallocate " << num << " element(s)"
                        << " of size " << sizeof(T)
                        << " at: " << static_cast<void*>(p) << std::endl;
                        
            ::operator delete(static_cast<void*>(p));
        }

        ~virtual_arena_allocator() noexcept = default;
    };

    //@Todo: Implement operator== and operator!= for allocator 
}

#endif LCLCOMPILER_ALLOCATORS_HPP