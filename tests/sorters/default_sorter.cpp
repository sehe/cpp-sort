/*
 * Copyright (c) 2015-2020 Morwenn
 * SPDX-License-Identifier: MIT
 */
#include <algorithm>
#include <forward_list>
#include <functional>
#include <iterator>
#include <list>
#include <vector>
#include <catch2/catch.hpp>
#include <cpp-sort/sorters/default_sorter.h>
#include <testing-tools/distributions.h>

TEST_CASE( "default sorter tests", "[default_sorter]" )
{
    // Collection to sort
    std::vector<int> vec; vec.reserve(80);
    auto distribution = dist::shuffled{};
    distribution(std::back_inserter(vec), 80, 0);

    cppsort::default_sorter sorter;

    SECTION( "sort with random-access iterable" )
    {
        sorter(vec);
        CHECK( std::is_sorted(std::begin(vec), std::end(vec)) );
    }

    SECTION( "sort with random-access iterable and compare" )
    {
        sorter(vec, std::greater<>{});
        CHECK( std::is_sorted(std::begin(vec), std::end(vec), std::greater<>{}) );
    }

    SECTION( "sort with random-access iterators" )
    {
        sorter(std::begin(vec), std::end(vec));
        CHECK( std::is_sorted(std::begin(vec), std::end(vec)) );
    }

    SECTION( "sort with random-access iterators and compare" )
    {
        sorter(std::begin(vec), std::end(vec), std::greater<>{});
        CHECK( std::is_sorted(std::begin(vec), std::end(vec), std::greater<>{}) );
    }

    SECTION( "sort with bidirectional iterators" )
    {
        std::list<int> li(std::begin(vec), std::end(vec));
        sorter(std::begin(li), std::end(li));
        CHECK( std::is_sorted(std::begin(li), std::end(li)) );
    }

    SECTION( "sort with bidirectional iterators and compare" )
    {
        std::list<int> li(std::begin(vec), std::end(vec));
        sorter(std::begin(li), std::end(li), std::greater<>{});
        CHECK( std::is_sorted(std::begin(li), std::end(li), std::greater<>{}) );
    }

    SECTION( "sort with forward iterators" )
    {
        std::forward_list<int> li(std::begin(vec), std::end(vec));
        sorter(std::begin(li), std::end(li));
        CHECK( std::is_sorted(std::begin(li), std::end(li)) );
    }

    SECTION( "sort with forward iterators and compare" )
    {
        std::forward_list<int> li(std::begin(vec), std::end(vec));
        sorter(std::begin(li), std::end(li), std::greater<>{});
        CHECK( std::is_sorted(std::begin(li), std::end(li), std::greater<>{}) );
    }

    SECTION( "sort with self-sortable iterable" )
    {
        std::list<int> li(std::begin(vec), std::end(vec));
        sorter(li);
        CHECK( std::is_sorted(std::begin(li), std::end(li)) );
    }

    SECTION( "sort with self-sortable iterable and compare" )
    {
        std::forward_list<int> li(std::begin(vec), std::end(vec));
        sorter(li, std::greater<>{});
        CHECK( std::is_sorted(std::begin(li), std::end(li), std::greater<>{}) );
    }
}
