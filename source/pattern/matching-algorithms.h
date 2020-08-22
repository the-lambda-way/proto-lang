/**
 * @file
 * @author Mike Castillo
 *
 * @section LICENSE
 *
 * Copyright (c) 2019 Mike Castillo. All rights reserved.
 * Licensed under the MIT License. See the LICENSE file for full license information.
 *
 * @section DESCRIPTION
 *
 * A set of algorithms for turning character-based sources into parse trees.
 */

#ifndef MATCHING_ALGORITHMS
#define MATCHING_ALGORITHMS

#include <string_view>
#include <tuple>
#include "concepts-kludge.h"
#include "scanning-algorithms.h"

using std::string_view;
using std::tuple;
using std::size_t;


// ---------------------------------------------------------------------------------------------------------------------
//  Concepts
// ---------------------------------------------------------------------------------------------------------------------
/**
 * An atomic matching algorithm receives a string and a scannable expression; if the associated scanning algorithm
 * applied to the string returns true, the matching algorithm fills a data structure with the match and returns true.
 *
 * @param    f           Function implementing the algorithm
 * @param    out         Output container to hold a positive match
 * @param    first_out   Iterator holding the start of a match
 * @param    last_out    Iterator holding the end of a match
 * @param    first_in    Iterator to the start of a string
 * @param    last_in     Sentinel to the end of the string
 * @param    args...     Arguments passed to the scanning algorithm
 * @param    e           A scannable expression to match the input string against
 */
template <typename Function,
          typename Output,
          forward_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          typename... Args,
          scannable_expression<Iterator, Sentinel, Args...> Expression>
concept bool atomic_matching_algorithm =
    requires (Function f,
              Iterator& first_out, Iterator& last_out,
              Iterator& first_in, Sentinel last_in,
              Expression e, Args&&... args)
    {
        { f(first_out, last_out, first_in, last_in, e, std::forward<Args>(args)...) } -> bool;
    }
    ||
    requires (Function f,
              Output& out,
              Iterator& first_in, Sentinel last_in,
              Expression e, Args&&... args)
    {
        { f(out, first_in, last_in, e, std::forward<Args>(args)...) } -> bool;
    };


/**
 * A compound matching algorithm receives a string and multiple scannable expressions; if all the associated scanning
 * algorithm applied to the string in succession return true, the matching algorithm fills a data structure with the
 * series of matches and returns true.
 *
 * @param    f           Function implementing the algorithm
 * @param    out         Output container to hold the outputs from each expression
 * @param    first_in    Iterator to the start of a string
 * @param    last_in     Sentinel to the end of the string
 * @param    e           A series of scannable expressions to match the input string against in succession
 */
template <typename Function,
          typename Output,
          forward_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          scannable_expression<Iterator, Sentinel>... Expressions>
concept bool compound_matching_algorithm =
    requires (Function f,
              Ouput& out,
              Iterator& first_in, Sentinel last_in,
              Expressions... e)
    {
        { f(out, first_in, last_in, e...) } -> bool;
    };


/**
 * Concept for a matching algorithm.
 */
template <typename... Ts>
concept bool matching_algorithm =
    atomic_matching_algorithm<Ts...> ||
    compound_matching_algorithm<Ts...>;


// ---------------------------------------------------------------------------------------------------------------------
//  Algorithms
// ---------------------------------------------------------------------------------------------------------------------
template <forward_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          typename... Args,
          atomic_scannable_expression<Iterator, Sentinel, Args...> Expression>
bool match_with_if (Iterator& first_out, Iterator& last_out,
                    Iterator& first_in, Sentinel last_in,
                    Expression e, Args&&... args)
{
    if (!scan_with(first_in, last_in, e, forward<Args>(args)...))    return false;

    first_out = first_in;
    ++first_in;
    last_out = first_in;
    return true;
}


template <forward_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          typename... Args,
          compound_scannable_expression<Iterator, Sentinel, Args...> Expression>
bool match_with_if (Iterator& first_out, Iterator& last_out,
                    Iterator& first_in, Sentinel last_in,
                    Expression e, Args&&... args)
{
    Iterator first_out = first_in;

    if (!advance_if(first_in, last_in, e, forward<Args>(args)...))    return false;

    last_out = first_in;
    return true;
}


template <forward_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          typename... Args,
          scannable_expression<Iterator, Sentinel, Args...> Expression>
bool match_with_if (std::pair<Iterator&, Iterator&> output,
                    Iterator& first_in, Sentinel last_in,
                    Expression e, Args&&... args)
{
    return match_with_if(output.left, output.right, first_in, first_out, e, forward<Args>(args)...);
}


