/*
 * Copyright (c) 2015-2018 Morwenn
 * SPDX-License-Identifier: MIT
 */
#ifndef CPPSORT_UTILITY_SIZE_H_
#define CPPSORT_UTILITY_SIZE_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <iterator>
#include <type_traits>
#include <utility>
#include "../detail/type_traits.h"

namespace cppsort::utility
{
    namespace detail
    {
        template<typename Iterable>
        using can_use_std_size_t = decltype(std::size(std::declval<const Iterable&>()));
    }

    template<
        typename Iterable,
        typename = std::enable_if_t<
            cppsort::detail::is_detected_v<detail::can_use_std_size_t, Iterable>
        >
    >
    constexpr auto size(const Iterable& iterable)
        noexcept(noexcept(std::size(iterable)))
        -> decltype(std::size(iterable))
    {
        return std::size(iterable);
    }

    template<
        typename Iterable,
        typename = std::enable_if_t<
            not cppsort::detail::is_detected_v<detail::can_use_std_size_t, Iterable>
        >
    >
    constexpr auto size(const Iterable& iterable)
        -> decltype(std::distance(std::begin(iterable), std::end(iterable)))
    {
        return std::distance(std::begin(iterable), std::end(iterable));
    }
}

#endif // CPPSORT_UTILITY_SIZE_H_
