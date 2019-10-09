/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2018 Morwenn
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <type_traits>
#include <catch2/catch.hpp>
#include <cpp-sort/utility/functional.h>

namespace
{
    constexpr int addition(int a, int b)
    {
        return a + b;
    }

    struct foobar
    {
        int foo = 8;
        int bar() { return 15; }
    };

    // Wrap different entities into function_constant
    using func_t = cppsort::utility::function_constant<addition>;
    using foo_t = cppsort::utility::function_constant<&foobar::foo>;
    using bar_t = cppsort::utility::function_constant<&foobar::bar>;
}

TEST_CASE( "basic tests for function_constant utility",
           "[utility][function_constant]" )
{
    SECTION( "simple functions" )
    {
        CHECK(( std::is_same<int (*)(int, int), func_t::value_type>::value ));
        CHECK( func_t::value == &addition );

        constexpr int (*func)(int, int) = func_t{};
        CHECK( func == &addition );

        constexpr int res = func_t{}(5, 8);
        CHECK( res == 13 );
    }

    SECTION( "member data and functions" )
    {
        CHECK(( std::is_same<int foobar::*, foo_t::value_type>::value ));
        CHECK(( std::is_same<int (foobar::*)(), bar_t::value_type>::value ));
        CHECK( foo_t::value == &foobar::foo );
        CHECK( bar_t::value == &foobar::bar );

        constexpr int foobar::* foo = foo_t{};
        constexpr int (foobar::*bar)() = bar_t{};
        CHECK( foo == &foobar::foo );
        CHECK( bar == &foobar::bar );

        CHECK( foo_t{}(foobar()) == 8 );
        CHECK( bar_t{}(foobar()) == 15 );
    }

    SECTION( "sizeof tests" )
    {
        // function_constant doesn't have any member, so it
        // should have a size of 1, smaller than the size of
        // a pointer

        CHECK( sizeof(func_t) < sizeof(&addition) );
        CHECK( sizeof(foo_t) < sizeof(&foobar::foo) );
        CHECK( sizeof(bar_t) < sizeof(&foobar::bar) );
    }
}

TEST_CASE( "test branchless traits over function_constant",
           "[utility][function_constant][branchless]" )
{
    using namespace cppsort::utility;

    SECTION( "simple functions" )
    {
        CHECK_FALSE(( is_probably_branchless_projection_v<func_t, foobar> ));
    }

    SECTION( "member data and functions" )
    {
        CHECK(( is_probably_branchless_projection_v<foo_t, foobar> ));
        CHECK_FALSE(( is_probably_branchless_projection_v<bar_t, foobar> ));
    }
}
