///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2015 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////

#include <gsl/byte> // for byte
#include <gsl/span> // for span, span_iterator, operator==, operator!=
#include <gsl/util> // for narrow_cast, at

#include <array>       // for array
#include <cstddef>     // for ptrdiff_t
#include <iostream>    // for ptrdiff_t
#include <iterator>    // for reverse_iterator, operator-, operator==
#include <memory>      // for unique_ptr, shared_ptr, make_unique, allo...
#include <regex>       // for match_results, sub_match, match_results<>...
#include <string>      // for string
#include <type_traits> // for integral_constant<>::value, is_default_co...
#include <utility>
#include <vector> // for vector

// the string_view include and macro are used in the deduction guide verification
#if (defined(__cpp_deduction_guides) && (__cpp_deduction_guides >= 201611L))
#ifdef __has_include
#if __has_include(<string_view>)
#include <string_view>
#define HAS_STRING_VIEW
#endif // __has_include(<string_view>)
#endif // __has_include
#endif // (defined(__cpp_deduction_guides) && (__cpp_deduction_guides >= 201611L))

using namespace gsl;

namespace
{

struct BaseClass
{
};
struct DerivedClass : BaseClass
{
};
} // namespace


void from_array_constructor()
{
    int arr[5] = {1, 2, 3, 4, 5};
    int arr2d[2][3] = {1, 2, 3, 4, 5, 6};

#ifdef from_array_constructor_TEST1 // "error: no matching constructor for initialization of 'span<int, 6>'"
    {
        span<int, 6> s{arr};
    }
#endif

#ifdef from_array_constructor_TEST2 // "error: no matching constructor for initialization of 'span<int, 0>'"
    {
        span<int, 0> s{arr};
    }
#endif

#ifdef from_array_constructor_TEST3 // "error: no matching constructor for initialization of 'span<int>'"
    {
        span<int> s{arr2d};
    }
#endif

#ifdef from_array_constructor_TEST4 // "error: no matching constructor for initialization of 'span<int, 0>'"
    {
        span<int, 0> s{arr2d};
    }
#endif

#ifdef from_array_constructor_TEST5 // "error: no matching constructor for initialization of 'span<int, 6>'"
    {
        span<int, 6> s{arr2d};
    }
#endif

    int arr3d[2][3][2] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

#ifdef from_array_constructor_TEST6 // "error: no matching constructor for initialization of 'span<int>'"
    {
        span<int> s{arr3d};
    }
#endif

#ifdef from_array_constructor_TEST7 // "error: no matching constructor for initialization of 'span<int, 0>'"
    {
        span<int, 0> s{arr3d};
    }
#endif

#ifdef from_array_constructor_TEST8 // "error: no matching constructor for initialization of 'span<int, 11>'"
    {
        span<int, 11> s{arr3d};
    }
#endif

#ifdef from_array_constructor_TEST9 // "error: no matching constructor for initialization of 'span<int, 12>'"
    {
        span<int, 12> s{arr3d};
    }
#endif
}

void from_std_array_constructor()
{
    std::array<int, 4> arr = {1, 2, 3, 4};
#ifdef from_std_array_constructor_TEST1 // "error: no matching constructor for initialization of 'span<int, 2>'"
    {
        span<int, 2> s{arr};
    }
#endif
#ifdef from_std_array_constructor_TEST2 // "error: no matching constructor for initialization of 'span<const int, 2>'"
    {
        span<const int, 2> cs{arr};
    }
#endif

#ifdef from_std_array_constructor_TEST3 // "error: no matching constructor for initialization of 'span<int, 0>'"
    {
        span<int, 0> s{arr};
    }
#endif
#ifdef from_std_array_constructor_TEST4 // "error: no matching constructor for initialization of 'span<const int, 0>'"
    {
        span<const int, 0> cs{arr};
    }
#endif

#ifdef from_std_array_constructor_TEST5 // "error: no matching constructor for initialization of 'span<int, 5>'"
    {
        span<int, 5> s{arr};
    }
#endif

#ifdef from_std_array_constructor_TEST6 // "error: no matching function for call to object of type"
    {
        auto get_an_array = []() -> std::array<int, 4> { return {1, 2, 3, 4}; };
        auto take_a_span = [](span<int> s) { static_cast<void>(s); };
        // try to take a temporary std::array
        take_a_span(get_an_array());
    }
#endif
}

