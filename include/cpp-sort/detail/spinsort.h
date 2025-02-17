/*
 * Copyright (c) 2019-2021 Morwenn
 * SPDX-License-Identifier: MIT
 */

//----------------------------------------------------------------------------
/// @file spinsort.hpp
/// @brief Spin Sort algorithm
///
/// @author Copyright (c) 2016 Francisco José Tapia (fjtapia@gmail.com )
///         Distributed under the Boost Software License, Version 1.0.
///         ( See accompanying file LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------

#ifndef CPPSORT_DETAIL_SPINSORT_H_
#define CPPSORT_DETAIL_SPINSORT_H_

#include <cstddef>
#include <cstdint>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>
#include <vector>
#include <cpp-sort/utility/as_function.h>
#include <cpp-sort/utility/iter_move.h>
#include "boost_common/util/merge.h"
#include "boost_common/range.h"
#include "bitops.h"
#include "config.h"
#include "functional.h"
#include "insertion_sort.h"
#include "is_sorted_until.h"
#include "iterator_traits.h"
#include "memory.h"
#include "move.h"
#include "type_traits.h"
#include "upper_bound.h"

namespace cppsort
{
namespace detail
{
    namespace spin_detail
    {
        using boost_common::range;

        template<typename RandomAccessIterator1, typename RandomAccessIterator2, typename Compare, typename Projection>
        auto sort_range_sort(const range<RandomAccessIterator1>& rng_data, const range<RandomAccessIterator2>& rng_aux,
                             Compare compare, Projection projection)
            -> void;

        //-----------------------------------------------------------------------------
        // function : insert_partial_sort
        // @brief : Insertion sort of elements sorted
        // @param first: iterator to the first element of the range
        // @param mid : last pointer of the sorted data, and first pointer to the
        //               elements to insert
        // @param last : iterator to the next element of the last in the range
        // @param compare : comparison function
        // @param projection : projection function
        // @comments : the two ranges are sorted
        //-----------------------------------------------------------------------------
        template<typename RandomAccessIterator1, typename RandomAccessIterator2, typename Compare, typename Projection>
        auto insert_partial_sort(RandomAccessIterator1 first, RandomAccessIterator1 mid, RandomAccessIterator1 last,
                                 Compare compare, Projection projection,
                                 const range<RandomAccessIterator2>& rng_aux)
            -> void
        {
            using utility::iter_move;

            CPPSORT_ASSERT(std::size_t(last - mid) <= rng_aux.size());
            if (mid == last || first == mid) {
                return;
            }

            auto&& proj = utility::as_function(projection);

            //------------------------------------------------------------------------
            // creation of the vector of elements to insert and their position in the
            // sorted part
            // the data are inserted in rng_aux
            //-----------------------------------------------------------------------
            std::vector<RandomAccessIterator1> viter;
            auto beta = rng_aux.first;
            auto data = rng_aux.first;

            for (auto alpha = mid ; alpha != last ; ++alpha) {
                *beta = iter_move(alpha);
                ++beta;
            }

            std::size_t ndata = last - mid;

            RandomAccessIterator1 linf = first, lsup = mid;
            for (std::uint32_t i = 0 ; i < ndata ; ++i) {
                auto it1 = detail::upper_bound(linf, lsup, proj(*(data + i)), compare, projection);
                viter.push_back(it1);
                linf = it1;
            }

            // moving the elements
            viter.push_back(mid);
            for (std::uint32_t i = viter.size() - 1; i != 0; --i) {
                RandomAccessIterator1 src = viter[i], limit = viter[i - 1];
                RandomAccessIterator1 dest = src + i;
                while (src != limit) {
                    *(--dest) = iter_move(--src);
                }
                *(viter[i - 1] + (i - 1)) = iter_move(data + (i - 1));
            }
        }