template <forward_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          typename... Args,
          scannable_expression<Args...> Expression>
bool match_with_if (string_view& output,
                    Iterator& first_in, Sentinel last_in,
                    Expression e, Args&&... args)
{
    Iterator copy = first_in;

    if (!advance_if(first_in, last_in, e, forward<Args>(args)...))    return false;

    output = {&*copy, first_in - copy};
    return true;
}


template <forward_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          typename... Args,
          scannable_expression<Args...> Expression>
bool match_with_if (std::string& output,
                    Iterator& first_in, Sentinel last_in,
                    Expression e, Args&&... args)
{
    Iterator copy = first_in;

    if (!advance_if(first_in, last_in, e, forward<Args>(args)...))    return false;

    output = {&*copy, first_in};
    return true;
}


template <size_t N,
          forward_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          typename... Args,
          scannable_expression<Args...> Expression>
bool match_with_if (char (&output)[N],
                    Iterator& first_in, Sentinel last_in,
                    Expression e, Args&&... args)
{
    Iterator copy = first_in;

    if (!advance_if(first_in, last_in, e, forward<Args>(args)...))    return false;

    int i = 0;
    while (copy != first_in)    output[i++] = *copy++;
    output[i] = '\0';

    return true;
}


// ---------------------------------------------------------------------------------------------------------------------
// Compound algorithms
// ---------------------------------------------------------------------------------------------------------------------
namespace Detail
{
    template <forward_iterator Iterator,
            sentinel_for<Iterator> Sentinel,
            scannable_expression<Iterator, Sentinel> Expression>
    bool delegate_expression_kind (output_iterator& out,
                                   Iterator& first_in, Sentinel last_in,
                                   Expression e)
    {
        return scan_with(first_in, last_in, e, forward<Args>(args)...);
    }
    

    template <forward_iterator Iterator,
              sentinel_for<Iterator> Sentinel,
              matching_algorithm<Iterator, Sentinel> Expression>
    bool delegate_expression_kind (output_iterator& out,
                                   Iterator& first_in, Sentinel last_in,
                                   Expression e)
    {
        return match_with_if(*out++, first_in, last_in, e);
    }


    template <forward_iterator Iterator,
              sentinel_for<Iterator> Sentinel,
              typename Tuple,
              size_t I = 0>
    bool compound_match_with_if (output_iterator& out,
                                 Iterator& first_in, Sentinel last_in,
                                 Tuple&& expressions)
    {
        if (first_in == last_in)    return true;

        if (!delegate_expression_kind(out, first_in, last_in, std::get<I>(expressions)))    return false;

        return compound_match_with_if<I + 1>(out, first_in, last_in, expressions);
    }

} // namespace Detail


template <forward_iterator Iterator,
          sentinel_for<Iterator> Sentinel>
bool match_with_if (output_iterator&& outputs,
                    Iterator&& first_in, Sentinel&& last_in,
                    scannable_expression&& e...)
{
    return compound_match_with_if(forward<decltype(outputs>(outputs),
                                  forward<Iterator>(first_in), forward<Sentinel>(last_in),
                                  forward<decltype(e)>(e)...));
}


namespace Detail
{
    
    template <typename... T, size_t I,
              forward_iterator Iterator,
              sentinel_for<Iterator> Sentinel>
    bool compound_match_with_if (tuple<T...>& outputs,
                                 Iterator& first_in, Sentinel last_in)
    {
        return true;
    }


    template <typename... T, size_t I,
              forward_iterator Iterator,
              sentinel_for<Iterator> Sentinel,
              scannable_expression<Iterator, Sentinel> Expression,
              typename... Expressions>
    bool compound_match_with_if (tuple<T...>& outputs,
                                 Iterator& first_in, Sentinel last_in,
                                 Expression first_expr, Expressions... rest_expr)
    {
        if (!scan_with(first_in, last_in, first_expr))    return false;

        return compound_match_with_if<I>(outputs, first_in, last_in, rest_expr...);
    }
    

    template <typename... T, size_t I,
              forward_iterator Iterator,
              sentinel_for<Iterator> Sentinel,
              matching_algorithm<Iterator, Sentinel> Expression
              typename... Expressions>
    bool compound_match_with_if (tuple<T...>& outputs,
                                 Iterator& first_in, Sentinel last_in,
                                 Expression first_expr, Expressions... rest_expr)
    {
        if (!match_with_if(std::get<I>(outputs), first_in, last_in, first_expr))    return false;

        return compound_match_with_if<I + 1>(outputs, first_in, last_in, rest_expr...);
    }

} // namespace Detail


