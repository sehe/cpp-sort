/*
 * Copyright (c) 2018 Morwenn
 * SPDX-License-Identifier: MIT
 */
#ifndef CPPSORT_DETAIL_SCOPE_EXIT_H_
#define CPPSORT_DETAIL_SCOPE_EXIT_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <exception>
#include <type_traits>

namespace cppsort::detail
{
    template<typename EF>
    struct scope_success
    {
        private:

            EF exit_function;
            bool execute_on_destruction = true;
            int uncaught_on_creation = std::uncaught_exceptions();

        public:

            template<typename EFP>
            explicit scope_success(EFP&& func)
                noexcept(std::is_nothrow_constructible<EF, EFP>::value ||
                         std::is_nothrow_constructible<EF, EFP&>::value):
                exit_function(std::forward<EFP>(func))
            {}

            scope_success(scope_success&& rhs)
                noexcept(std::is_nothrow_move_constructible<EF>::value ||
                         std::is_nothrow_copy_constructible<EF>::value):
                exit_function(std::forward<EF>(rhs.exit_function))
            {}

            ~scope_success()
                noexcept(noexcept(exit_function()))
            {
                if (execute_on_destruction && std::uncaught_exceptions() <= uncaught_on_creation) {
                    exit_function();
                }
            }

            auto release() noexcept
                -> void
            {
                execute_on_destruction = false;
            }

            scope_success(const scope_success&) = delete;
            scope_success& operator=(const scope_success&) = delete;
            scope_success& operator=(scope_success&&) = delete;
    };

    template<typename EF>
    scope_success(EF)
        -> scope_success<EF>;
}

#endif // CPPSORT_DETAIL_SCOPE_EXIT_H_
