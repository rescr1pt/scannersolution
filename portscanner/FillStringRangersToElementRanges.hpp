#pragma once

#include <list>
#include <string>
#include <functional>

#include "ElementRanges.h"

// ELEMENT_T - used type in ElementRanges<T>
// ELEMENT_DECIMAL_T - is native decimal type of the ELEMENTS_T. Used for comparing 
// function for fill the ElementRanges with sorting and without duplicates

template <typename ELEMENT_T, typename ELEMENT_DECIMAL_T>
void fillStringRangesToElementRanges(const ElementRanges<std::string>& stringRanges
    , ElementRanges<ELEMENT_T>& elRangesSkip
    , ElementRanges<ELEMENT_T>& elRanges
    , std::function<void(ELEMENT_T&, const std::string&)> stringToElement
    , std::function<ELEMENT_DECIMAL_T& (ELEMENT_T&)> elementToDecimal)
{
    auto& sElements = stringRanges.elements_;
    auto& sRanges = stringRanges.ranges_;
    auto& ranges = elRanges.ranges_;
    auto& elements = elRanges.elements_;
    auto& rangesSkip = elRangesSkip.ranges_;
    auto& elementsSkip = elRangesSkip.elements_;

    // fill the ElementRanges<T>.ranges
    for (const auto& pr : sRanges) {
        ELEMENT_T rangeStart;
        stringToElement(rangeStart, pr.first);
        ELEMENT_T rangeEnd;
        stringToElement(rangeEnd, pr.second);

        ELEMENT_DECIMAL_T& nativeTypeStart = elementToDecimal(rangeStart);
        ELEMENT_DECIMAL_T& nativeTypeEnd = elementToDecimal(rangeEnd);

        // Check self duplicates
        auto fndIt = std::find_if(ranges.begin(), ranges.end(), [nativeTypeStart, nativeTypeEnd, elementToDecimal](auto& pr)
        {
            return nativeTypeStart == elementToDecimal(pr.first) && nativeTypeEnd == elementToDecimal(pr.second);
        });

        ranges.emplace_back(rangeStart, rangeEnd);
    }

    // fill the ElementRanges<T>.elements
    for (const auto& p : sElements) {
        ELEMENT_T element;
        stringToElement(element, p);
        ELEMENT_DECIMAL_T& nativeType = elementToDecimal(element);

        // Check self duplicates
        auto fndIt = std::find_if(elements.begin(), elements.end(), [nativeType, elementToDecimal](auto& pr)
        {
            return nativeType == elementToDecimal(pr);
        });

        if (fndIt == elements.end()) {
            elements.emplace_back(element);
        }
    }

    // remove skipped ranges
    for (auto& sRange : rangesSkip) {
        auto& skipA = elementToDecimal(sRange.first);
        auto& skipB = elementToDecimal(sRange.second);

        for (auto it = ranges.begin(); it != ranges.end(); ) {
            auto& rA = elementToDecimal(it->first);
            auto& rB = elementToDecimal(it->second);

            if (skipA <= rA) {
                // [20-40]: -[10-15]: 20-40
                if (skipB < rA) {
                    ++it;
                }
                else {
                    // [20-40]: -[15-45]: removed
                    if (skipB >= rB) {
                        it = ranges.erase(it);
                    }
                    // [20-40]: -[10-25]: 26-40
                    else {
                        rA = skipB + 1;
                        ++it;
                    }
                }
            }
            else {
                // [20-40]: -[42-50]: 20-40
                if (skipA > rB) {
                    ++it;
                }
                // [20-40]: -[25-42]: 20-24
                else if (skipB >= rB) {
                    rB = skipA - 1;
                    ++it;
                }
                // -[25-30]: 20-24, 31-40
                else {
                    ranges.push_front(*it);
                    elementToDecimal(ranges.front().second) = skipA - 1;
                    rA = skipB + 1;
                    ++it;
                }
            }
        }

        for (auto it = elements.begin(); it != elements.end();) {
            auto& el = elementToDecimal(*it);
            if (el >= skipA && el <= skipB) {
                it = elements.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    for (auto& sEl : elementsSkip) {
        auto& skip = elementToDecimal(sEl);

        for (auto it = ranges.begin(); it != ranges.end(); ) {
            auto& rA = elementToDecimal(it->first);
            auto& rB = elementToDecimal(it->second);

            // [20-40]: -[19]: 20-40 
            if (skip < rA) {
                ++it;
            }
            // [20-40]: -[20]: 21-40 
            else if (skip == rA) {
                ++rA;
                ++it;
            }
            else {
                // [20-40]: -[41]: 20-40 
                if (skip > rB) {
                    ++it;
                }
                // [20-40]: -[22]: 20-21, 23-40 
                else {
                    ranges.push_front(*it);
                    elementToDecimal(ranges.front().second) = skip - 1;

                    rA = skip + 1;
                    ++it;
                }
            }
        }

        for (auto it = elements.begin(); it != elements.end();) {
            auto& el = elementToDecimal(*it);
            if (skip == el) {
                it = elements.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    // Sort after exclude skipped elements
    ranges.sort([elementToDecimal](std::pair<ELEMENT_T, ELEMENT_T>& a, std::pair<ELEMENT_T, ELEMENT_T>& b)
    {
        return elementToDecimal(a.first) < elementToDecimal(b.first);
    });

    elements.sort([elementToDecimal](ELEMENT_T& a, ELEMENT_T& b)
    {
        return elementToDecimal(a) < elementToDecimal(b);
    });

    // remove duplicates in ElementRanges<T>.ranges
    if (ranges.size() > 1) {
        auto it = ranges.begin();
        auto nextIt = it;
        ++nextIt;
        while (nextIt != ranges.end()) {
            if (elementToDecimal(nextIt->first) <= elementToDecimal(it->second)) {
                elementToDecimal(nextIt->first) = elementToDecimal(it->second) + 1;
            }

            if (elementToDecimal(nextIt->first) > elementToDecimal(nextIt->second)) {
                nextIt = ranges.erase(nextIt);
            }
            else if (elementToDecimal(it->second) + 1 == elementToDecimal(nextIt->first)) {
                it->second = nextIt->second;
                nextIt = ranges.erase(nextIt);
            }
            else {
                ++it;
                ++nextIt;
            }
        }
    }

    // fill ElementRanges<T>.elements without ElementRanges<T>.ranges
    for (auto it = elements.begin(); it != elements.end();) {
        ELEMENT_DECIMAL_T& nativeType = elementToDecimal(*it);

        auto fndIt = std::find_if(ranges.begin(), ranges.end(), [nativeType, elementToDecimal](auto& pr)
        {
            return nativeType >= elementToDecimal(pr.first) && nativeType <= elementToDecimal(pr.second);
        });

        // erase element
        if (fndIt != ranges.end()) {
            it = elements.erase(it);
        }
        else {
            ++it;
        }
    }
}
