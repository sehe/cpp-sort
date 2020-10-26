/*
 * Copyright (c) 2015-2020 Morwenn
 * SPDX-License-Identifier: MIT
 */
#ifndef CPPSORT_DETAIL_INDIRECT_COMPARE_H_
#define CPPSORT_DETAIL_INDIRECT_COMPARE_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <utility>
#include <cpp-sort/utility/as_function.h>

namespace cppsort::detail
{
    template<typename Compare, typename Projection>
    class indirect_compare
    {
        private:

            using compare_t = decltype(utility::as_function(std::declval<Compare&>()));
            using projection_t = decltype(utility::as_function(std::declval<Projection&>()));
            [[no_unique_address]] compare_t comp;
            [[no_unique_address]] projection_t proj;

        public:

            indirect_compare(Compare compare, Projection projection):
                comp(utility::as_function(compare)),
                proj(utility::as_function(projection))
            {}

            template<typename Iterator>
            auto operator()(Iterator lhs, Iterator rhs)
                noexcept(noexcept(comp(proj(*lhs), proj(*rhs))))
                -> decltype(comp(proj(*lhs), proj(*rhs)))
            {
                return comp(proj(*lhs), proj(*rhs));
            }
    };
}

#endif // CPPSORT_DETAIL_INDIRECT_COMPARE_H_