        //-----------------------------------------------------------------------------
        //  function : check_stable_sort
        // @brief check if the elements between first and last are sorted or reverse
        //        sorted. If the number of elements not sorted is small, insert in
        //        the sorted part
        // @param range_input : range with the elements to sort
        // @param range_buffer : range with the elements sorted
        // @param comp : object for to compare two elements
        // @param level : when is 1, sort with the insertionsort algorithm
        //                if not make a recursive call splitting the ranges
        //
        // @comments : if the number of levels is odd, the data are in the first
        // parameter of range_sort, and the results appear in the second parameter
        // If the number of levels is even, the data are in the second
        // parameter of range_sort, and the results are in the same parameter
        //-----------------------------------------------------------------------------
        template<typename RandomAccessIterator1, typename RandomAccessIterator2, typename Compare, typename Projection>
        auto check_stable_sort(const range<RandomAccessIterator1>& rng_data, const range<RandomAccessIterator2>& rng_aux,
                               Compare compare, Projection projection)
            -> bool
        {
            // the maximun number of elements not ordered, for to be inserted in the
            // sorted part
            //const ptrdiff_t  min_insert_partial_sort = 32 ;
            const std::size_t ndata = rng_data.size();
            if (ndata < 32) {
                insertion_sort(rng_data.first, rng_data.last, compare, projection);
                return true;
            }
            const std::size_t min_insert_partial_sort = ((ndata >> 3) < 33) ? 32 : (ndata >> 3);
            if (ndata < 2) return true; // TODO: redundant?

            // check if sorted
            auto it = detail::is_sorted_until(rng_data.first, rng_data.last, compare, projection);
            if (it == rng_data.last) {
                return true;
            }

            // insert the elements between it1 and last
            if (std::size_t(rng_data.last - it) < min_insert_partial_sort) {
                sort_range_sort(range<RandomAccessIterator1>(it, rng_data.last), rng_aux, compare, projection);
                insert_partial_sort(rng_data.first, it, rng_data.last, compare, projection, rng_aux);
                return true;
            }

            // check if reverse sorted
            if (it != std::next(rng_data.first)) {
                return false;
            }

            it = detail::is_sorted_until(rng_data.first, rng_data.last, detail::not_fn(compare), projection);
            if (std::size_t(rng_data.last - it) >= min_insert_partial_sort) {
                return false;
            }

            // reverse the elements between first and it1
            std::size_t nreverse = it - rng_data.first;
            auto beta = std::prev(it);
            auto mid = rng_data.first + (nreverse >> 1);
            for (auto alpha = rng_data.first ; alpha != mid ; ++alpha) {
                using utility::iter_swap;
                iter_swap(alpha, beta);
                --beta;
            }

            // insert the elements between it1 and last
            if (it != rng_data.last) {
                sort_range_sort(range<RandomAccessIterator1>(it, rng_data.last), rng_aux, compare, projection);
                insert_partial_sort(rng_data.first, it, rng_data.last, compare, projection, rng_aux);
            }
            return true;
        }

