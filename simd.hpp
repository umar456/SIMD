
#include <cstddef>
#include <cstdio>
#include <cmath>

template<typename SIMDIntrinsic>
ptrdiff_t get_aligned_offset(const void* addr)
{
    static const int alignment = sizeof(SIMDIntrinsic);
    static const ptrdiff_t mask = alignment-1;
    const ptrdiff_t addr_val = reinterpret_cast<ptrdiff_t>(addr);
    if ((addr_val & mask) == 0)
        return 0;
    return alignment - (addr_val & mask);
}

template<typename SIMDIntrinsic, typename T>
SIMDIntrinsic* get_aligned(T* raw_addr)
{
    ptrdiff_t aligned_addr = reinterpret_cast<ptrdiff_t>(raw_addr);
    aligned_addr += get_aligned_offset<SIMDIntrinsic>(raw_addr);
    return reinterpret_cast<SIMDIntrinsic*>(aligned_addr);
}

///
/// \brief This is a generic class to wrap SIMD intrensic elements
///
template<typename T, typename SIMDIntrinsic>
class simd
{
    static const size_t simd_size = sizeof(SIMDIntrinsic)/sizeof(T); //Number of elements in SIMD element
    T* raw_ptr;
    int align_offset;             //number of elements from raw ptr to first_aligned
    SIMDIntrinsic *first_aligned;   //First aligned memory location
    int total_elements;             //Total number of elements
    int simd_elements;              //Number of aligned SIMD sized elements
    int tail_elements;              //Number of elements after the SIMD elements

    //  |<---------------total_elements------------->|
    //  |<---------tail_offset---------->|
    //  |< aligned >|<---simd_elements-->|<  tail   >|
    //  |< offset  >|      (3)           |< elements>|
    //  ______________________________________________
    //  | e | e | e | SIMD | SIMD | SIMD | e | e | e |
    //  ^            ^                    ^
    //  |            |                    |
    //  raw_ptr      |                    aligned_end
    //              first_aligned
    int end_offset() const {
        return simd_elements * simd_size + align_offset;
    }
public:

    simd()
        : raw_ptr(nullptr)
        , first_aligned(nullptr)
        , align_offset(0)
        , total_elements(0)
    {}

    simd(T* addr, size_t elements)
        : raw_ptr(addr)
        , first_aligned(get_aligned<SIMDIntrinsic>(addr))
        , align_offset(get_aligned_offset<SIMDIntrinsic>(addr))
        , total_elements(elements)
        , simd_elements(floor(float(elements - align_offset) / simd_size))
        , tail_elements(elements - end_offset())
    {
    }

    /// \brief The number of elements before the aligned element
    int aligned_offset()                { return align_offset / sizeof(T); }

    /// \brief The first aligned element
    SIMDIntrinsic* aligned_begin()      { return first_aligned; }

    /// \brief One past the last aligned element in the array
    SIMDIntrinsic* aligned_end()        { return first_aligned + simd_elements; }
};

#include <cstdio>
template<typename T, typename SIMDIntrinsic>
class simd_iterator {
    SIMDIntrinsic *aligned_ptr;
public:
    static const int pack_size = sizeof(SIMDIntrinsic)/sizeof(T);
    simd_iterator(T* ptr)
        : aligned_ptr(get_aligned<SIMDIntrinsic>(ptr))
    { }

    simd_iterator& operator++() {
        aligned_ptr++;
        return *this;
    }

    simd_iterator operator++(int) {
        simd_iterator tmp = *this;
        aligned_ptr++;
        return tmp;
    }

    const   SIMDIntrinsic& operator*() const    { return *aligned_ptr; }
            SIMDIntrinsic& operator*()          { return *aligned_ptr; }

    SIMDIntrinsic *get() {
        return aligned_ptr;
    }
};