void from_const_std_array_constructor()
{
    const std::array<int, 4> arr = {1, 2, 3, 4};
#ifdef from_const_std_array_constructor_TEST1 // "error: no matching constructor for initialization of 'span<const int, 2>'"
    {
        span<const int, 2> s{arr};
    }
#endif

#ifdef from_const_std_array_constructor_TEST2 // "error: no matching constructor for initialization of 'span<const int, 0>'"
    {
        span<const int, 0> s{arr};
    }
#endif

#ifdef from_const_std_array_constructor_TEST3 // "error: no matching constructor for initialization of 'span<const int, 5>'"
    {
        span<const int, 5> s{arr};
    }
#endif
}

void from_std_array_const_constructor()
{
    std::array<const int, 4> arr = {1, 2, 3, 4};

#ifdef from_std_array_const_constructor_TEST1 // "error: no matching constructor for initialization of 'span<const int, 2>'"
    {
        span<const int, 2> s{arr};
    }
#endif

#ifdef from_std_array_const_constructor_TEST2 // "error: no matching constructor for initialization of 'span<const int, 0>'"
    {
        span<const int, 0> s{arr};
    }
#endif

#ifdef from_std_array_const_constructor_TEST3 // "error: no matching constructor for initialization of 'span<const int, 5>'"
    {
        span<const int, 5> s{arr};
    }
#endif

#ifdef from_std_array_const_constructor_TEST4 // "error: no matching constructor for initialization of 'span<int, 4>'"
    {
        span<int, 4> s{arr};
    }
#endif
}

void from_container_constructor()
{
    std::vector<int> v = {1, 2, 3};
    const std::vector<int> cv = v;

    std::string str = "hello";
    const std::string cstr = "hello";

#ifdef from_container_constructor_TEST1 // "error: no matching constructor for initialization of 'span<char>'"
    {
        span<char> s{str};
    }
#endif

#ifdef from_container_constructor_TEST2 // "error: no matching constructor for initialization of 'span<char>'"
    {
        span<char> s{cstr};
    }
#endif

#ifdef from_container_constructor_TEST3 // "error: no matching function for call to object of type"
    {
        auto get_temp_vector = []() -> std::vector<int> { return {}; };
        auto use_span = [](span<int> s) { static_cast<void>(s); };
        use_span(get_temp_vector());
    }
#endif

#ifdef from_container_constructor_TEST4 // "error: no matching function for call to object of type"
    {
        auto get_temp_string = []() -> std::string { return {}; };
        auto use_span = [](span<char> s) { static_cast<void>(s); };
        use_span(get_temp_string());
    }
#endif

#ifdef from_container_constructor_TEST5 // "error: no matching function for call to object of type"
    {
        auto get_temp_vector = []() -> const std::vector<int> { return {}; };
        auto use_span = [](span<const char> s) { static_cast<void>(s); };
        use_span(get_temp_vector());
    }
#endif

#ifdef from_container_constructor_TEST6 // "error: no matching constructor for initialization of 'span<int>'"
    {
        std::map<int, int> m;
        span<int> s{m};
    }
#endif
}

void from_convertible_span_constructor()
{
#ifdef from_convertible_span_constructor_TEST1 // "error: no viable conversion from 'span<\(anonymous namespace\)::DerivedClass>' to 'span<\(anonymous namespace\)::BaseClass>'"
    {
        span<DerivedClass> avd;
        span<BaseClass> avb = avd;
        static_cast<void>(avb);
    }
#endif

#ifdef from_convertible_span_constructor_TEST2 // "error: no viable conversion from 'span<int>' to 'span<unsigned int>'"
    {
        span<int> s;
        span<unsigned int> s2 = s;
        static_cast<void>(s2);
    }
#endif

#ifdef from_convertible_span_constructor_TEST3 // "error: no viable conversion from 'span<int>' to 'span<const unsigned int>'"
    {
        span<int> s;
        span<const unsigned int> s2 = s;
        static_cast<void>(s2);
    }
#endif

#ifdef from_convertible_span_constructor_TEST4 // "error: no viable conversion from 'span<int>' to 'span<short>'"
    {
        span<int> s;
        span<short> s2 = s;
        static_cast<void>(s2);
    }
#endif
}

