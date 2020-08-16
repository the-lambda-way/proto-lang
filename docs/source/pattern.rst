
########################################################################################################################
Pattern Library
########################################################################################################################

************************************************************************************************************************
Overview
************************************************************************************************************************

The Pattern Library is a toolkit for building recursive descent parsers, offering a range of features from low-level pointer manipulation, to high-level, grammar-like abstractions. Traditional parser generators embed semantic actions within a domain-specific parsing language. Instead, the Pattern Library is designed to embed parsers within normal C++ code. Thus, the custom needs of an application can be met using a mixture of high level and low level code as appropriate.

This library follows the principles of the C++ Standard Library, so that it fits naturally within a variety of program designs. Specifically, it avoids incidental runtime costs, prefers value semantics, uses generic algorithms where appropriate, and is compatible with the <algorithms> library, as well as ranges, views, and range adapters.

TODO: add to description



.. toctree::
    :maxdepth: 2
    :caption: Contents:

    concepts
    fn-combinators
    scanning-algorithms
    scan_view
