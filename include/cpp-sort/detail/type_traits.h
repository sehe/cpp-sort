/*
 * Copyright (c) 2015-2020 Morwenn
 * SPDX-License-Identifier: MIT
 */
#ifndef CPPSORT_DETAIL_TYPE_TRAITS_H_
#define CPPSORT_DETAIL_TYPE_TRAITS_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <cstddef>
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
    // std::is_bounded_array from C++20

    template<typename T>
    struct is_bounded_array:
        std::false_type
    {};

    template<typename T, std::size_t N>
    struct is_bounded_array<T[N]>:
        std::true_type
    {};

    template<typename T>
    constexpr bool is_bounded_array_v = is_bounded_array<T>::value;

    ////////////////////////////////////////////////////////////
    // Type traits to take __int128 into account even when the
    // standard library isn't instrumented but the type is still
    // available (e.g. -std=c++17)

#ifdef __SIZEOF_INT128__
    template<typename T>
    struct is_integral:
        std::is_integral<T>
    {};

    template<>
    struct is_integral<__int128_t>:
        std::true_type
    {};

    template<>
    struct is_integral<__uint128_t>:
        std::true_type
    {};

    template<typename T>
    struct is_signed:
        std::is_signed<T>
    {};

    template<>
    struct is_signed<__int128_t>:
        std::true_type
    {};

    template<typename T>
    struct is_unsigned:
        std::is_unsigned<T>
    {};

    template<>
    struct is_unsigned<__uint128_t>:
        std::true_type
    {};
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
