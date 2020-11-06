/*
 * Copyright (c) 2015-2020 Morwenn
 * SPDX-License-Identifier: MIT
 */
#ifndef CPPSORT_DETAIL_LOW_COMPARISONS_SORT7_H_
#define CPPSORT_DETAIL_LOW_COMPARISONS_SORT7_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <functional>
#include <type_traits>
#include <utility>
#include <cpp-sort/sorter_traits.h>
#include "../front_insert.h"

namespace cppsort::detail
{
    template<>
    struct low_comparisons_sorter_impl<7u>
    {
        template<
            typename RandomAccessIterator,
            typename Compare = std::less<>,
            typename Projection = std::identity,
            typename = std::enable_if_t<is_projection_iterator_v<
                Projection, RandomAccessIterator, Compare
            >>
        >
        auto operator()(RandomAccessIterator first, RandomAccessIterator,
                        Compare compare={}, Projection projection={}) const
            -> void
        {
            low_comparisons_sorter<6u>{}(first+1u, first+7u, compare, projection);
            front_insert<7u>(std::move(first), std::move(compare), std::move(projection));
        }
    };
}

#endif // CPPSORT_DETAIL_LOW_COMPARISONS_SORT7_H_
