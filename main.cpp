#include "simd.hpp"
#include <x86intrin.h>
#include <vector>
#include <iostream>

using namespace std;

int main()
{
    typedef int test_type;

    vector<test_type> a(4e8, 100);
    vector<test_type> b(4e8, 10);
    vector<test_type> result(4e8, 4);

    simd<test_type, __m256i> aligned_a(&a[0], a.size());
    simd<test_type, __m256i> aligned_b(&b[2], b.size() - 2);
    simd<test_type, __m256i> aligned_r(&result[2], result.size() - 2);

    cout << "Aligned Offset A: "    << aligned_a.aligned_offset() << endl;
    cout << "Aligned Offset B: "    << aligned_b.aligned_offset() << endl;
    cout << "Aligned Offset result: " << aligned_r.aligned_offset() << endl;

    auto asimd = aligned_a.aligned_begin();
    auto bsimd = aligned_b.aligned_begin();
    auto rsimd = aligned_r.aligned_begin();
    for(int i = 0; i < 3; i++) {
        *rsimd++ = _mm256_add_epi8(*asimd++, *bsimd++);
    }

    cout << "Pack Size: " << simd_iterator<test_type, __m256i>::pack_size << endl;
    for(int i =0; i < 100; i++) {
        cout << (int)result[i] << endl;
    }
}
