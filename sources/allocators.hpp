#ifndef LCLCOMPILER_ALLOCATORS_HPP
#define LCLCOMPILER_ALLOCATORS_HPP

#include <cstddef>
#include <cassert>

namespace lcl::memory
{

    [[nodiscard]] constexpr bool support_for_huge_pages()
    {
        return false;
    }

    [[nodiscard]] size_t     get_page_size();
    [[nodiscard]] std::byte* reserve_memory_pages(const size_t pages_to_reserve);
    
    void commit_memory_pages   (const void* base_address, const size_t pages_to_commit);
    void decommit_memory_pages (const void* base_address, const size_t pages_to_uncommit);
    void unreserve_memory_pages(const void* base_address, const size_t pages_to_unreserve);

    class contiguous_virtual_memory_arena
    {
        const size_t m_reserved_pages;
        size_t       m_committed_pages = 0;
        std::byte*   m_base            = nullptr;

        public:
        explicit contiguous_virtual_memory_arena(const size_t pages_to_reserve) : m_reserved_pages(pages_to_reserve)
        {
            m_base = reserve_memory_pages(m_reserved_pages);
        }

        private:
        [[nodiscard]] std::byte* address_to_commit_from() const noexcept
        {
            return m_base + committed_bytes();
        }

        public:
        [[nodiscard]] size_t reserved_pages() const noexcept 
        {
            return m_reserved_pages;
        }

        [[nodiscard]] size_t reserved_bytes() const noexcept
        {
            return m_reserved_pages * get_page_size();
        }

        [[nodiscard]] size_t committed_pages() const noexcept 
        {
            return m_committed_pages;
        }

        [[nodiscard]] size_t committed_bytes() const noexcept 
        {
            return m_committed_pages * get_page_size();
        }

        [[nodiscard]] std::byte* base_pointer() const noexcept 
        {
            return m_base;
        }

        [[nodiscard]] void commit_memory(const size_t pages_to_commit) 
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

        template <class U>
        struct rebind 
        { 
            using other = virtual_arena_allocator<U>; 
        };

        private: 
        contiguous_virtual_arena& m_arena;

        public:
        virtual_arena_allocator(const size_type reserve_count) noexcept : m_reseved(reserve_count)
        {
            //Empty
        }

        virtual_arena_allocator(const virtual_arena_allocator&)
        {

        }

        template<class U> virtual_arena_allocator(const virtual_arena_allocator<U>&) 
        {

        }
        
        pointer address(reference value) const noexcept
        {
            return &value;
        }

        const_pointer address(const_reference value) const noexcept 
        {
            return &value;
        }

        size_type max_size() const noexcept 
        {
            return m_reseved;
        }

        pointer allocate(size_type size, const void* = nullptr) 
        {
            pointer ret = (pointer)(::operator new(num*sizeof(T)));
            std::cerr << " allocated at: " << (void*)ret << std::endl;
            return ret;
        }

        // initialize elements of allocated storage p with value value
        void construct (pointer p, const T& value) 
        {
            // initialize memory with placement new
            new((void*)p)T(value);
        }

        // destroy elements of initialized storage p
        void destroy (pointer p) 
        {
            // destroy objects by calling their destructor
            p->~T();
        }

        // deallocate storage p of deleted elements
        void deallocate (pointer p, size_type num) 
        {
            // print message and deallocate memory with global delete
            std::cerr << "deallocate " << num << " element(s)"
                        << " of size " << sizeof(T)
                        << " at: " << (void*)p << std::endl;
            ::operator delete((void*)p);
        }

        ~virtual_arena_allocator() noexcept = default;
    };

    //@Todo: Implement operator== and operator!= for allocator 
}

#endif LCLCOMPILER_ALLOCATORS_HPP