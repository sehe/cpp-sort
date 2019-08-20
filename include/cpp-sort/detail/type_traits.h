/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2019 Morwenn
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef CPPSORT_DETAIL_TYPE_TRAITS_H_
#define CPPSORT_DETAIL_TYPE_TRAITS_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <type_traits>

namespace cppsort::detail
{
    ////////////////////////////////////////////////////////////
    // Alternative to std::conditional from C++11

    template<bool>
    struct conditional
    {
        template<typename T, typename U>
        using type = T;
    };

    template<>
    struct conditional<false>
    {
        template<typename T, typename U>
        using type = U;
    };

    template<bool B, typename T, typename U>
    using conditional_t = typename conditional<B>::template type<T, U>;

    ////////////////////////////////////////////////////////////
    // std::nonesuch from Library Fundamentals TS v2

    struct nonesuch
    {
        nonesuch() = delete;
        ~nonesuch() = delete;
        nonesuch(nonesuch const&) = delete;
        void operator=(nonesuch const&) = delete;
    };

    ////////////////////////////////////////////////////////////
    // Detection idiom from Library Fundamentals TS v2

    template<
        typename Default,
        typename AlwaysVoid,
        template<typename...> class Op, typename... Args
    >
    struct detector
    {
        using value_t = std::false_type;
        using type = Default;
    };

    template<
        typename Default,
        template<typename...> class Op,
        typename... Args
    >
    struct detector<Default, std::void_t<Op<Args...>>, Op, Args...>
    {
        using value_t = std::true_type;
        using type = Op<Args...>;
    };

    template<template<typename...> class Op, typename... Args>
    using is_detected = typename detector<nonesuch, void, Op, Args...>::value_t;

    template<template<typename...> class Op, typename... Args >
    constexpr bool is_detected_v = is_detected<Op, Args...>::value;

    template<template<typename...> class Op, typename... Args>
    using detected_t = typename detector<nonesuch, void, Op, Args...>::type;

    ////////////////////////////////////////////////////////////
    // Type traits to take __int128 into account even when the
    // standard library isn't instrumented but the type is still
    // available (e.g. -std=c++17)

#ifdef __SIZEOF_INT128__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

    template<typename T>
    struct is_integral:
        std::is_integral<T>
    {};

    template<>
    struct is_integral<signed __int128>:
        std::true_type
    {};

    template<>
    struct is_integral<unsigned __int128>:
        std::true_type
    {};

    template<typename T>
    struct is_signed:
        std::is_signed<T>
    {};

    template<>
    struct is_signed<signed __int128>:
        std::true_type
    {};

    template<typename T>
    struct is_unsigned:
        std::is_unsigned<T>
    {};

    template<>
    struct is_unsigned<unsigned __int128>:
        std::true_type
    {};

#pragma GCC diagnostic pop
#else

    template<typename T>
    using is_integral = std::is_integral<T>;

    template<typename T>
    using is_signed = std::is_signed<T>;

    template<typename T>
    using is_unsigned = std::is_unsigned<T>;

#endif
}

#endif // CPPSORT_DETAIL_TYPE_TRAITS_H_
