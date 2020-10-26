/*
 * Copyright (c) 2016-2020 Morwenn
 * SPDX-License-Identifier: MIT
 */
#ifndef CPPSORT_DETAIL_PROJECTION_COMPARE_H_
#define CPPSORT_DETAIL_PROJECTION_COMPARE_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <type_traits>
#include <utility>
#include <cpp-sort/utility/as_function.h>

namespace cppsort::detail
{
    template<typename Compare, typename Projection>
    class projection_compare
    {
        private:

            using compare_t = std::remove_cvref_t<decltype(utility::as_function(std::declval<Compare&>()))>;
            using projection_t = std::remove_cvref_t<decltype(utility::as_function(std::declval<Projection&>()))>;
            [[no_unique_address]] compare_t comp;
            [[no_unique_address]] projection_t proj;

        public:

            projection_compare(Compare compare, Projection projection):
                comp(utility::as_function(compare)),
                proj(utility::as_function(projection))
            {}

            template<typename T, typename U>
            auto operator()(T&& lhs, U&& rhs)
                noexcept(noexcept(comp(proj(std::forward<T>(lhs)), proj(std::forward<U>(rhs)))))
                -> decltype(comp(proj(std::forward<T>(lhs)), proj(std::forward<U>(rhs))))
            {
                return comp(proj(std::forward<T>(lhs)), proj(std::forward<U>(rhs)));
            }
    };
}

#endif // CPPSORT_DETAIL_PROJECTION_COMPARE_H_