        //-----------------------------------------------------------------------------
        //  function : range_sort
        // @brief this function divide r_input in two parts, sort it,and merge moving
        //        the elements to range_buf
        // @param range_input : range with the elements to sort
        // @param range_buffer : range with the elements sorted
        // @param comp : object for to compare two elements
        // @param level : when is 1, sort with the insertionsort algorithm
        //                if not make a recursive call splitting the ranges
        //
        // @comments : if the number of levels is odd, the data are in the first
        // parameter of range_sort, and the results appear in the second parameter
        // If the number of levels is even, the data are in the second
        // parameter of range_sort, and the results are in the same parameter
        // The two ranges must have the same size
        //-----------------------------------------------------------------------------
        template<typename RandomAccessIterator1, typename RandomAccessIterator2, typename Compare, typename Projection>
        auto range_sort(const range<RandomAccessIterator1>& range1, const range<RandomAccessIterator2>& range2,
                        Compare compare, Projection projection, std::uint32_t level)
            -> void
        {
            using range_it1 = range<RandomAccessIterator1>;
            using range_it2 = range<RandomAccessIterator2>;
            CPPSORT_ASSERT(range1.size() == range2.size() && level != 0);

            //------------------- check if sort --------------------------------------
            if (range1.size() > 1024) {
                if ((level & 1) == 0) {
                    if (check_stable_sort(range2, range1, compare, projection)) return;
                } else {
                    if (check_stable_sort(range1, range2, compare, projection)) {
                        move_forward(range2, range1);
                        return;
                    }
                }
            }

            //------------------- normal process -----------------------------------
            std::size_t nelem1 = (range1.size() + 1) >> 1;
            range_it1 range_input1(range1.first, range1.first + nelem1),
                                   range_input2(range1.first + nelem1, range1.last);

            if (level < 2) {
                insertion_sort(range_input1.first, range_input1.last, compare, projection);
                insertion_sort(range_input2.first, range_input2.last, compare, projection);
            } else {
                range_sort(range_it2(range2.first, range2.first + nelem1),
                           range_input1, compare, projection, level - 1);

                range_sort(range_it2(range2.first + nelem1, range2.last),
                           range_input2, compare, projection, level - 1);
            }

            merge(range2, range_input1, range_input2,
                  std::move(compare), std::move(projection));
        }

        //-----------------------------------------------------------------------------
        //  function : sort_range_sort
        // @brief this sort elements using the range_sort function and receiving a
        //        buffer of initialized memory
        // @param rng_data : range with the elements to sort
        // @param rng_aux : range of at least the same memory than rng_data used as
        //                  auxiliary memory in the sorting
        // @param comp : object for to compare two elements
        //-----------------------------------------------------------------------------
        template<typename RandomAccessIterator1, typename RandomAccessIterator2, typename Compare, typename Projection>
        auto sort_range_sort(const range<RandomAccessIterator1>& rng_data, const range<RandomAccessIterator2>& rng_aux,
                             Compare compare, Projection projection)
            -> void
        {
            // minimal number of element before to jump to insertionsort
            static const std::uint32_t sort_min = 32;
            if (rng_data.size() <= sort_min) {
                insertion_sort(rng_data.first, rng_data.last, compare, projection);
                return;
            }

            CPPSORT_ASSERT(rng_aux.size () >= rng_data.size());
            range<RandomAccessIterator2> rng_buffer(rng_aux.first, rng_aux.first + rng_data.size());
            CPPSORT_ASSERT(((rng_data.size() + sort_min - 1) / sort_min) - 1 > 0);
            std::uint32_t nlevel = detail::log2(((rng_data.size() + sort_min - 1) / sort_min) - 1) + 1;

            if ((nlevel & 1) == 0) {
                range_sort(rng_buffer, rng_data, compare, projection, nlevel);
            } else {
                range_sort(rng_data, rng_buffer, compare, projection, nlevel);
                move_forward(rng_data, rng_buffer);
            }
        }

        ////////////////////////////////////////////////////////////
        // spinsort

        //---------------------------------------------------------------------------
        // @struct spin_sort
        // @brief  This class implement s stable sort algorithm with 1 thread, with
        //         an auxiliary memory of N/2 elements
        //----------------------------------------------------------------------------
        template<typename RandomAccessIterator, typename Compare, typename Projection>
        class spinsort
        {
            private:

                using range_it = range<RandomAccessIterator>;
                using rvalue_type = rvalue_type_t<RandomAccessIterator>;
                using range_buf = range<rvalue_type*>;

                // When the number of elements to sort is smaller than Sort_min, are sorted
                // by the insertion sort algorithm
                static constexpr std::uint32_t Sort_min = 36;

                // Pointer to the auxiliary memory
                std::unique_ptr<rvalue_type, operator_deleter> ptr;

                // Number of elements in the auxiliary memory
                std::size_t nptr;

            public:

