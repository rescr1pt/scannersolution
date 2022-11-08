#pragma once

#include <list>

template <typename T>
struct ElementRanges
{
    typedef std::pair<T, T> Range;
    typedef std::list<T> ListElement;
    typedef std::list<Range> ListRange;

    ListElement elements_;
    ListRange ranges_;
};