void span_first()
{
    const auto terminateHandler = std::set_terminate([] {
        std::cerr << "Expected Death. first";
        std::abort();
    });
    const auto expected = GetExpectedDeathString(terminateHandler);

    int arr[5] = {1, 2, 3, 4, 5};
    {
        span<int, 5> av = arr;

#ifdef span_first_TEST1 // "error: no viable conversion from 'span<int>' to 'span<short>'"
        EXPECT_TRUE(av.first<6>().size() == 6);
#endif
#ifdef span_first_TEST1 // "error: no viable conversion from 'span<int>' to 'span<short>'"
        EXPECT_TRUE(av.first<-1>().size() == -1);
#endif
#ifdef span_first_TEST1 // "error: no viable conversion from 'span<int>' to 'span<short>'"
        EXPECT_DEATH(av.first(6).size(), expected);
#endif
    }
}
//
// TEST(span_test, last)
// {
//     const auto terminateHandler = std::set_terminate([] {
//         std::cerr << "Expected Death. last";
//         std::abort();
//     });
//     const auto expected = GetExpectedDeathString(terminateHandler);
//
//     int arr[5] = {1, 2, 3, 4, 5};
//
//     {
//         span<int, 5> av = arr;
//         EXPECT_TRUE(av.last<6>().size() == 6);
//     }
// }
//
// TEST(span_test, as_writable_bytes)
// {
//     int a[] = {1, 2, 3, 4};
//
//     {
//         // you should not be able to get writeable bytes for const objects
//         span<const int> s = a;
//         EXPECT_TRUE(s.size() == 4);
//         span<const byte> bs = as_writable_bytes(s);
//         EXPECT_TRUE(static_cast<void*>(bs.data()) == static_cast<void*>(s.data()));
//         EXPECT_TRUE(bs.size() == s.size_bytes());
//     }
// }
//
// TEST(span_test, fixed_size_conversions)
// {
//     const auto terminateHandler = std::set_terminate([] {
//         std::cerr << "Expected Death. fixed_size_conversions";
//         std::abort();
//     });
//     const auto expected = GetExpectedDeathString(terminateHandler);
//
//     int arr[] = {1, 2, 3, 4};
//
//     // converting to an span from an equal size array is ok
//     span<int, 4> s4 = arr;
//
// // initialization or assignment to static span that REDUCES size is NOT ok
//     {
//         span<int, 2> s = arr;
//     }
//     {
//         span<int, 2> s2 = s4;
//         static_cast<void>(s2);
//     }
//
//     // even when done dynamically
//     {
//         /*
//         // this now results in a compile-time error, rather than runtime.
//         // There is no suitable conversion from dynamic span to fixed span.
//         span<int> s = arr;
//         auto f = [&]() {
//             const span<int, 2> s2 = s;
//             static_cast<void>(s2);
//         };
//         EXPECT_DEATH(f(), expected);
//         */
//     }
//
//     /*
//      // this is not a legal operation in std::span, so we are no longer supporting it
//      // conversion from span<int, 4> to span<int, dynamic_extent> via call to `first`
//      // then convert from span<int, dynamic_extent> to span<int, 1>
//      // The dynamic to fixed extents are not supported in the standard
//      // to make this work, span<int, 1> would need to be span<int>.
//      {
//
//          // NB: implicit conversion to span<int,1> from span<int>
//          span<int, 1> s1 = s4.first(1);
//          static_cast<void>(s1);
//      }
//      */
//
//     // initialization or assignment to static span that requires size INCREASE is not ok.
//     int arr2[2] = {1, 2};
//
//     {
//         span<int, 4> s3 = arr2;
//     }
//     {
//         span<int, 2> s2 = arr2;
//         span<int, 4> s4a = s2;
//     }
//
//     /*
//      // This no longer compiles. There is no suitable conversion from dynamic span to a fixed size
//      span.
//      // this should fail - we are trying to assign a small dynamic span to a fixed_size larger one
//      span<int> av = arr2; auto f = [&]() {
//          const span<int, 4> _s4 = av;
//          static_cast<void>(_s4);
//      };
//      EXPECT_DEATH(f(), expected);
//      */
// }
//
