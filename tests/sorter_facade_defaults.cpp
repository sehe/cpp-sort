/*
 * Copyright (c) 2015-2020 Morwenn
 * SPDX-License-Identifier: MIT
 */
#include <functional>
#include <iterator>
#include <vector>
#include <catch2/catch.hpp>
#include <cpp-sort/sorter_facade.h>

namespace
{
    struct comparison_sorter_impl
    {
        template<typename Iterator, typename Compare>
        auto operator()(Iterator, Iterator, Compare) const
            -> bool
        {
            return true;
        }

        template<typename Iterator>
        auto operator()(Iterator, Iterator) const
            -> bool
        {
            return false;
        }
    };

    struct non_comparison_sorter_impl
    {
        template<typename Iterator>
        auto operator()(Iterator, Iterator) const
            -> bool
        {
            return true;
        }
    };

    struct non_comparison_iterable_sorter_impl
    {
        template<typename Iterator>
        auto operator()(Iterator, Iterator) const
            -> bool
        {
            return true;
        }

        template<typename Iterable>
        auto operator()(Iterable&) const
            -> bool
        {
            return false;
        }
    };

    struct comparison_projection_sorter_impl
    {
        template<typename Iterator, typename Compare, typename Projection>
        auto operator()(Iterator, Iterator, Compare, Projection) const
            -> bool
        {
            return true;
        }
    };

    struct comparison_sorter:
        cppsort::sorter_facade<comparison_sorter_impl>
    {};

    struct non_comparison_sorter:
        cppsort::sorter_facade<non_comparison_sorter_impl>
    {};

    struct non_comparison_iterable_sorter:
        cppsort::sorter_facade<non_comparison_iterable_sorter_impl>
    {};

    struct comparison_projection_sorter:
        cppsort::sorter_facade<comparison_projection_sorter_impl>
    {};
}

TEST_CASE( "std::less<> forwarding to sorters",
           "[sorter_facade][compare]" )
{
    // Check that sorter_facade only creates the overloads for
    // std::less when the original sorter does not support
    // custom comparison functions

    // Make sure the iterator overload calls the operator() from
    // the sorter for iterators, and that the iterable overload
    // calls the original sorter's iterable operator() overload

    // Equivalent tests are done for the automatic overloads
    // of operator() for std::identity

    std::vector<int> vec(3);

    SECTION( "with std::less<>" )
    {
        CHECK( comparison_sorter{}(vec, std::less<>{}) );
        CHECK( comparison_sorter{}(std::begin(vec), std::end(vec), std::less<>{}) );

        CHECK( non_comparison_sorter{}(vec, std::less<>{}) );
        CHECK( non_comparison_sorter{}(std::begin(vec), std::end(vec), std::less<>{}) );

        CHECK( not non_comparison_iterable_sorter{}(vec, std::less<>{}) );
        CHECK( non_comparison_iterable_sorter{}(std::begin(vec), std::end(vec), std::less<>{}) );
    }

    SECTION( "with std::identity" )
    {
        CHECK( comparison_sorter{}(vec, std::identity{}) );
        CHECK( comparison_sorter{}(std::begin(vec), std::end(vec), std::identity{}) );

        CHECK( non_comparison_sorter{}(vec, std::identity{}) );
        CHECK( non_comparison_sorter{}(std::begin(vec), std::end(vec), std::identity{}) );

        CHECK( not non_comparison_iterable_sorter{}(vec, std::identity{}) );
        CHECK( non_comparison_iterable_sorter{}(std::begin(vec), std::end(vec), std::identity{}) );

        CHECK( comparison_projection_sorter{}(vec, std::identity{}) );
        CHECK( comparison_projection_sorter{}(std::begin(vec), std::end(vec), std::identity{}) );
    }
}
