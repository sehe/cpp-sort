/*
 * Copyright (c) 2015-2020 Morwenn
 * SPDX-License-Identifier: MIT
 */
#ifndef CPPSORT_DETAIL_COMPARISON_COUNTER_H_
#define CPPSORT_DETAIL_COMPARISON_COUNTER_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <cstddef>
#include <functional>
#include <utility>
#include <cpp-sort/utility/as_function.h>

namespace cppsort::detail
{
    template<
        typename Compare = std::less<>,
        typename CountType = std::size_t
    >
    class comparison_counter
    {
        public:

            comparison_counter(Compare compare, CountType& count):
                compare(std::move(compare)),
                count(count)
            {}

            template<typename T, typename U>
            auto operator()(T&& lhs, U&& rhs)
                -> decltype(auto)
            {
                ++count;
                auto&& comp = utility::as_function(compare);
                return comp(std::forward<T>(lhs), std::forward<U>(rhs));
            }

            // Accessible member data
            [[no_unique_address]] Compare compare;

        private:

            // Comparison functions are generally passed by value,
            // therefore we need to know where is the original counter
            // in order to increment it
            CountType& count;
    };
}

#endif // CPPSORT_DETAIL_COMPARISON_COUNTER_H_