                //-------------------------------------------------------------------------
                //  function : spinsort
                // @brief constructor
                //
                // @param first : iterator to the first element of the range to sort
                // @param last : iterator after the last element to the range to sort
                // @param compare : object for to compare two elements pointed by RandomAccessIterator
                //                  iterators
                // @param projection : projection object
                //------------------------------------------------------------------------
                spinsort(RandomAccessIterator first, RandomAccessIterator last,
                         Compare compare, Projection projection):
                    ptr(nullptr),
                    nptr(0)
                {
                    using utility::iter_move;

                    range<RandomAccessIterator> range_input(first, last);
                    CPPSORT_ASSERT(range_input.valid());

                    std::size_t nelem = range_input.size();
                    if (nelem <= (Sort_min << 1)) {
                        insertion_sort(range_input.first, range_input.last,
                                       std::move(compare), std::move(projection));
                        return;
                    }

                    nptr = (nelem + 1) >> 1;
                    std::size_t nelem_1 = nptr;
                    std::size_t nelem_2 = nelem - nelem_1;
                    ptr.reset(static_cast<rvalue_type*>(
                        ::operator new(nptr * sizeof(rvalue_type))
                    ));
                    range_buf range_aux(ptr.get(), (ptr.get() + nptr));

                    destruct_n<rvalue_type> d(0);
                    std::unique_ptr<rvalue_type, destruct_n<rvalue_type>&> h2(ptr.get(), d);

                    //---------------------------------------------------------------------
                    //                  Process
                    //---------------------------------------------------------------------
                    std::uint32_t nlevel = detail::log2(((nelem + Sort_min - 1) / Sort_min) - 1);
                    CPPSORT_ASSERT(nlevel != 0);

                    if ((nlevel & 1) == 1) {
                        //----------------------------------------------------------------
                        // if the number of levels is odd, the data are in the first
                        // parameter of range_sort, and the results appear in the second
                        // parameter
                        //----------------------------------------------------------------
                        range_it range_1(first, first + nelem_2), range_2(first + nelem_2, last);
                        range_aux.last = uninitialized_move(range_2.first, range_2.last, range_aux.first, d);

                        range_sort(range_aux, range_2, compare, projection, nlevel);
                        range_buf rng_bx(range_aux.first, range_aux.first + nelem_2);

                        range_sort(range_1, rng_bx, compare, projection, nlevel);
                        merge_half(range_input, rng_bx, range_2, compare, projection);
                    } else {
                        //----------------------------------------------------------------
                        // If the number of levels is even, the data are in the second
                        // parameter of range_sort, and the results are in the same
                        //  parameter
                        //----------------------------------------------------------------
                        range_it range_1(first, first + nelem_1);
                        range_it range_2(first + nelem_1, last);
                        range_aux.last = uninitialized_move(range_1.first, range_1.last, range_aux.first, d);

                        range_sort(range_1, range_aux, compare, projection, nlevel);
                        range_1.last = range_1.first + range_2.size();
                        range_sort(range_1, range_2, compare, projection, nlevel);
                        merge_half(range_input, range_aux, range_2, compare, projection);
                    }
                }
        };
    }

    //-----------------------------------------------------------------------------
    //  function : spinsort
    // @brief this function implement a single thread stable sort
    //
    // @param first : iterator to the first element of the range to sort
    // @param last : iterator after the last element to the range to sort
    // @param comp : object for to compare two elements pointed by RandomAccessIterator
    //               iterators
    //-----------------------------------------------------------------------------
    template<typename RandomAccessIterator, typename Compare, typename Projection>
    auto spinsort(RandomAccessIterator first, RandomAccessIterator last,
                  Compare compare, Projection projection)
        -> void
    {
        spin_detail::spinsort<RandomAccessIterator, Compare, Projection>(
            std::move(first), std::move(last),
            std::move(compare), std::move(projection)
        );
    }
}}

#endif // CPPSORT_DETAIL_SPINSORT_H_
