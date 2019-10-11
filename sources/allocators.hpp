#ifndef LCLCOMPILER_ALLOCATORS_HPP
#define LCLCOMPILER_ALLOCATORS_HPP

#include <cstddef>

namespace lcl::memory
{
    [[nodiscard]] void* reserve_memory_block(const size_t size);
    
    [[nodiscard]] bool commit_memory_block   (const void* block_begin, const size_t size_to_commit);
    [[nodiscard]] bool decommit_memory_block (const void* block_begin, const size_t size_to_uncommit);
    [[nodiscard]] bool unreserve_memory_block(const void* block_begin, const size_t size_to_unreserve);

    class contiguous_virtual_arena
    {
        const size_t m_reserved;
        size_t       m_committed = 0;
        std::byte*   m_base      = nullptr;

        public:
        explicit contiguous_virtual_arena(const size_t amount_to_reserve) : m_reserved(amount_to_reserve)
        {
            //Empty
        }

        private: 
        [[nodiscard]] std::byte* address_to_commit_from() const noexcept
        {
            return m_base + m_committed;
        }

        [[nodiscard]] size_t reserved_amount() const noexcept
        {
            return m_reserved;
        }

        [[nodiscard]] size_t committed_amount() const noexcept 
        {
            return m_committed;
        }

        [[nodiscard]] std::byte* base_pointer() const noexcept 
        {
            return m_base;
        }

        [[nodiscard]] void commit_memory(const size_t amount) const 
        {
            commit_memory_block(address_to_commit_from(), amount);
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
        void construct (pointer p, const T& value) {
            // initialize memory with placement new
            new((void*)p)T(value);
        }

        // destroy elements of initialized storage p
        void destroy (pointer p) {
            // destroy objects by calling their destructor
            p->~T();
        }

        // deallocate storage p of deleted elements
        void deallocate (pointer p, size_type num) {
            // print message and deallocate memory with global delete
            std::cerr << "deallocate " << num << " element(s)"
                        << " of size " << sizeof(T)
                        << " at: " << (void*)p << std::endl;
            ::operator delete((void*)p);
        }

        ~virtual_arena_allocator() noexcept = default;
    };

    // return that all specializations of this allocator are interchangeable
    template <class T1, class T2>
    bool operator== (const MyAlloc<T1>&,
                    const MyAlloc<T2>&) throw() {
        return true;
    }
    template <class T1, class T2>
    bool operator!= (const MyAlloc<T1>&,
                    const MyAlloc<T2>&) throw() {
        return false;
    }
}

#endif LCLCOMPILER_ALLOCATORS_HPP