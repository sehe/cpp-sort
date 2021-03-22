/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2018 Morwenn
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
#include <catch2/catch.hpp>
#include <cpp-sort/utility/as_function.h>
#include <cpp-sort/utility/functional.h>
#include <cpp-sort/detail/projection_compare.h>

TEST_CASE( "Issue 139",
           "[issue_139][projection_compare]" )
{
    // minimal fixture
    namespace detail  = cppsort::detail;
    namespace utility = cppsort::utility;

    utility::identity proj;
    //// same with:
    // auto proj = utility::as_function(utility::identity{});

    using Iterator = double const*;
    double   value = 0;
    Iterator it    = &value;

    SECTION( "define iterproj based on proj" )
    {
        /*
         * Projection taken from indirect_adapter:
         *
         * return Sorter{}(
         *     std::begin(iterators), std::end(iterators), std::move(compare),
         *     [&proj](RandomAccessIterator it) -> decltype(auto) { return proj(*it); });
         *
         * We add a `paranoia-check` projptr to check whether the reference (or
         * pointer) mutates
         */

        auto iterproj =
            [&proj, projptr = std::addressof(proj)](Iterator it) -> decltype(auto) {

            // Manifests the WTF
            CHECK(std::addressof(proj) == projptr);

            return 42; // (*projptr)(*it); not even required
        };

        SECTION( "demo iterproj is sane" ) {
            iterproj(it);
        }

        SECTION( "copy of iterproj is sane" ) {
            auto copy = iterproj;
            copy(it);
        }

        SECTION(  "using projection_compare instead" ) {
            detail::projection_compare<std::less<>, decltype(iterproj)> prjcomp(
                {}, iterproj);

            prjcomp.projection()(it);
            prjcomp.projection()(it);

            SECTION( "iterproj still sane" ) { iterproj(it); }
        }

        SECTION(" adhoc approach ") {
            // This tried to reduce projection_compare to its suspected
            // culprit, but is actually behaving just fine
            auto adhoc =
                [data =
                     std::tuple<decltype(utility::as_function(std::less<>{})),
                                decltype(utility::as_function(iterproj))>(
                         std::less<>{}, iterproj)](Iterator a, Iterator b) {
                    return std::get<0>(data)(std::get<1>(data)(a),
                                             std::get<1>(data)(b));
                };

            CHECK(not adhoc(it, it)); // invokes iterproj twice

            SECTION(" iterproj still sane ") { iterproj(it); }
        }
    }
}
