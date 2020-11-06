/*
 * Copyright (c) 2015-2020 Morwenn
 * SPDX-License-Identifier: MIT
 */
#ifndef CPPSORT_DETAIL_SORTING_NETWORK_SORT0_H_
#define CPPSORT_DETAIL_SORTING_NETWORK_SORT0_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <functional>
#include <type_traits>
#include <cpp-sort/sorter_traits.h>

namespace cppsort::detail
{
    template<>
    struct sorting_network_sorter_impl<0u>
    {
        template<
            typename RandomAccessIterator,
            typename Compare = std::less<>,
            typename Projection = std::identity,
            typename = std::enable_if_t<is_projection_iterator_v<
                Projection, RandomAccessIterator, Compare
            >>
        >
        auto operator()(RandomAccessIterator, RandomAccessIterator,
                        Compare={}, Projection={}) const
            -> void
        {}
    };
}

#endif // CPPSORT_DETAIL_SORTING_NETWORK_SORT0_H_
