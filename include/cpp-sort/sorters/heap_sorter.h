/*
 * Copyright (c) 2015-2019 Morwenn
 * SPDX-License-Identifier: MIT
 */
#ifndef CPPSORT_SORTERS_HEAP_SORTER_H_
#define CPPSORT_SORTERS_HEAP_SORTER_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>
#include <cpp-sort/fwd.h>
#include <cpp-sort/sorter_facade.h>
#include <cpp-sort/sorter_traits.h>
#include <cpp-sort/utility/functional.h>
#include "../detail/dary_heapsort.h"
#include "../detail/heapsort.h"
#include "../detail/iterator_traits.h"

namespace cppsort
{
    ////////////////////////////////////////////////////////////
    // Sorter

    namespace detail
    {
        template<int D>
        struct heap_sorter_impl
        {
            template<
                typename RandomAccessIterator,
                typename Compare = std::less<>,
                typename Projection = utility::identity,
                typename = std::enable_if_t<
                    is_projection_iterator_v<Projection, RandomAccessIterator, Compare>
                >
            >
            auto operator()(RandomAccessIterator first, RandomAccessIterator last,
                            Compare compare={}, Projection projection={}) const
                -> void
            {
                static_assert(
                    std::is_base_of_v<
                        std::random_access_iterator_tag,
                        iterator_category_t<RandomAccessIterator>
                    >,
                    "heap_sorter requires at least random-access iterators"
                );

                if constexpr (D == 2) {
                    heapsort(std::move(first), std::move(last),
                             std::move(compare), std::move(projection));
                } else {
                    make_d_ary_heap<D>(first, last, compare, projection);
                    sort_d_ary_heap<D>(std::move(first), std::move(last),
                                       std::move(compare), std::move(projection));
                }
            }

            ////////////////////////////////////////////////////////////
            // Sorter traits

            using iterator_category = std::random_access_iterator_tag;
            using is_always_stable = std::false_type;
        };
    }

    template<int D>
    struct heap_sorter:
        sorter_facade<detail::heap_sorter_impl<D>>
    {
        static_assert(D >= 2, "d-ary heap_sorter must have d >= 2");
    };

    ////////////////////////////////////////////////////////////
    // Sort function

    inline constexpr heap_sorter heap_sort{};
}

#endif // CPPSORT_SORTERS_HEAP_SORTER_H_