template <typename... T,
          forward_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          typename... Expression>
bool match_with_if (tuple<T...>&& outputs,
                    Iterator&& first_in, Sentinel&& last_in,
                    Expression e...)
{
    return compound_match_with_if(forward<tuple<T...>>(outputs),
                                  forward<Iterator>(first_in), forward<Sentinel>(last_in),
                                  forward<Expression>(e)...);
}


namespace Detail
{
    // *keep_going* indicates status of the parsing operation
    // tuple<> is used to skip storing output (for scan-only expressions)

    template <forward_iterator Iterator,
              sentinel_for<Iterator> Sentinel,
              scannable_expression<Iterator, Sentinel> Expression,
              typename OutputType>
    tuple<> generate_output (bool& keep_going,
                             Iterator& first_in, Sentinel last_in,
                             Expression e)
    {
        if (keep_going && !advance_if(first_in, last_in, e)))    keep_going = false
        return {};
    }
    

    template <forward_iterator Iterator,
              sentinel_for<Iterator> Sentinel,
              matching_algorithm<Iterator, Sentinel> Expression
              typename OutputType>
    tuple<OutputType> generate_output (bool& keep_going,
                                       Iterator& first_in, Sentinel last_in,
                                       Expression e)
    {
        OutputType out;
        if (keep_going && !match_with_if(out, first_in, last_in, e))    keep_going = false;
        
        return make_tuple(out);
    }

} // namespace Detail


template <forward_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          typename... Expression,
          typename OutputType>
auto generate_parse_tree_for (Iterator& first_in, Sentinel last_in,
                              Expression e...)
{
    bool keep_going = true;

    // Using recursion we could short-circuit this. However, this would require O(N^2) operations due to nesting
    // concatenation, as opposed to O(N) with the parameter pack expansion.
    auto out = std::tuple_cat(Detail::generate_output<OutputType>(keep_going, first_in, last_in, e)...);

    if (!keep_going)    return tuple<>{};
    return out;
}


// ---------------------------------------------------------------------------------------------------------------------
// Ranged Versions
// ---------------------------------------------------------------------------------------------------------------------
template <forward_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          typename... Args,
          scannable_expression<Iterator, Sentinel, Args...> Expression>
bool match_with_if (Iterator& first_out, Iterator& last_out,
                    mutable_range& in,
                    Expression e, Args&&... args)
{
    return match_with_if(first_out, last_out, in.begin(), in.end(), e, forward<Args>(args)...);
}


template <forward_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          typename... Args,
          scannable_expression<Iterator, Sentinel, Args...> Expression>
bool match_with_if (std::pair<Iterator&, Iterator&> output,
                    mutable_range& in,
                    Expression e, Args&&... args)
{
    return match_with_if(output, in.begin(), in.end(), e, forward<Args>(args)...);
}


template <forward_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          typename... Args,
          scannable_expression<Iterator, Sentinel, Args...> Expression>
bool match_with_if (string_view& output, mutable_range& in,
                    Expression e, Args&&... args)
{
    return match_with_if(output, in.begin(), in.end(), e, forward<Args>(args)...);
}


template <forward_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          typename... Args,
          scannable_expression<Iterator, Sentinel, Args...> Expression>
bool match_with_if (std::string& output, mutable_range& in,
                    Expression e, Args&&... args)
{
    return match_with_if(output, in.begin(), in.end(), e, forward<Args>(args)...);
}


template <size_t N,
          forward_iterator Iterator,
          sentinel_for<Iterator> Sentinel,
          typename... Args,
          scannable_expression<forward_iterator, Sentinel, Args...> Expression>
bool match_with_if (char (&output)[N], mutable_range& in,
                    Expression e, Args&&... args)
{
    return match_with_if(output, in.begin(), in.end(), e, forward<Args>(args)...);
}


template <scannable_expression... Expression>
bool match_with_if (output_iterator outputs, mutable_range& in, Expression e...)
{
    return match_with_if(outputs, in.begin(), in.end(), e...);
}


template <typename... T,
          scannable_expression... Expression>
bool match_with_if (tuple<T...>& outputs, mutable_range& in, Expression e...)
{
    return match_with_if(outputs, in.begin(), in.end(), e...);
}


template <forward_iterator Iterator,
          typename OutputType>
auto generate_parse_tree_for (mutable_range& in, Expression e...)
{
    return generate_parse_tree_for(in.begin(), in.end(), e...);
}


#endif // MATCHING_ALGORITHMS
